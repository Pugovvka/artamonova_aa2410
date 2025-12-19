#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <set>

const std::string PIPE_IDENTIFIER = "[PIPE]";
const std::string STATION_IDENTIFIER = "[STATION]";
const std::string CONNECTION_IDENTIFIER = "[CONNECTION]";
const std::set<int> ALLOWED_DIAMETERS = { 500, 700, 1000, 1400 };

#endif // CONSTANTS_H