#include "map.h"
#include <iostream>
#include <vector>

bool Cell::containsShip(){
    return hasShip;
}

bool Cell::beenAttacked(){
    return hasAttack;
}

void Cell::placeShip(){
    hasShip = true;
}
void Cell::attackCell(){
    hasAttack = true;
}

bool validPlacement(Ship current, Cell grid[gridSize][gridSize], int row, int col, std::string direction){  
    for (int i = 0; i < current.getHealth(); i++){
        int r = row;
        int c = col;
        if (direction == "h" || direction == "H"){
            c = col + i;
        }
        else if (direction == "v" || direction == "V"){
            r = row + i;
        }
        else{
            return false;
        }
        if (r < 0 || r >= gridSize || c < 0 || c >= gridSize){
            return false; 
        }
        if(grid[r][c].containsShip()){
            return false;
        }
    }
    return true;
}

void setShip(Ship current, Cell (&grid)[gridSize][gridSize], int row, int col, std::string direction){
    for (int i = 0; i < current.getHealth(); i++){
        if (direction == "h" || direction == "H"){
            grid[row][col + i].placeShip();
        }
        else if (direction == "v" || direction == "V"){
            grid[row + i][col].placeShip();
        }
    }
}
