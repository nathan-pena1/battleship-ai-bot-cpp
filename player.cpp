#include "player.h"

Player::Player(std::string name) : name(name){}

Player Player::createPlayer(std::string name){
    return Player(name);
}

std::string Player::getName(){
    return name;
}

Bot::Bot() : Player("E-Tron"){}

void Bot::createGrid(){

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> range(0, 9);
    std::uniform_int_distribution<int> flip(0, 1);

    for(int i = 0; i < fleet.size(); i++){
        bool placed = false;
        while(!placed){
            int row = range(gen);
            int col = range(gen);
            std::string direction = (flip(gen) == 0) ? "h" : "v";
            if(validPlacement(fleet[i], grid, row, col, direction)){
                fleet[i].place();
                setShip(fleet[i], grid, row, col, direction);
                placed = true;
            }
        }
    }
}

// Returns a reference to a 2D array of Cells of size [gridSize]x[gridSize]
Cell (&Bot::getGrid())[gridSize][gridSize] {
    return grid;
}