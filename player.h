#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <vector>
#include <random>
#include "map.h" 
#include "ships.h"
#include "combat.h"

class Player : public Combat{

    protected:
        std::string name;
        std::vector<Ship> fleet;
        Cell grid[gridSize][gridSize];
    public:
        Player(std::string name);
        static Player createPlayer(std::string name);
        virtual ~Player() {}
        std::string getName();
        Cell (&getGrid())[gridSize][gridSize];
        std::vector<Ship>& getFleet();
        void setFleet(const std::vector<Ship>& ships);
        static std::vector<Ship> defaultFleet();

//  private:
//  std::string password;

};

class User : public Player{
    
};

class Bot : public Player{

    public:
        Bot();
        void createGrid();
        int genCoordinate();
};

#endif