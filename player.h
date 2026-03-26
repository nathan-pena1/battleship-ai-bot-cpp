#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <vector>
#include <random>
#include "map.h" 
#include "ships.h"

class Player {

    protected:
        std::string name;
        std::vector<Ship> fleet;
        Cell grid[gridSize][gridSize];
    public:
        Player(std::string name);
        static Player createPlayer(std::string name);
        virtual ~Player() {}
        std::string getName();
//  private:
//  std::string password;

};

class User : public Player{
    
};

class Bot : public Player{

    public:
        Bot();
        void createGrid();
        Cell (&getGrid())[gridSize][gridSize];
};

#endif