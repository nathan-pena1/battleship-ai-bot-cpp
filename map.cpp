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

// Temporary terminal display for testing and debugging 
void displaySelection(Cell grid[gridSize][gridSize]){
    std::cout << "   ";
    for(int i = 0; i < gridSize; i++){
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "   ";
    for(int i = 0; i < gridSize; i++){
        std::cout << "_ ";
    }
    std::cout << std::endl;
    for(int i = 0; i < gridSize; i++){
        std::cout << i << " |";
        for(int j = 0; j < gridSize; j++){
            if(grid[i][j].containsShip()){
                if(grid[i][j].beenAttacked()){
                    std::cout << "x ";
                }
                else{
                    std::cout << "* ";
                }
            }
            else{
                std::cout << "~ ";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "   ";
    for(int i = 0; i < gridSize; i++){
        std::cout << "- ";
    }
    std::cout << std::endl;
}

void displayMap(Cell (&grid)[gridSize][gridSize]){
    std::cout << "   ";
    for(int i = 0; i < gridSize; i++){
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "   ";
    for(int i = 0; i < gridSize; i++){
        std::cout << "_ ";
    }
    std::cout << std::endl;
    for(int i = 0; i < gridSize; i++){
        std::cout << i << " |";
        for(int j = 0; j < gridSize; j++){
            if(grid[i][j].beenAttacked()){
                if(grid[i][j].containsShip()){
                    std::cout << "X ";
                }
                else{
                    std::cout << "O ";
                }
            }
            else{
                std::cout << "~ ";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "   ";
    for(int i = 0; i < gridSize; i++){
        std::cout << "- ";
    }
    std::cout << std::endl;
}