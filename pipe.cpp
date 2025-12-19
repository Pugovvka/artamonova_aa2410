#include "pipe.h"
#include <string>

std::ostream& operator<<(std::ostream& out, const Pipe& pipe) {
    out << pipe.id << "\n"
        << pipe.name << "\n"
        << pipe.length << "\n"
        << pipe.diameter << "\n"
        << pipe.underRepair << "\n";
    return out;
}

std::istream& operator>>(std::istream& in, Pipe& pipe) {
    std::string line;

    // Читаем ID
    getline(in, line);
    pipe.id = std::stoi(line);

    // Читаем имя
    getline(in, pipe.name);

    // Читаем длину
    getline(in, line);
    pipe.length = std::stoi(line);

    // Читаем диаметр
    getline(in, line);
    pipe.diameter = std::stoi(line);

    // Читаем статус ремонта
    getline(in, line);
    pipe.underRepair = (line == "1" || line == "true");

    return in;
}