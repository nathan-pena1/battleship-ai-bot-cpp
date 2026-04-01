#ifndef COMBAT_H
#define COMBAT_H
#include <vector>
#include "map.h" 
#include "ships.h"

class Player;

class Combat{

    public:
    bool attackCoordinate(Cell (&gameGrid)[gridSize][gridSize], Player& enemy, int row, int col);
    bool validAttack(Cell (&gameGrid)[gridSize][gridSize], int row, int col);
};



#endif