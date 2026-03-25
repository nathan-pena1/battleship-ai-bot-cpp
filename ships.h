#ifndef SHIPS_H
#define SHIPS_H
#include <string>

class Ship{

    std::string name;
    int health;

// below may become obsolete 
    bool placed = false;

    public:
        Ship(std::string name, int health);
        std::string getName();
        int getHealth();
        bool beenPlaced();
        void place();

};

#endif
