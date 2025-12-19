#include "gasnetwork.h"
#include "pipe.h"
#include "station.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

bool GasNetwork::addConnection(int pipeId, int fromStationId, int toStationId) {
    // Проверка на петлю
    if (fromStationId == toStationId) {
        std::cout << "Error: Cannot connect station to itself!\n";
        return false;
    }

    // Проверка на существование дублирующего соединения
    for (const auto& conn : connections) {
        if (conn.second.fromStationId == fromStationId &&
            conn.second.toStationId == toStationId) {
            std::cout << "Error: Connection already exists!\n";
            return false;
        }
    }

    int connectionId = nextConnectionId++;
    connections[connectionId] = { pipeId, fromStationId, toStationId };
    pipeToConnection[pipeId] = connectionId;

    adjacencyList[fromStationId].push_back(toStationId);

    std::cout << "Connection added successfully! (ID: " << connectionId << ")\n";
    return true;
}

bool GasNetwork::removeConnection(int connectionId) {
    auto it = connections.find(connectionId);
    if (it == connections.end()) {
        std::cout << "Error: Connection not found!\n";
        return false;
    }

    int pipeId = it->second.pipeId;
    int fromStationId = it->second.fromStationId;
    int toStationId = it->second.toStationId;

    // Удаляем из списка смежности
    auto& adjList = adjacencyList[fromStationId];
    adjList.erase(std::remove(adjList.begin(), adjList.end(), toStationId), adjList.end());

    // Удаляем из мап
    connections.erase(it);
    pipeToConnection.erase(pipeId);

    std::cout << "Connection removed successfully!\n";
    return true;
}

int GasNetwork::findConnectionByPipeId(int pipeId) {
    auto it = pipeToConnection.find(pipeId);
    if (it != pipeToConnection.end()) {
        return it->second;
    }
    return -1;
}

bool GasNetwork::isPipeInUse(int pipeId) {
    return pipeToConnection.find(pipeId) != pipeToConnection.end();
}

bool GasNetwork::hasConnections(int stationId) {
    return adjacencyList.find(stationId) != adjacencyList.end() &&
        !adjacencyList[stationId].empty();
}

std::vector<int> GasNetwork::getStationOutgoingConnections(int stationId) {
    std::vector<int> result;
    for (const auto& conn : connections) {
        if (conn.second.fromStationId == stationId) {
            result.push_back(conn.first);
        }
    }
    return result;
}

std::vector<int> GasNetwork::getStationIncomingConnections(int stationId) {
    std::vector<int> result;
    for (const auto& conn : connections) {
        if (conn.second.toStationId == stationId) {
            result.push_back(conn.first);
        }
    }
    return result;
}

void GasNetwork::displayNetwork(const std::unordered_map<int, Pipe>& pipes,
    const std::unordered_map<int, CompressorStation>& stations) {
    if (connections.empty()) {
        std::cout << "Gas network is empty.\n";
        return;
    }

    std::cout << "\n=== GAS TRANSPORT NETWORK ===\n";
    std::cout << "Total connections: " << connections.size() << "\n\n";

    for (const auto& conn : connections) {
        const Connection& c = conn.second;

        // Найти трубу
        auto pipeIt = pipes.find(c.pipeId);
        std::string pipeName = "Unknown";
        int pipeDiameter = 0;
        if (pipeIt != pipes.end()) {
            pipeName = pipeIt->second.name;
            pipeDiameter = pipeIt->second.diameter;
        }

        // Найти станции
        auto fromIt = stations.find(c.fromStationId);
        auto toIt = stations.find(c.toStationId);
        std::string fromName = "Unknown";
        std::string toName = "Unknown";

        if (fromIt != stations.end()) fromName = fromIt->second.name;
        if (toIt != stations.end()) toName = toIt->second.name;

        std::cout << "Connection ID: " << conn.first << "\n"
            << "  Pipe: " << pipeName << " (ID: " << c.pipeId
            << ", Diameter: " << pipeDiameter << " mm)\n"
            << "  From: " << fromName << " (ID: " << c.fromStationId << ")\n"
            << "  To: " << toName << " (ID: " << c.toStationId << ")\n"
            << "  ----------------------------------\n";
    }
}

std::vector<int> GasNetwork::topologicalSort(const std::unordered_map<int, CompressorStation>& stations) {
    std::vector<int> result;

    if (adjacencyList.empty()) {
        std::cout << "Network is empty, topological sort not applicable.\n";
        return result;
    }

    // 1. Вычисляем полустепени захода для всех вершин
    std::unordered_map<int, int> inDegree;

    // Инициализируем для всех станций
    for (const auto& station : stations) {
        inDegree[station.first] = 0;
    }

    // Считаем полустепени захода
    for (const auto& conn : connections) {
        inDegree[conn.second.toStationId]++;
    }

    // 2. Очередь вершин с нулевой полустепенью захода
    std::queue<int> zeroInDegreeQueue;
    for (const auto& item : inDegree) {
        if (item.second == 0) {
            zeroInDegreeQueue.push(item.first);
        }
    }

    // 3. Обработка вершин
    int processedCount = 0;
    while (!zeroInDegreeQueue.empty()) {
        int stationId = zeroInDegreeQueue.front();
        zeroInDegreeQueue.pop();
        result.push_back(stationId);

        // Уменьшаем полустепень захода для всех соседей
        if (adjacencyList.find(stationId) != adjacencyList.end()) {
            for (int neighborId : adjacencyList[stationId]) {
                inDegree[neighborId]--;
                if (inDegree[neighborId] == 0) {
                    zeroInDegreeQueue.push(neighborId);
                }
            }
        }

        processedCount++;
    }

    // 4. Проверка на наличие циклов
    if (processedCount != stations.size()) {
        std::cout << "Warning: Graph contains cycles! Topological sort may be incomplete.\n";

        // Добавляем оставшиеся вершины
        for (const auto& station : stations) {
            if (std::find(result.begin(), result.end(), station.first) == result.end()) {
                result.push_back(station.first);
            }
        }
    }

    return result;
}

std::vector<int> GasNetwork::topologicalSortDFS(const std::unordered_map<int, CompressorStation>& stations) {
    std::vector<int> result;
    std::unordered_set<int> visited;
    std::unordered_set<int> tempMark;
    std::stack<int> dfsStack;

    // Функция для посещения вершины
    std::function<bool(int)> visit = [&](int stationId) -> bool {
        if (tempMark.find(stationId) != tempMark.end()) {
            return false; // Найден цикл
        }

        if (visited.find(stationId) == visited.end()) {
            tempMark.insert(stationId);

            // Посещаем всех соседей
            if (adjacencyList.find(stationId) != adjacencyList.end()) {
                for (int neighborId : adjacencyList[stationId]) {
                    if (!visit(neighborId)) {
                        return false;
                    }
                }
            }

            tempMark.erase(stationId);
            visited.insert(stationId);
            dfsStack.push(stationId);
        }
        return true;
        };

    // Посещаем все вершины
    for (const auto& station : stations) {
        if (visited.find(station.first) == visited.end()) {
            if (!visit(station.first)) {
                std::cout << "Error: Graph contains cycles!\n";
                return std::vector<int>();
            }
        }
    }

    // Извлекаем из стека в правильном порядке
    while (!dfsStack.empty()) {
        result.push_back(dfsStack.top());
        dfsStack.pop();
    }

    return result;
}

void GasNetwork::displayAsGraph(const std::unordered_map<int, CompressorStation>& stations) {
    if (adjacencyList.empty()) {
        std::cout << "Graph is empty.\n";
        return;
    }

    std::cout << "\n=== NETWORK GRAPH (Adjacency List) ===\n";
    for (const auto& entry : adjacencyList) {
        int stationId = entry.first;
        const auto& neighbors = entry.second;

        auto it = stations.find(stationId);
        std::string stationName = (it != stations.end()) ? it->second.name : "Unknown";

        std::cout << "Station " << stationName << " (ID: " << stationId << ") -> ";

        if (neighbors.empty()) {
            std::cout << "No outgoing connections";
        }
        else {
            for (size_t i = 0; i < neighbors.size(); i++) {
                auto neighborIt = stations.find(neighbors[i]);
                std::string neighborName = (neighborIt != stations.end()) ? neighborIt->second.name : "Unknown";
                std::cout << neighborName << " (ID: " << neighbors[i] << ")";
                if (i < neighbors.size() - 1) std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
}

void GasNetwork::saveToFile(std::ofstream& outFile) {
    outFile << "[NETWORK_INFO]\n";
    outFile << nextConnectionId << "\n";

    outFile << "[CONNECTION] " << connections.size() << "\n";
    for (const auto& conn : connections) {
        outFile << conn.first << " "
            << conn.second.pipeId << " "
            << conn.second.fromStationId << " "
            << conn.second.toStationId << "\n";
    }
}

void GasNetwork::loadFromFile(std::ifstream& inFile) {
    std::string line;
    while (getline(inFile, line)) {
        if (line.find("[NETWORK_INFO]") != std::string::npos) {
            // Читаем nextConnectionId
            getline(inFile, line);
            nextConnectionId = std::stoi(line);
        }
        else if (line.find("[CONNECTION]") != std::string::npos) {
            // Читаем количество соединений
            std::stringstream ss(line);
            std::string token;
            ss >> token; // [CONNECTION]
            int count;
            ss >> count;

            // Читаем соединения
            for (int i = 0; i < count; i++) {
                getline(inFile, line);
                std::stringstream connSS(line);
                int connId, pipeId, fromId, toId;
                connSS >> connId >> pipeId >> fromId >> toId;

                connections[connId] = { pipeId, fromId, toId };
                pipeToConnection[pipeId] = connId;
                adjacencyList[fromId].push_back(toId);
            }
            break;
        }
    }
}

void GasNetwork::clear() {
    connections.clear();
    adjacencyList.clear();
    pipeToConnection.clear();
    nextConnectionId = 1;
}

size_t GasNetwork::getConnectionCount() const {
    return connections.size();
}