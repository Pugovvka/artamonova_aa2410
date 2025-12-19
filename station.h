#ifndef STATION_H
#define STATION_H

#include <iostream>
#include <string>

class CompressorStation {
public:
    int id = 0;
    std::string name = "";
    unsigned int totalWorkshops = 0;
    unsigned int activeWorkshops = 0;
    int stationClass = 0;

    friend std::ostream& operator<<(std::ostream& out, const CompressorStation& station);
    friend std::istream& operator>>(std::istream& in, CompressorStation& station);
};

#endif // STATION_H#pragma once
