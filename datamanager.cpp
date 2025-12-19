#include "datamanager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <functional>

const std::set<int> DataManager::ALLOWED_DIAMETERS = { 500, 700, 1000, 1400 };

std::string DataManager::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

int DataManager::generateUniqueId(std::unordered_set<int>& usedIds, int& nextId) {
    while (usedIds.find(nextId) != usedIds.end()) {
        nextId++;
    }
    int newId = nextId;
    usedIds.insert(newId);
    nextId++;
    return newId;
}

void DataManager::releaseId(std::unordered_set<int>& usedIds, int id) {
    usedIds.erase(id);
}

// Шаблонная функция должна быть определена в заголовочном файле, 
// но мы можем оставить здесь, если добавим явную инстанциацию
template<typename T>
T DataManager::getValidatedNumber(const std::string& prompt, T minValue, T maxValue) {
    std::string input;
    T value;

    while (true) {
        std::cout << prompt;
        getline(std::cin, input);

        std::stringstream ss(input);

        if (ss >> value) {
            char remaining;
            if (ss >> remaining) {
                std::cout << "Invalid input! Please enter only a number without extra characters.\n";
                continue;
            }

            if (value < minValue || value > maxValue) {
                std::cout << "Invalid input! Please enter a number between " << minValue << " and " << maxValue << ".\n";
                continue;
            }

            return value;
        }
        else {
            std::cout << "Invalid input! Please enter a valid number.\n";
        }
    }
}

double DataManager::getValidatedDouble(const std::string& prompt, double minValue, double maxValue) {
    std::string input;
    double value;

    while (true) {
        std::cout << prompt;
        getline(std::cin, input);

        std::stringstream ss(input);

        if (ss >> value) {
            char remaining;
            if (ss >> remaining) {
                std::cout << "Invalid input! Please enter only a number without extra characters.\n";
                continue;
            }

            if (value < minValue || value > maxValue) {
                std::cout << "Invalid input! Please enter a number between " << minValue << " and " << maxValue << ".\n";
                continue;
            }

            return value;
        }
        else {
            std::cout << "Invalid input! Please enter a valid number.\n";
        }
    }
}

bool DataManager::getConfirmation(const std::string& message) {
    std::string input;
    while (true) {
        std::cout << message << " (y/n): ";
        getline(std::cin, input);

        if (input == "y" || input == "Y") {
            return true;
        }
        else if (input == "n" || input == "N") {
            return false;
        }
        else {
            std::cout << "Invalid input! Please enter 'y' or 'n'.\n";
        }
    }
}

int DataManager::findFreePipeByDiameter(int diameter) {
    for (const auto& pipePair : pipes) {
        const Pipe& pipe = pipePair.second;
        if (pipe.diameter == diameter &&
            !pipe.underRepair &&
            !network.isPipeInUse(pipe.id)) {
            return pipe.id;
        }
    }
    return -1; // Не найдена
}

void DataManager::connectStations() {
    if (stations.size() < 2) {
        std::cout << "Need at least 2 stations to create a connection!\n";
        return;
    }

    std::cout << "\n=== CONNECT STATIONS ===\n";

    // Показать все станции
    displayAllStations();

    // Выбрать станцию входа
    int fromStationId = getValidatedNumber<int>("Enter FROM station ID: ");
    if (stations.find(fromStationId) == stations.end()) {
        std::cout << "Station with ID " << fromStationId << " not found!\n";
        return;
    }

    // Выбрать станцию выхода
    int toStationId = getValidatedNumber<int>("Enter TO station ID: ");
    if (stations.find(toStationId) == stations.end()) {
        std::cout << "Station with ID " << toStationId << " not found!\n";
        return;
    }

    // Выбрать диаметр трубы
    std::cout << "\nAllowed pipe diameters: ";
    for (int d : ALLOWED_DIAMETERS) {
        std::cout << d << " ";
    }
    std::cout << "mm\n";

    int diameter = getValidatedNumber<int>("Enter pipe diameter: ");

    // Проверить, что диаметр допустимый
    if (ALLOWED_DIAMETERS.find(diameter) == ALLOWED_DIAMETERS.end()) {
        std::cout << "Invalid diameter! Allowed values: ";
        for (int d : ALLOWED_DIAMETERS) {
            std::cout << d << " ";
        }
        std::cout << "mm\n";

        if (!getConfirmation("Do you want to use the closest available diameter?")) {
            return;
        }

        // Найти ближайший допустимый диаметр
        int closest = *ALLOWED_DIAMETERS.begin();
        int minDiff = abs(diameter - closest);
        for (int d : ALLOWED_DIAMETERS) {
            int diff = abs(diameter - d);
            if (diff < minDiff) {
                minDiff = diff;
                closest = d;
            }
        }
        diameter = closest;
        std::cout << "Using diameter: " << diameter << " mm\n";
    }

    // Поиск свободной трубы
    int pipeId = findFreePipeByDiameter(diameter);

    if (pipeId == -1) {
        std::cout << "\nNo free pipes with diameter " << diameter << " mm found.\n";

        if (getConfirmation("Create a new pipe with this diameter?")) {
            // Создать новую трубу
            Pipe newPipe;
            newPipe.id = generateUniqueId(usedPipeIds, nextPipeId);
            newPipe.diameter = diameter;

            std::cout << "Enter pipe data:\n";
            std::cin >> newPipe;

            pipes[newPipe.id] = newPipe;
            pipeId = newPipe.id;

            std::cout << "New pipe created! (ID: " << pipeId << ")\n";
        }
        else {
            return;
        }
    }
    else {
        std::cout << "Found free pipe with ID: " << pipeId << "\n";
    }

    // Создать соединение
    network.addConnection(pipeId, fromStationId, toStationId);

    // Отметить трубу как используемую
    pipes[pipeId].inUse = true;
}

void DataManager::removeConnection() {
    if (network.getConnectionCount() == 0) {
        std::cout << "No connections in the network!\n";
        return;
    }

    network.displayNetwork(pipes, stations);

    int connectionId = getValidatedNumber<int>("Enter connection ID to remove: ");

    // Найти соединение
    bool removed = network.removeConnection(connectionId);
    if (removed) {
        std::cout << "Connection removed successfully!\n";
    }
}

void DataManager::displayNetwork() {
    network.displayNetwork(pipes, stations);
}

void DataManager::displayGraph() {
    network.displayAsGraph(stations);
}

void DataManager::performTopologicalSort() {
    if (stations.empty()) {
        std::cout << "No stations in the network!\n";
        return;
    }

    std::cout << "\n=== TOPOLOGICAL SORT ===\n";
    std::cout << "Choose algorithm:\n";
    std::cout << "1. Kahn's algorithm (BFS-based)\n";
    std::cout << "2. DFS-based algorithm\n";

    int choice = getValidatedNumber("Choice: ", 1, 2);

    std::vector<int> sortedStations;

    if (choice == 1) {
        sortedStations = network.topologicalSort(stations);
    }
    else {
        sortedStations = network.topologicalSortDFS(stations);
    }

    if (sortedStations.empty()) {
        std::cout << "Topological sort failed or network is empty.\n";
        return;
    }

    std::cout << "\nTopological order of stations:\n";
    for (size_t i = 0; i < sortedStations.size(); i++) {
        int stationId = sortedStations[i];
        auto it = stations.find(stationId);
        if (it != stations.end()) {
            std::cout << i + 1 << ". " << it->second.name
                << " (ID: " << stationId << ")\n";
        }
        else {
            std::cout << i + 1 << ". Unknown station (ID: " << stationId << ")\n";
        }
    }
}

void DataManager::displayAllPipes() {
    if (pipes.empty()) {
        std::cout << "No pipes available.\n";
        return;
    }

    std::cout << "\n=== ALL PIPES ===\n";
    for (const auto& pair : pipes) {
        const Pipe& pipe = pair.second;
        std::cout << "ID: " << pipe.id
            << " | Name: " << pipe.name
            << " | Length: " << pipe.length << " km"
            << " | Diameter: " << pipe.diameter << " mm"
            << " | Under repair: " << (pipe.underRepair ? "Yes" : "No")
            << " | In use: " << (network.isPipeInUse(pipe.id) ? "Yes" : "No") << "\n";
    }
}

void DataManager::displayAllStations() {
    if (stations.empty()) {
        std::cout << "No stations available.\n";
        return;
    }

    std::cout << "\n=== ALL COMPRESSOR STATIONS ===\n";
    for (const auto& pair : stations) {
        const CompressorStation& station = pair.second;
        std::cout << "ID: " << station.id
            << " | Name: " << station.name
            << " | Workshops: " << station.activeWorkshops << "/" << station.totalWorkshops
            << " | Class: " << station.stationClass
            << " | Connections: " << network.getStationOutgoingConnections(station.id).size()
            << " outgoing, " << network.getStationIncomingConnections(station.id).size() << " incoming\n";
    }
}

std::vector<int> DataManager::findPipesByName(const std::string& searchName) {
    std::vector<int> foundIds;
    std::string searchNameLower = toLower(searchName);

    for (const auto& pair : pipes) {
        if (toLower(pair.second.name).find(searchNameLower) != std::string::npos) {
            foundIds.push_back(pair.first);
        }
    }

    return foundIds;
}

std::vector<int> DataManager::findPipesByRepairStatus(bool status) {
    std::vector<int> foundIds;

    for (const auto& pair : pipes) {
        if (pair.second.underRepair == status) {
            foundIds.push_back(pair.first);
        }
    }

    return foundIds;
}

void DataManager::displayPipesByIds(const std::vector<int>& pipeIds) {
    if (pipeIds.empty()) {
        std::cout << "No pipes to display.\n";
        return;
    }

    std::cout << "\n=== FOUND PIPES ===\n";
    for (int id : pipeIds) {
        auto it = pipes.find(id);
        if (it != pipes.end()) {
            const Pipe& pipe = it->second;
            std::cout << "ID: " << pipe.id
                << " | Name: " << pipe.name
                << " | Length: " << pipe.length << " km"
                << " | Diameter: " << pipe.diameter << " mm"
                << " | Under repair: " << (pipe.underRepair ? "Yes" : "No")
                << " | In use: " << (network.isPipeInUse(pipe.id) ? "Yes" : "No") << "\n";
        }
    }
    std::cout << "Total found: " << pipeIds.size() << " pipe(s)\n";
}

void DataManager::searchPipesByName() {
    if (pipes.empty()) {
        std::cout << "No pipes available to search!\n";
        return;
    }

    std::string searchName;
    std::cout << "Enter pipe name to search for: ";
    getline(std::cin, searchName);

    std::vector<int> foundIds = findPipesByName(searchName);

    if (foundIds.empty()) {
        std::cout << "No pipes found with name containing: " << searchName << "\n";
        return;
    }

    displayPipesByIds(foundIds);
}

void DataManager::searchPipesByRepairStatus() {
    if (pipes.empty()) {
        std::cout << "No pipes available to search!\n";
        return;
    }

    std::cout << "Search for pipes:\n";
    std::cout << "1. Under repair\n";
    std::cout << "2. Operational\n";
    int choice = getValidatedNumber("Choose status: ", 1, 2);

    bool searchStatus = (choice == 1);
    std::vector<int> foundIds = findPipesByRepairStatus(searchStatus);

    if (foundIds.empty()) {
        std::cout << "No pipes found with the selected status.\n";
        return;
    }

    displayPipesByIds(foundIds);
}

void DataManager::batchEditPipes() {
    if (pipes.empty()) {
        std::cout << "No pipes available to edit!\n";
        return;
    }

    std::cout << "\n=== BATCH PIPE EDITING ===\n";
    std::cout << "1. Search by name\n";
    std::cout << "2. Search by repair status\n";
    std::cout << "0. Back to main menu\n";

    int choice = getValidatedNumber("Choose search type: ", 0, 2);

    std::vector<int> foundIds;

    switch (choice) {
    case 1: {
        std::string searchName;
        std::cout << "Enter pipe name to search for: ";
        getline(std::cin, searchName);
        foundIds = findPipesByName(searchName);
        break;
    }
    case 2: {
        std::cout << "Search for pipes:\n";
        std::cout << "1. Under repair\n";
        std::cout << "2. Operational\n";
        int statusChoice = getValidatedNumber("Choose status: ", 1, 2);
        foundIds = findPipesByRepairStatus(statusChoice == 1);
        break;
    }
    case 0:
        return;
    }

    if (foundIds.empty()) {
        std::cout << "No pipes found with the selected criteria.\n";
        return;
    }

    displayPipesByIds(foundIds);

    std::cout << "\nBatch editing options:\n";
    std::cout << "1. Edit all found pipes\n";
    std::cout << "2. Select specific pipes to edit\n";
    std::cout << "0. Cancel\n";

    int editChoice = getValidatedNumber("Choose editing mode: ", 0, 2);

    if (editChoice == 0) {
        return;
    }

    std::vector<int> pipesToEdit;

    if (editChoice == 1) {
        pipesToEdit = foundIds;
        std::cout << "Selected all " << foundIds.size() << " pipes for editing.\n";
    }
    else if (editChoice == 2) {
        std::cout << "Enter pipe IDs to edit (separated by spaces): ";
        std::string input;
        getline(std::cin, input);

        std::stringstream ss(input);
        int id;
        std::set<int> selectedIds;

        while (ss >> id) {
            if (std::find(foundIds.begin(), foundIds.end(), id) != foundIds.end()) {
                selectedIds.insert(id);
            }
            else {
                std::cout << "Pipe ID " << id << " not found in search results. Skipping.\n";
            }
        }

        if (selectedIds.empty()) {
            std::cout << "No valid pipe IDs selected.\n";
            return;
        }

        pipesToEdit.assign(selectedIds.begin(), selectedIds.end());
        std::cout << "Selected " << pipesToEdit.size() << " pipes for editing.\n";
    }

    std::cout << "\nChoose editing action:\n";
    std::cout << "1. Mark as under repair\n";
    std::cout << "2. Mark as operational\n";
    std::cout << "3. Toggle repair status (swap current status)\n";

    int action = getValidatedNumber("Choose action: ", 1, 3);

    int changedCount = 0;
    for (int id : pipesToEdit) {
        auto it = pipes.find(id);
        if (it != pipes.end()) {
            Pipe& pipe = it->second;
            bool oldStatus = pipe.underRepair;

            switch (action) {
            case 1:
                pipe.underRepair = true;
                break;
            case 2:
                pipe.underRepair = false;
                break;
            case 3:
                pipe.underRepair = !pipe.underRepair;
                break;
            }

            if (oldStatus != pipe.underRepair) {
                changedCount++;
            }
        }
    }

    std::cout << "Successfully updated repair status for " << changedCount << " pipes.\n";

    if (getConfirmation("Show updated pipes?")) {
        displayPipesByIds(pipesToEdit);
    }
}

void DataManager::batchDeletePipes() {
    if (pipes.empty()) {
        std::cout << "No pipes available to delete!\n";
        return;
    }

    std::cout << "\n=== BATCH PIPE DELETION ===\n";
    std::cout << "1. Search by name\n";
    std::cout << "2. Search by repair status\n";
    std::cout << "0. Back to main menu\n";

    int choice = getValidatedNumber("Choose search type: ", 0, 2);

    std::vector<int> foundIds;

    switch (choice) {
    case 1: {
        std::string searchName;
        std::cout << "Enter pipe name to search for: ";
        getline(std::cin, searchName);
        foundIds = findPipesByName(searchName);
        break;
    }
    case 2: {
        std::cout << "Search for pipes:\n";
        std::cout << "1. Under repair\n";
        std::cout << "2. Operational\n";
        int statusChoice = getValidatedNumber("Choose status: ", 1, 2);
        foundIds = findPipesByRepairStatus(statusChoice == 1);
        break;
    }
    case 0:
        return;
    }

    if (foundIds.empty()) {
        std::cout << "No pipes found with the selected criteria.\n";
        return;
    }

    // Проверить, есть ли используемые трубы
    std::vector<int> pipesInUse;
    for (int id : foundIds) {
        if (network.isPipeInUse(id)) {
            pipesInUse.push_back(id);
        }
    }

    if (!pipesInUse.empty()) {
        std::cout << "\nWarning: " << pipesInUse.size() << " pipe(s) are used in the network:\n";
        for (int id : pipesInUse) {
            auto it = pipes.find(id);
            if (it != pipes.end()) {
                std::cout << "  Pipe ID: " << id << " - " << it->second.name << "\n";
            }
        }

        if (!getConfirmation("Delete them anyway? This will remove connections using these pipes.")) {
            return;
        }

        // Удалить соединения, использующие эти трубы
        for (int pipeId : pipesInUse) {
            int connId = network.findConnectionByPipeId(pipeId);
            if (connId != -1) {
                network.removeConnection(connId);
            }
        }
    }

    displayPipesByIds(foundIds);

    if (getConfirmation("Delete all these pipes?")) {
        for (int id : foundIds) {
            pipes.erase(id);
            releaseId(usedPipeIds, id);
        }
        std::cout << "Successfully deleted " << foundIds.size() << " pipes.\n";
    }
}

void DataManager::batchDeleteStations() {
    if (stations.empty()) {
        std::cout << "No stations available to delete!\n";
        return;
    }

    std::cout << "\n=== BATCH STATION DELETION ===\n";
    std::vector<int> foundIds = findStationsByName();

    if (foundIds.empty()) {
        std::cout << "No stations found with the specified name.\n";
        return;
    }

    // Проверить, есть ли станции с соединениями
    std::vector<int> stationsWithConnections;
    for (int id : foundIds) {
        if (network.hasConnections(id)) {
            stationsWithConnections.push_back(id);
        }
    }

    if (!stationsWithConnections.empty()) {
        std::cout << "\nWarning: " << stationsWithConnections.size()
            << " station(s) have connections in the network:\n";
        for (int id : stationsWithConnections) {
            auto it = stations.find(id);
            if (it != stations.end()) {
                std::cout << "  Station ID: " << id << " - " << it->second.name << "\n";
            }
        }

        if (!getConfirmation("Delete them anyway? This will remove all connections involving these stations.")) {
            return;
        }

        // Удалить все соединения с этими станциями
        for (int stationId : stationsWithConnections) {
            // Удалить исходящие соединения
            std::vector<int> outgoing = network.getStationOutgoingConnections(stationId);
            for (int connId : outgoing) {
                network.removeConnection(connId);
            }

            // Удалить входящие соединения
            std::vector<int> incoming = network.getStationIncomingConnections(stationId);
            for (int connId : incoming) {
                network.removeConnection(connId);
            }
        }
    }

    std::cout << "\n=== FOUND STATIONS ===\n";
    for (int id : foundIds) {
        auto it = stations.find(id);
        if (it != stations.end()) {
            std::cout << it->second;
        }
    }

    if (getConfirmation("Delete all these stations?")) {
        for (int id : foundIds) {
            stations.erase(id);
            releaseId(usedStationIds, id);
        }
        std::cout << "Successfully deleted " << foundIds.size() << " stations.\n";
    }
}

void DataManager::searchPipesMenu() {
    if (pipes.empty()) {
        std::cout << "No pipes available to search!\n";
        return;
    }

    std::cout << "\n=== PIPE SEARCH ===\n";
    std::cout << "1. Search by name\n";
    std::cout << "2. Search by repair status\n";
    std::cout << "0. Back to main menu\n";

    int choice = getValidatedNumber("Choose search type: ", 0, 2);

    switch (choice) {
    case 1:
        searchPipesByName();
        break;
    case 2:
        searchPipesByRepairStatus();
        break;
    case 0:
        return;
    }
}

std::vector<int> DataManager::findStationsByName() {
    std::vector<int> foundIds;
    if (stations.empty()) {
        return foundIds;
    }

    std::string searchName;
    std::cout << "Enter station name to search for: ";
    getline(std::cin, searchName);

    std::string searchNameLower = toLower(searchName);

    for (const auto& pair : stations) {
        if (toLower(pair.second.name).find(searchNameLower) != std::string::npos) {
            foundIds.push_back(pair.first);
        }
    }

    return foundIds;
}

void DataManager::searchStationsByName() {
    if (stations.empty()) {
        std::cout << "No stations available to search!\n";
        return;
    }

    std::vector<int> foundIds = findStationsByName();

    if (foundIds.empty()) {
        std::cout << "No stations found with the specified name.\n";
        return;
    }

    std::cout << "\n=== FOUND STATIONS ===\n";
    for (int id : foundIds) {
        auto it = stations.find(id);
        if (it != stations.end()) {
            std::cout << it->second;
        }
    }
    std::cout << "Total found: " << foundIds.size() << " station(s)\n";
}

void DataManager::searchStationsByUnusedPercentage() {
    if (stations.empty()) {
        std::cout << "No stations available to search!\n";
        return;
    }

    std::cout << "Search stations by percentage of unused workshops (0-100%)\n";
    double minPercentage = getValidatedDouble("Enter minimum percentage: ", 0.0, 100.0);
    double maxPercentage = getValidatedDouble("Enter maximum percentage: ", minPercentage, 100.0);

    std::vector<int> foundIds;

    for (const auto& pair : stations) {
        const CompressorStation& station = pair.second;
        if (station.totalWorkshops > 0) {
            double unusedPercentage = (1.0 - (double)station.activeWorkshops / station.totalWorkshops) * 100.0;
            if (unusedPercentage >= minPercentage && unusedPercentage <= maxPercentage) {
                foundIds.push_back(pair.first);
            }
        }
    }

    if (foundIds.empty()) {
        std::cout << "No stations found with unused workshops percentage between "
            << minPercentage << "% and " << maxPercentage << "%\n";
        return;
    }

    std::cout << "\n=== FOUND STATIONS ===\n";
    for (int id : foundIds) {
        auto it = stations.find(id);
        if (it != stations.end()) {
            const CompressorStation& station = it->second;
            double unusedPercentage = (1.0 - (double)station.activeWorkshops / station.totalWorkshops) * 100.0;

            std::cout << "ID: " << station.id
                << " | Name: " << station.name
                << " | Workshops: " << station.activeWorkshops << "/" << station.totalWorkshops
                << " | Unused: " << unusedPercentage << "%"
                << " | Class: " << station.stationClass << "\n";
        }
    }
    std::cout << "Total found: " << foundIds.size() << " station(s)\n";
}

void DataManager::searchStationsMenu() {
    if (stations.empty()) {
        std::cout << "No stations available to search!\n";
        return;
    }

    std::cout << "\n=== STATION SEARCH ===\n";
    std::cout << "1. Search by name\n";
    std::cout << "2. Search by percentage of unused workshops\n";
    std::cout << "0. Back to main menu\n";

    int choice = getValidatedNumber("Choose search type: ", 0, 2);

    switch (choice) {
    case 1:
        searchStationsByName();
        break;
    case 2:
        searchStationsByUnusedPercentage();
        break;
    case 0:
        return;
    }
}

void DataManager::addPipe() {
    Pipe newPipe;
    newPipe.id = generateUniqueId(usedPipeIds, nextPipeId);

    std::cout << "Enter pipe data:\n";
    std::cin >> newPipe;

    pipes[newPipe.id] = newPipe;
    std::cout << "Pipe added successfully! (ID: " << newPipe.id << ")\n";
}

void DataManager::addStation() {
    CompressorStation newStation;
    newStation.id = generateUniqueId(usedStationIds, nextStationId);

    std::cout << "Enter station data:\n";
    std::cin >> newStation;

    stations[newStation.id] = newStation;
    std::cout << "Station added successfully! (ID: " << newStation.id << ")\n";
}

void DataManager::editPipeStatus() {
    if (pipes.empty()) {
        std::cout << "No pipes available to edit!\n";
        return;
    }

    displayAllPipes();
    int pipeId = getValidatedNumber<int>("\nEnter pipe ID to edit: ");

    auto it = pipes.find(pipeId);
    if (it == pipes.end()) {
        std::cout << "Pipe with ID " << pipeId << " not found!\n";
        return;
    }

    Pipe& pipe = it->second;
    std::cout << "Current repair status: " << (pipe.underRepair ? "Under repair" : "Operational") << std::endl;

    // Проверить, используется ли труба в сети
    if (network.isPipeInUse(pipeId)) {
        std::cout << "Warning: This pipe is used in the gas network!\n";
        if (!getConfirmation("Change repair status anyway?")) {
            return;
        }
    }

    if (getConfirmation("Change repair status?")) {
        pipe.underRepair = !pipe.underRepair;
        std::cout << "Status changed successfully!\n";
    }
}

void DataManager::editStationWorkshops() {
    if (stations.empty()) {
        std::cout << "No stations available to edit!\n";
        return;
    }

    displayAllStations();
    int stationId = getValidatedNumber<int>("\nEnter station ID to edit: ");

    auto it = stations.find(stationId);
    if (it == stations.end()) {
        std::cout << "Station with ID " << stationId << " not found!\n";
        return;
    }

    CompressorStation& station = it->second;
    std::cout << "Current workshops: " << station.activeWorkshops << "/" << station.totalWorkshops << " active\n";
    std::cout << "1. Start workshop\n2. Stop workshop\nChoose action: ";

    int action = getValidatedNumber("", 1, 2);
    unsigned int changeAmount = getValidatedNumber<unsigned int>("Enter number of workshops: ", 1);

    if (action == 1) {
        if (station.activeWorkshops + changeAmount <= station.totalWorkshops) {
            station.activeWorkshops += changeAmount;
            std::cout << changeAmount << " workshop(s) started\n";
        }
        else {
            std::cout << "Cannot start more than " << station.totalWorkshops - station.activeWorkshops << " workshops\n";
        }
    }
    else {
        if (changeAmount <= station.activeWorkshops) {
            station.activeWorkshops -= changeAmount;
            std::cout << changeAmount << " workshop(s) stopped\n";
        }
        else {
            std::cout << "Cannot stop more than " << station.activeWorkshops << " workshops\n";
        }
    }
}

void DataManager::deletePipe() {
    if (pipes.empty()) {
        std::cout << "No pipes available to delete!\n";
        return;
    }

    displayAllPipes();
    int pipeId = getValidatedNumber<int>("\nEnter pipe ID to delete: ");

    auto it = pipes.find(pipeId);
    if (it == pipes.end()) {
        std::cout << "Pipe with ID " << pipeId << " not found!\n";
        return;
    }

    // Проверить, используется ли труба в сети
    if (network.isPipeInUse(pipeId)) {
        std::cout << "Error: This pipe is used in the gas network!\n";
        std::cout << "You must remove the connection first.\n";
        return;
    }

    std::cout << "You are about to delete pipe: " << it->second.name << " (ID: " << pipeId << ")\n";
    if (getConfirmation("Are you sure?")) {
        pipes.erase(it);
        releaseId(usedPipeIds, pipeId);
        std::cout << "Pipe deleted successfully!\n";
    }
}

void DataManager::deleteStation() {
    if (stations.empty()) {
        std::cout << "No stations available to delete!\n";
        return;
    }

    displayAllStations();
    int stationId = getValidatedNumber<int>("\nEnter station ID to delete: ");

    auto it = stations.find(stationId);
    if (it == stations.end()) {
        std::cout << "Station with ID " << stationId << " not found!\n";
        return;
    }

    // Проверить, есть ли у станции соединения
    if (network.hasConnections(stationId)) {
        std::cout << "Error: This station has connections in the gas network!\n";
        std::cout << "You must remove all connections first.\n";
        return;
    }

    std::cout << "You are about to delete station: " << it->second.name << " (ID: " << stationId << ")\n";
    if (getConfirmation("Are you sure?")) {
        stations.erase(it);
        releaseId(usedStationIds, stationId);
        std::cout << "Station deleted successfully!\n";
    }
}

void DataManager::saveData() {
    std::string filename;
    std::cout << "Enter filename to save (without extension): ";
    getline(std::cin, filename);
    filename += ".txt";

    std::ifstream testFile(filename);
    if (testFile.good()) {
        testFile.close();
        if (!getConfirmation("File already exists. Overwrite?")) {
            std::cout << "Save cancelled.\n";
            return;
        }
    }

    std::ofstream outFile(filename);
    if (!outFile) {
        std::cout << "Error: Could not create file " << filename << std::endl;
        return;
    }

    outFile << "[NEXT_PIPE_ID]" << std::endl << nextPipeId << std::endl;
    outFile << "[NEXT_STATION_ID]" << std::endl << nextStationId << std::endl;

    outFile << "[USED_PIPE_IDS]" << std::endl;
    for (int id : usedPipeIds) {
        outFile << id << " ";
    }
    outFile << std::endl;

    outFile << "[USED_STATION_IDS]" << std::endl;
    for (int id : usedStationIds) {
        outFile << id << " ";
    }
    outFile << std::endl;

    outFile << "[PIPES_COUNT]" << std::endl << pipes.size() << std::endl;
    for (const auto& pair : pipes) {
        outFile << "[PIPE]" << std::endl;
        outFile << pair.second;
    }

    outFile << "[STATIONS_COUNT]" << std::endl << stations.size() << std::endl;
    for (const auto& pair : stations) {
        outFile << "[STATION]" << std::endl;
        outFile << pair.second;
    }

    // Сохраняем сеть
    network.saveToFile(outFile);

    outFile.close();
    std::cout << "Data successfully saved to " << filename << std::endl;
    std::cout << "Saved: " << pipes.size() << " pipes, " << stations.size()
        << " stations, " << network.getConnectionCount() << " connections\n";
}

void DataManager::loadData() {
    std::string filename;
    std::cout << "Enter filename to load (without extension): ";
    getline(std::cin, filename);
    filename += ".txt";

    std::ifstream inFile(filename);
    if (!inFile) {
        std::cout << "Error: Could not open file " << filename << std::endl;
        return;
    }

    if (!pipes.empty() || !stations.empty()) {
        if (!getConfirmation("Current data will be overwritten. Continue?")) {
            std::cout << "Load cancelled.\n";
            inFile.close();
            return;
        }
    }

    pipes.clear();
    stations.clear();
    usedPipeIds.clear();
    usedStationIds.clear();
    network.clear();

    std::string line;
    bool readingPipes = false;
    bool readingStations = false;
    int pipesCount = 0;
    int stationsCount = 0;
    int pipesRead = 0;
    int stationsRead = 0;

    while (getline(inFile, line)) {
        if (line == "[NEXT_PIPE_ID]") {
            std::string idStr;
            getline(inFile, idStr);
            nextPipeId = std::stoi(idStr);
        }
        else if (line == "[NEXT_STATION_ID]") {
            std::string idStr;
            getline(inFile, idStr);
            nextStationId = std::stoi(idStr);
        }
        else if (line == "[USED_PIPE_IDS]") {
            std::string idsStr;
            getline(inFile, idsStr);
            std::stringstream ss(idsStr);
            int id;
            while (ss >> id) {
                usedPipeIds.insert(id);
            }
        }
        else if (line == "[USED_STATION_IDS]") {
            std::string idsStr;
            getline(inFile, idsStr);
            std::stringstream ss(idsStr);
            int id;
            while (ss >> id) {
                usedStationIds.insert(id);
            }
        }
        else if (line == "[PIPES_COUNT]") {
            std::string countStr;
            getline(inFile, countStr);
            pipesCount = std::stoi(countStr);
            readingPipes = true;
            readingStations = false;
        }
        else if (line == "[STATIONS_COUNT]") {
            std::string countStr;
            getline(inFile, countStr);
            stationsCount = std::stoi(countStr);
            readingStations = true;
            readingPipes = false;
        }
        else if (line == "[PIPE]" && readingPipes) {
            Pipe pipe;
            inFile >> pipe;
            pipes[pipe.id] = pipe;
            pipesRead++;
        }
        else if (line == "[STATION]" && readingStations) {
            CompressorStation station;
            inFile >> station;
            stations[station.id] = station;
            stationsRead++;
        }
        else if (line.find("[NETWORK_INFO]") != std::string::npos) {
            // Загружаем сеть
            network.loadFromFile(inFile);
            break;
        }
    }

    inFile.close();
    std::cout << "Data successfully loaded from " << filename << std::endl;
    std::cout << "Loaded: " << pipesRead << " pipes, " << stationsRead
        << " stations, " << network.getConnectionCount() << " connections\n";
    std::cout << "Next available IDs - Pipe: " << nextPipeId << ", Station: " << nextStationId << std::endl;
}

void DataManager::viewAllObjects() {
    std::cout << "\n=== CURRENT STATE ===\n";
    displayAllPipes();
    displayAllStations();
    displayNetwork();
}

void DataManager::run() {
    int choice = -1;

    while (true) {
        std::cout << "\n=== GAS TRANSPORT NETWORK MANAGER ===\n"
            << "1. Add Pipe\n"
            << "2. Add Compressor Station\n"
            << "3. View All Objects\n"
            << "4. Edit Pipe Status\n"
            << "5. Edit Station Workshops\n"
            << "6. Delete Pipe\n"
            << "7. Delete Station\n"
            << "8. Search Pipes\n"
            << "9. Search Stations\n"
            << "10. Batch Edit Pipes\n"
            << "11. Batch Delete Pipes\n"
            << "12. Batch Delete Stations\n"
            << "13. Connect Stations\n"
            << "14. Remove Connection\n"
            << "15. View Network\n"
            << "16. View Network Graph\n"
            << "17. Topological Sort\n"
            << "18. Save Data\n"
            << "19. Load Data\n"
            << "0. Exit\n"
            << "Choose action: ";

        std::string input;
        getline(std::cin, input);
        std::stringstream ss(input);

        if (!(ss >> choice)) {
            std::cout << "Invalid input! Please enter a number.\n";
            continue;
        }

        char remaining;
        if (ss >> remaining) {
            std::cout << "Invalid input! Please enter only a number without extra characters.\n";
            continue;
        }

        switch (choice) {
        case 1:
            addPipe();
            break;

        case 2:
            addStation();
            break;

        case 3:
            viewAllObjects();
            break;

        case 4:
            editPipeStatus();
            break;

        case 5:
            editStationWorkshops();
            break;

        case 6:
            deletePipe();
            break;

        case 7:
            deleteStation();
            break;

        case 8:
            searchPipesMenu();
            break;

        case 9:
            searchStationsMenu();
            break;

        case 10:
            batchEditPipes();
            break;

        case 11:
            batchDeletePipes();
            break;

        case 12:
            batchDeleteStations();
            break;

        case 13:
            connectStations();
            break;

        case 14:
            removeConnection();
            break;

        case 15:
            displayNetwork();
            break;

        case 16:
            displayGraph();
            break;

        case 17:
            performTopologicalSort();
            break;

        case 18:
            saveData();
            break;

        case 19:
            loadData();
            break;

        case 0:
            std::cout << "Exiting program...\n";
            return;

        default:
            std::cout << "Invalid choice! Try again.\n";
        }
    }
}