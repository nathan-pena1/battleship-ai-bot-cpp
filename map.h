#ifndef MAP_H
#define MAP_H
#include "ships.h"
#include <vector>

const int gridSize = 10;

class Cell{

    bool hasShip = false;
    bool hasAttack = false;
    Ship* ship = nullptr;

    public:
        bool containsShip() const;
        bool beenAttacked() const;
        void placeShip();
        void attackCell();
        Ship* getShip() const;
        void setShip(Ship* ship);

};

bool validPlacement(Ship current, Cell grid[gridSize][gridSize], int row, int col, char direction);
void placeShip(Ship& current, Cell (&grid)[gridSize][gridSize], int row, int col, char direction);
void displaySelection(Cell grid[gridSize][gridSize]);
void displayMap(Cell (&grid)[gridSize][gridSize]);



#endif