#ifndef MAP_H
#define MAP_H
#include "ships.h"
#include <vector>

const int gridSize = 10;

class Cell{

    bool hasShip = false;
    bool hasAttack = false;

    public:
        bool containsShip();
        bool beenAttacked();
        void placeShip();
        void attackCell();

};

bool validPlacement(Ship current, Cell grid[gridSize][gridSize], int row, int col, std::string direction);
void setShip(Ship current, Cell (&grid)[gridSize][gridSize], int row, int col, std::string direction);

// void placeShip(std::vector<Ship>& fleet, Cell (&grid)[gridSize][gridSize]);



#endif