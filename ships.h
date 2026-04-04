#ifndef SHIPS_H
#define SHIPS_H
#include <string>

class Ship
{

    std::string name;
    int health;
    bool placed = false;

public:
    Ship(const std::string &name, int health);
    std::string getName() const;
    int getHealth() const;
    bool beenPlaced() const;
    void place();
    void registerHit();
    bool isSunk() const;
};

#endif
