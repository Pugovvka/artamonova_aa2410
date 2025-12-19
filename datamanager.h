#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include "pipe.h"
#include "station.h"
#include "gasnetwork.h"

class DataManager {
private:
    std::unordered_map<int, Pipe> pipes;
    std::unordered_map<int, CompressorStation> stations;
    std::unordered_set<int> usedPipeIds;
    std::unordered_set<int> usedStationIds;
    GasNetwork network;
    int nextPipeId = 1;
    int nextStationId = 1;

    static const std::set<int> ALLOWED_DIAMETERS;

    std::string toLower(const std::string& str);
    int generateUniqueId(std::unordered_set<int>& usedIds, int& nextId);
    void releaseId(std::unordered_set<int>& usedIds, int id);

public:
    template<typename T>
    T getValidatedNumber(const std::string& prompt, T minValue = 1, T maxValue = std::numeric_limits<T>::max());

    double getValidatedDouble(const std::string& prompt, double minValue = 0.0, double maxValue = 100.0);
    bool getConfirmation(const std::string& message);

    int findFreePipeByDiameter(int diameter);
    void connectStations();
    void removeConnection();
    void displayNetwork();
    void displayGraph();
    void performTopologicalSort();

    void displayAllPipes();
    void displayAllStations();
    std::vector<int> findPipesByName(const std::string& searchName);
    std::vector<int> findPipesByRepairStatus(bool status);
    void displayPipesByIds(const std::vector<int>& pipeIds);
    void searchPipesByName();
    void searchPipesByRepairStatus();
    void batchEditPipes();
    void batchDeletePipes();
    void batchDeleteStations();
    void searchPipesMenu();

    std::vector<int> findStationsByName();
    void searchStationsByName();
    void searchStationsByUnusedPercentage();
    void searchStationsMenu();

    void addPipe();
    void addStation();
    void editPipeStatus();
    void editStationWorkshops();
    void deletePipe();
    void deleteStation();

    void saveData();
    void loadData();

    void viewAllObjects();
    void run();
};

#endif // DATAMANAGER_H#pragma once
