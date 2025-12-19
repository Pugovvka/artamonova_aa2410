#include "station.h"
#include <string>

std::ostream& operator<<(std::ostream& out, const CompressorStation& station) {
    out << station.id << "\n"
        << station.name << "\n"
        << station.totalWorkshops << "\n"
        << station.activeWorkshops << "\n"
        << station.stationClass << "\n";
    return out;
}

std::istream& operator>>(std::istream& in, CompressorStation& station) {
    std::string line;

    // Читаем ID
    getline(in, line);
    station.id = std::stoi(line);

    // Читаем имя
    getline(in, station.name);

    // Читаем общее количество цехов
    getline(in, line);
    station.totalWorkshops = std::stoi(line);

    // Читаем активные цеха
    getline(in, line);
    station.activeWorkshops = std::stoi(line);

    // Читаем класс станции
    getline(in, line);
    station.stationClass = std::stoi(line);

    return in;
}