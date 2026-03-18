#ifndef SHIPS_H
#define SHIPS_H
#include <string>

class Ship{

    public:
    std::string name;
    int health;
    bool beenPlaced = false;
    Ship(std::string name, int health);

};

#endif