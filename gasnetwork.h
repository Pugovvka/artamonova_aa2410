#ifndef GASNETWORK_H
#define GASNETWORK_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <stack>
#include <functional>

class GasNetwork {
private:
    struct Connection {
        int pipeId;
        int fromStationId;
        int toStationId;
    };

    std::unordered_map<int, std::vector<int>> adjacencyList; // Список смежности: stationId -> список соединенных станций
    std::unordered_map<int, Connection> connections; // id соединения -> Connection
    std::unordered_map<int, int> pipeToConnection; // pipeId -> connectionId
    int nextConnectionId = 1;

public:
    bool addConnection(int pipeId, int fromStationId, int toStationId);
    bool removeConnection(int connectionId);
    int findConnectionByPipeId(int pipeId);
    bool isPipeInUse(int pipeId);
    bool hasConnections(int stationId);
    std::vector<int> getStationOutgoingConnections(int stationId);
    std::vector<int> getStationIncomingConnections(int stationId);
    void displayNetwork(const std::unordered_map<int, class Pipe>& pipes,
        const std::unordered_map<int, class CompressorStation>& stations);
    std::vector<int> topologicalSort(const std::unordered_map<int, CompressorStation>& stations);
    std::vector<int> topologicalSortDFS(const std::unordered_map<int, CompressorStation>& stations);
    void displayAsGraph(const std::unordered_map<int, CompressorStation>& stations);
    void saveToFile(std::ofstream& outFile);
    void loadFromFile(std::ifstream& inFile);
    void clear();
    size_t getConnectionCount() const;
};

#endif // GASNETWORK_H#pragma once
