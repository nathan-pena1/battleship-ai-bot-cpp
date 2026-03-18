#ifndef MAP_H
#define MAP_H
#include "ships.h"
#include <vector>

const int gridSize = 10;

class Cell{

    public:
    bool hasShip = false;
    bool hasAttack = false;

};

void placeShip(vector<Ship>& fleet, Cell& grid[gridSize][gridSize]);


#endif