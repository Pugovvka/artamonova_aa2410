#include <iostream>
#include <string>
#include <limits>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <cctype>
#include <set>
#include <map>
#include <unordered_map>

using namespace std;

const string PIPE_IDENTIFIER = "[PIPE]";
const string STATION_IDENTIFIER = "[STATION]";

class Pipe {
public:
    int id = 0;
    string name = "";
    int length = 0;
    int diameter = 0;
    bool underRepair = false;

    friend ostream& operator<<(ostream& out, const Pipe& pipe);
    friend istream& operator>>(istream& in, Pipe& pipe);
};

class CompressorStation {
public:
    int id = 0;
    string name = "";
    unsigned int totalWorkshops = 0;
    unsigned int activeWorkshops = 0;
    int stationClass = 0;

    friend ostream& operator<<(ostream& out, const CompressorStation& station);
    friend istream& operator>>(istream& in, CompressorStation& station);
};

class DataManager {
private:
    unordered_map<int, Pipe> pipes;
    unordered_map<int, CompressorStation> stations;
    unordered_set<int> usedPipeIds;
    unordered_set<int> usedStationIds;
    int nextPipeId = 1;
    int nextStationId = 1;

    string toLower(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    int generateUniqueId(unordered_set<int>& usedIds, int& nextId) {
        while (usedIds.find(nextId) != usedIds.end()) {
            nextId++;
        }
        int newId = nextId;
        usedIds.insert(newId);
        nextId++;
        return newId;
    }

    void releaseId(unordered_set<int>& usedIds, int id) {
        usedIds.erase(id);
    }

public:
    template<typename T>
    T getValidatedNumber(const string& prompt, T minValue = 1, T maxValue = numeric_limits<T>::max()) {
        string input;
        T value;

        while (true) {
            cout << prompt;
            getline(cin, input);

            stringstream ss(input);

            if (ss >> value) {
                char remaining;
                if (ss >> remaining) {
                    cout << "Invalid input! Please enter only a number without extra characters.\n";
                    continue;
                }

                if (value < minValue  value > maxValue) {
                    cout << "Invalid input! Please enter a number between " << minValue << " and " << maxValue << ".\n";
                    continue;
                }

                return value;
            }
            else {
                cout << "Invalid input! Please enter a valid number.\n";
            }
        }
    }

    double getValidatedDouble(const string& prompt, double minValue = 0.0, double maxValue = 100.0) {
        string input;
        double value;

        while (true) {
            cout << prompt;
            getline(cin, input);

            stringstream ss(input);

            if (ss >> value) {
                char remaining;
                if (ss >> remaining) {
                    cout << "Invalid input! Please enter only a number without extra characters.\n";
                    continue;
                }

                if (value < minValue  value > maxValue) {
                    cout << "Invalid input! Please enter a number between " << minValue << " and " << maxValue << ".\n";
                    continue;
                }

                return value;
            }
            else {
                cout << "Invalid input! Please enter a valid number.\n";
            }
        }
    }

    bool getConfirmation(const string& message) {
        string input;
        while (true) {
            cout << message << " (y/n): ";
            getline(cin, input);

            if (input == "y"  input == "Y") {
                return true;
            }
            else if (input == "n"  input == "N") {
                return false;
            }
            else {
                cout << "Invalid input! Please enter 'y' or 'n'.\n";
            }
        }
    }
void displayAllPipes() {
        if (pipes.empty()) {
            cout << "No pipes available.\n";
            return;
        }

        cout << "\n=== ALL PIPES ===\n";
        for (const auto& pair : pipes) {
            const Pipe& pipe = pair.second;
            cout << pipe;
        }
    }

    void displayAllStations() {
        if (stations.empty()) {
            cout << "No stations available.\n";
            return;
        }

        cout << "\n=== ALL COMPRESSOR STATIONS ===\n";
        for (const auto& pair : stations) {
            const CompressorStation& station = pair.second;
            cout << station;
        }
    }

    vector<int> findPipesByName(const string& searchName) {
        vector<int> foundIds;
        string searchNameLower = toLower(searchName);
        
        for (const auto& pair : pipes) {
            if (toLower(pair.second.name).find(searchNameLower) != string::npos) {
                foundIds.push_back(pair.first);
            }
        }
        
        return foundIds;
    }

    vector<int> findPipesByRepairStatus(bool status) {
        vector<int> foundIds;
        
        for (const auto& pair : pipes) {
            if (pair.second.underRepair == status) {
                foundIds.push_back(pair.first);
            }
        }
        
        return foundIds;
    }

    void displayPipesByIds(const vector<int>& pipeIds) {
        if (pipeIds.empty()) {
            cout << "No pipes to display.\n";
            return;
        }

        cout << "\n=== FOUND PIPES ===\n";
        for (int id : pipeIds) {
            auto it = pipes.find(id);
            if (it != pipes.end()) {
                cout << it->second;
            }
        }
        cout << "Total found: " << pipeIds.size() << " pipe(s)\n";
    }

    void searchPipesByName() {
        if (pipes.empty()) {
            cout << "No pipes available to search!\n";
            return;
        }

        string searchName;
        cout << "Enter pipe name to search for: ";
        getline(cin, searchName);
        
        vector<int> foundIds = findPipesByName(searchName);
        
        if (foundIds.empty()) {
            cout << "No pipes found with name containing: " << searchName << "\n";
            return;
        }
        
        displayPipesByIds(foundIds);
    }

    void searchPipesByRepairStatus() {
        if (pipes.empty()) {
            cout << "No pipes available to search!\n";
            return;
        }

        cout << "Search for pipes:\n";
        cout << "1. Under repair\n";
        cout << "2. Operational\n";
        int choice = getValidatedNumber("Choose status: ", 1, 2);
        
        bool searchStatus = (choice == 1);
        vector<int> foundIds = findPipesByRepairStatus(searchStatus);
        
        if (foundIds.empty()) {
            cout << "No pipes found with the selected status.\n";
            return;
        }
        
        displayPipesByIds(foundIds);
    }

    void batchEditPipes() {
        if (pipes.empty()) {
            cout << "No pipes available to edit!\n";
            return;
        }
cout << "\n=== BATCH PIPE EDITING ===\n";
        cout << "1. Search by name\n";
        cout << "2. Search by repair status\n";
        cout << "0. Back to main menu\n";
        
        int choice = getValidatedNumber("Choose search type: ", 0, 2);
        
        vector<int> foundIds;
        
        switch (choice) {
            case 1: {
                string searchName;
                cout << "Enter pipe name to search for: ";
                getline(cin, searchName);
                foundIds = findPipesByName(searchName);
                break;
            }
            case 2: {
                cout << "Search for pipes:\n";
                cout << "1. Under repair\n";
                cout << "2. Operational\n";
                int statusChoice = getValidatedNumber("Choose status: ", 1, 2);
                foundIds = findPipesByRepairStatus(statusChoice == 1);
                break;
            }
            case 0:
                return;
        }
        
        if (foundIds.empty()) {
            cout << "No pipes found with the selected criteria.\n";
            return;
        }
        
        displayPipesByIds(foundIds);
        
        cout << "\nBatch editing options:\n";
        cout << "1. Edit all found pipes\n";
        cout << "2. Select specific pipes to edit\n";
        cout << "0. Cancel\n";
        
        int editChoice = getValidatedNumber("Choose editing mode: ", 0, 2);
        
        if (editChoice == 0) {
            return;
        }
        
        vector<int> pipesToEdit;
        
        if (editChoice == 1) {
            pipesToEdit = foundIds;
            cout << "Selected all " << foundIds.size() << " pipes for editing.\n";
        } else if (editChoice == 2) {
            cout << "Enter pipe IDs to edit (separated by spaces): ";
            string input;
            getline(cin, input);
            
            stringstream ss(input);
            int id;
            set<int> selectedIds;
            
            while (ss >> id) {
                if (find(foundIds.begin(), foundIds.end(), id) != foundIds.end()) {
                    selectedIds.insert(id);
                } else {
                    cout << "Pipe ID " << id << " not found in search results. Skipping.\n";
                }
            }
            
            if (selectedIds.empty()) {
                cout << "No valid pipe IDs selected.\n";
                return;
            }
            
            pipesToEdit.assign(selectedIds.begin(), selectedIds.end());
            cout << "Selected " << pipesToEdit.size() << " pipes for editing.\n";
        }
        
        cout << "\nChoose editing action:\n";
        cout << "1. Mark as under repair\n";
        cout << "2. Mark as operational\n";
        cout << "3. Toggle repair status (swap current status)\n";
        
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
        
        cout << "Successfully updated repair status for " << changedCount << " pipes.\n";
        
        if (getConfirmation("Show updated pipes?")) {
            displayPipesByIds(pipesToEdit);
        }
    }

    void batchDeletePipes() {
        if (pipes.empty()) {
            cout << "No pipes available to delete!\n";
            return;
        }
cout << "\n=== BATCH PIPE DELETION ===\n";
        cout << "1. Search by name\n";
        cout << "2. Search by repair status\n";
        cout << "0. Back to main menu\n";
        
        int choice = getValidatedNumber("Choose search type: ", 0, 2);
        
        vector<int> foundIds;
        
        switch (choice) {
            case 1: {
                string searchName;
                cout << "Enter pipe name to search for: ";
                getline(cin, searchName);
                foundIds = findPipesByName(searchName);
                break;
            }
            case 2: {
                cout << "Search for pipes:\n";
                cout << "1. Under repair\n";
                cout << "2. Operational\n";
                int statusChoice = getValidatedNumber("Choose status: ", 1, 2);
                foundIds = findPipesByRepairStatus(statusChoice == 1);
                break;
            }
            case 0:
                return;
        }
        
        if (foundIds.empty()) {
            cout << "No pipes found with the selected criteria.\n";
            return;
        }
        
        displayPipesByIds(foundIds);
        
        if (getConfirmation("Delete all these pipes?")) {
            for (int id : foundIds) {
                pipes.erase(id);
                releaseId(usedPipeIds, id);
            }
            cout << "Successfully deleted " << foundIds.size() << " pipes.\n";
        }
    }

    void batchDeleteStations() {
        if (stations.empty()) {
            cout << "No stations available to delete!\n";
            return;
        }

        cout << "\n=== BATCH STATION DELETION ===\n";
        vector<int> foundIds = findStationsByName();
        
        if (foundIds.empty()) {
            cout << "No stations found with the specified name.\n";
            return;
        }
        
        cout << "\n=== FOUND STATIONS ===\n";
        for (int id : foundIds) {
            auto it = stations.find(id);
            if (it != stations.end()) {
                cout << it->second;
            }
        }
        
        if (getConfirmation("Delete all these stations?")) {
            for (int id : foundIds) {
                stations.erase(id);
                releaseId(usedStationIds, id);
            }
            cout << "Successfully deleted " << foundIds.size() << " stations.\n";
        }
    }

    void searchPipesMenu() {
        if (pipes.empty()) {
            cout << "No pipes available to search!\n";
            return;
        }

        cout << "\n=== PIPE SEARCH ===\n";
        cout << "1. Search by name\n";
        cout << "2. Search by repair status\n";
        cout << "0. Back to main menu\n";
        
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

    vector<int> findStationsByName() {
        vector<int> foundIds;
        if (stations.empty()) {
            return foundIds;
        }

        string searchName;
        cout << "Enter station name to search for: ";
        getline(cin, searchName);
        
        string searchNameLower = toLower(searchName);
        
        for (const auto& pair : stations) {
            if (toLower(pair.second.name).find(searchNameLower) != string::npos) {
                foundIds.push_back(pair.first);
            }
        }
        
        return foundIds;
    }

    void searchStationsByName() {
        if (stations.empty()) {
            cout << "No stations available to search!\n";
            return;
        }
vector<int> foundIds = findStationsByName();
        
        if (foundIds.empty()) {
            cout << "No stations found with the specified name.\n";
            return;
        }
        
        cout << "\n=== FOUND STATIONS ===\n";
        for (int id : foundIds) {
            auto it = stations.find(id);
            if (it != stations.end()) {
                cout << it->second;
            }
        }
        cout << "Total found: " << foundIds.size() << " station(s)\n";
    }

    void searchStationsByUnusedPercentage() {
        if (stations.empty()) {
            cout << "No stations available to search!\n";
            return;
        }

        cout << "Search stations by percentage of unused workshops (0-100%)\n";
        double minPercentage = getValidatedDouble("Enter minimum percentage: ", 0.0, 100.0);
        double maxPercentage = getValidatedDouble("Enter maximum percentage: ", minPercentage, 100.0);
        
        vector<int> foundIds;
        
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
            cout << "No stations found with unused workshops percentage between " 
                 << minPercentage << "% and " << maxPercentage << "%\n";
            return;
        }
        
        cout << "\n=== FOUND STATIONS ===\n";
        for (int id : foundIds) {
            auto it = stations.find(id);
            if (it != stations.end()) {
                const CompressorStation& station = it->second;
                double unusedPercentage = (1.0 - (double)station.activeWorkshops / station.totalWorkshops) * 100.0;
                
                cout << "ID: " << station.id
                    << " | Name: " << station.name
                    << " | Workshops: " << station.activeWorkshops << "/" << station.totalWorkshops
                    << " | Unused: " << unusedPercentage << "%"
                    << " | Class: " << station.stationClass << "\n";
            }
        }
        cout << "Total found: " << foundIds.size() << " station(s)\n";
    }

    void searchStationsMenu() {
        if (stations.empty()) {
            cout << "No stations available to search!\n";
            return;
        }

        cout << "\n=== STATION SEARCH ===\n";
        cout << "1. Search by name\n";
        cout << "2. Search by percentage of unused workshops\n";
        cout << "0. Back to main menu\n";
        
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

    void addPipe() {
        Pipe newPipe;
        newPipe.id = generateUniqueId(usedPipeIds, nextPipeId);
        
        cout << "Enter pipe data:\n";
        cin >> newPipe;
        
        pipes[newPipe.id] = newPipe;
        cout << "Pipe added successfully! (ID: " << newPipe.id << ")\n";
    }

    void addStation() {
        CompressorStation newStation;
        newStation.id = generateUniqueId(usedStationIds, nextStationId);
        
        cout << "Enter station data:\n";
        cin >> newStation;
        
        stations[newStation.id] = newStation;
        cout << "Station added successfully! (ID: " << newStation.id << ")\n";
    }

    void editPipeStatus() {
        if (pipes.empty()) {
            cout << "No pipes available to edit!\n";
            return;
        }
displayAllPipes();
        int pipeId = getValidatedNumber<int>("\nEnter pipe ID to edit: ");
        
        auto it = pipes.find(pipeId);
        if (it == pipes.end()) {
            cout << "Pipe with ID " << pipeId << " not found!\n";
            return;
        }

        Pipe& pipe = it->second;
        cout << "Current repair status: " << (pipe.underRepair ? "Under repair" : "Operational") << endl;
        
        if (getConfirmation("Change repair status?")) {
            pipe.underRepair = !pipe.underRepair;
            cout << "Status changed successfully!\n";
        }
    }

    void editStationWorkshops() {
        if (stations.empty()) {
            cout << "No stations available to edit!\n";
            return;
        }

        displayAllStations();
        int stationId = getValidatedNumber<int>("\nEnter station ID to edit: ");
        
        auto it = stations.find(stationId);
        if (it == stations.end()) {
            cout << "Station with ID " << stationId << " not found!\n";
            return;
        }

        CompressorStation& station = it->second;
        cout << "Current workshops: " << station.activeWorkshops << "/" << station.totalWorkshops << " active\n";
        cout << "1. Start workshop\n2. Stop workshop\nChoose action: ";

        int action = getValidatedNumber("", 1, 2);
        unsigned int changeAmount = getValidatedNumber<unsigned int>("Enter number of workshops: ", 1);

        if (action == 1) {
            if (station.activeWorkshops + changeAmount <= station.totalWorkshops) {
                station.activeWorkshops += changeAmount;
                cout << changeAmount << " workshop(s) started\n";
            }
            else {
                cout << "Cannot start more than " << station.totalWorkshops - station.activeWorkshops << " workshops\n";
            }
        }
        else {
            if (changeAmount <= station.activeWorkshops) {
                station.activeWorkshops -= changeAmount;
                cout << changeAmount << " workshop(s) stopped\n";
            }
            else {
                cout << "Cannot stop more than " << station.activeWorkshops << " workshops\n";
            }
        }
    }

    void deletePipe() {
        if (pipes.empty()) {
            cout << "No pipes available to delete!\n";
            return;
        }

        displayAllPipes();
        int pipeId = getValidatedNumber<int>("\nEnter pipe ID to delete: ");
        
        auto it = pipes.find(pipeId);
        if (it == pipes.end()) {
            cout << "Pipe with ID " << pipeId << " not found!\n";
            return;
        }

        cout << "You are about to delete pipe: " << it->second.name << " (ID: " << pipeId << ")\n";
        if (getConfirmation("Are you sure?")) {
            pipes.erase(it);
            releaseId(usedPipeIds, pipeId);
            cout << "Pipe deleted successfully!\n";
        }
    }

    void deleteStation() {
        if (stations.empty()) {
            cout << "No stations available to delete!\n";
            return;
        }

        displayAllStations();
        int stationId = getValidatedNumber<int>("\nEnter station ID to delete: ");
        
        auto it = stations.find(stationId);
        if (it == stations.end()) {
            cout << "Station with ID " << stationId << " not found!\n";
            return;
        }

        cout << "You are about to delete station: " << it->second.name << " (ID: " << stationId << ")\n";
        if (getConfirmation("Are you sure?")) {
            stations.erase(it);
            releaseId(usedStationIds, stationId);
            cout << "Station deleted successfully!\n";
        }
    }

    void saveData() {
        string filename;
        cout << "Enter filename to save (without extension): ";
        getline(cin, filename);
        filename += ".txt";
ifstream testFile(filename);
        if (testFile.good()) {
            testFile.close();
            if (!getConfirmation("File already exists. Overwrite?")) {
                cout << "Save cancelled.\n";
                return;
            }
        }

        ofstream outFile(filename);
        if (!outFile) {
            cout << "Error: Could not create file " << filename << endl;
            return;
        }

        outFile << "[NEXT_PIPE_ID]" << endl << nextPipeId << endl;
        outFile << "[NEXT_STATION_ID]" << endl << nextStationId << endl;
        
        outFile << "[USED_PIPE_IDS]" << endl;
        for (int id : usedPipeIds) {
            outFile << id << " ";
        }
        outFile << endl;
        
        outFile << "[USED_STATION_IDS]" << endl;
        for (int id : usedStationIds) {
            outFile << id << " ";
        }
        outFile << endl;

        for (const auto& pair : pipes) {
            outFile << PIPE_IDENTIFIER << endl;
            outFile << pair.second;
        }

        for (const auto& pair : stations) {
            outFile << STATION_IDENTIFIER << endl;
            outFile << pair.second;
        }

        outFile.close();
        cout << "Data successfully saved to " << filename << endl;
        cout << "Saved: " << pipes.size() << " pipes, " << stations.size() << " stations\n";
    }

    void loadData() {
        string filename;
        cout << "Enter filename to load (without extension): ";
        getline(cin, filename);
        filename += ".txt";

        ifstream inFile(filename);
        if (!inFile) {
            cout << "Error: Could not open file " << filename << endl;
            return;
        }

        if (!pipes.empty() || !stations.empty()) {
            if (!getConfirmation("Current data will be overwritten. Continue?")) {
                cout << "Load cancelled.\n";
                inFile.close();
                return;
            }
        }

        pipes.clear();
        stations.clear();
        usedPipeIds.clear();
        usedStationIds.clear();
        
        string line;
        bool readingUsedPipeIds = false;
        bool readingUsedStationIds = false;

        while (getline(inFile, line)) {
            if (line == "[NEXT_PIPE_ID]") {
                string idStr;
                getline(inFile, idStr);
                nextPipeId = stoi(idStr);
            }
            else if (line == "[NEXT_STATION_ID]") {
                string idStr;
                getline(inFile, idStr);
                nextStationId = stoi(idStr);
            }
            else if (line == "[USED_PIPE_IDS]") {
                string idsStr;
                getline(inFile, idsStr);
                stringstream ss(idsStr);
                int id;
                while (ss >> id) {
                    usedPipeIds.insert(id);
                }
            }
            else if (line == "[USED_STATION_IDS]") {
                string idsStr;
                getline(inFile, idsStr);
                stringstream ss(idsStr);
                int id;
                while (ss >> id) {
                    usedStationIds.insert(id);
                }
            }
            else if (line == PIPE_IDENTIFIER) {
                Pipe pipe;
                inFile >> pipe;
                pipes[pipe.id] = pipe;
            }
            else if (line == STATION_IDENTIFIER) {
                CompressorStation station;
                inFile >> station;
                stations[station.id] = station;
            }
        }

        inFile.close();
        cout << "Data successfully loaded from " << filename << endl;
        cout << "Loaded: " << pipes.size() << " pipes, " << stations.size() << " stations\n";
        cout << "Next available IDs - Pipe: " << nextPipeId << ", Station: " << nextStationId << endl;
    }

    void viewAllObjects() {
        cout << "\n=== CURRENT STATE ===\n";
        displayAllPipes();
        displayAllStations();
    }

    void run() {
        int choice = -1;
while (true) {
            cout << "\nMain Menu:\n"
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
                << "13. Save Data\n"
                << "14. Load Data\n"
                << "0. Exit\n"
                << "Choose action: ";

            string input;
            getline(cin, input);
            stringstream ss(input);

            if (!(ss >> choice)) {
                cout << "Invalid input! Please enter a number.\n";
                continue;
            }

            char remaining;
            if (ss >> remaining) {
                cout << "Invalid input! Please enter only a number without extra characters.\n";
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
                saveData();
                break;

            case 14:
                loadData();
                break;

            case 0:
                cout << "Exiting program...\n";
                return;

            default:
                cout << "Invalid choice! Try again.\n";
            }
        }
    }
};

ostream& operator<<(ostream& out, const Pipe& pipe) {
    out << "ID: " << pipe.id
        << " | Name: " << pipe.name
        << " | Length: " << pipe.length << " km"
        << " | Diameter: " << pipe.diameter << " mm"
        << " | Under repair: " << (pipe.underRepair ? "Yes" : "No") << "\n";
    return out;
}

istream& operator>>(istream& in, Pipe& pipe) {
    cout << "Enter pipe name: ";
    in.ignore();
    getline(in, pipe.name);
    cout << "Enter length (km, must be positive): ";
    in >> pipe.length;
    cout << "Enter diameter (mm, must be positive): ";
    in >> pipe.diameter;
    pipe.underRepair = false;
    return in;
}

ostream& operator<<(ostream& out, const CompressorStation& station) {
    out << "ID: " << station.id
        << " | Name: " << station.name
        << " | Workshops: " << station.activeWorkshops << "/" << station.totalWorkshops
        << " | Class: " << station.stationClass << "\n";
    return out;
}

istream& operator>>(istream& in, CompressorStation& station) {
    cout << "Enter station name: ";
    in.ignore();
    getline(in, station.name);
    cout << "Enter total workshops: ";
    in >> station.totalWorkshops;
    cout << "Enter active workshops: ";
    in >> station.activeWorkshops;
    cout << "Enter station class: ";
    in >> station.stationClass;
    return in;
}

int main() {
    DataManager manager;
    manager.run();
    return 0;
}
