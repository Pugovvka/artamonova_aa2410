#ifndef PIPE_H
#define PIPE_H

#include <iostream>
#include <string>

class Pipe {
public:
    int id = 0;
    std::string name = "";
    int length = 0;
    int diameter = 0;
    bool underRepair = false;
    bool inUse = false; // Флаг, используется ли труба в сети

    friend std::ostream& operator<<(std::ostream& out, const Pipe& pipe);
    friend std::istream& operator>>(std::istream& in, Pipe& pipe);
};

#endif // PIPE_H