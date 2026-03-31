#include "combat.h"

bool Combat::attackCoordinate(Cell (&gameGrid)[gridSize][gridSize],int row, int col){
    gameGrid[row][col].attackCell();
    if(gameGrid[row][col].containsShip()){
        Ship* hitShip = gameGrid[row][col].getShip();
        hitShip->registerHit();
        return true;
    }
    return false;
}

bool Combat::validAttack(Cell (&gameGrid)[gridSize][gridSize], int row, int col){
    if(row > 9 || row < 0){
        return false;
    }
    else if(col > 9 || col < 0){
        return false;
    }
    if (gameGrid[row][col].beenAttacked()){
        return false;
    }
    return true;
}

