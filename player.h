#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <vector>
#include <random>
#include <queue>
#include <utility>
#include "map.h" 
#include "ships.h"
#include "combat.h"

class Player : public Combat{

    protected:
        std::string name;
        std::vector<Ship> fleet;
        Cell grid[gridSize][gridSize];
        int numShips = -1;
    public:
        Player(std::string name);
        static Player createPlayer(std::string name);
        virtual ~Player() {}
        std::string getName();
        Cell (&getGrid())[gridSize][gridSize];
        std::vector<Ship>& getFleet();
        void setFleet(const std::vector<Ship>& ships);
        static std::vector<Ship> defaultFleet();
        int getNumShips() const;
        void removeShip();

//  private:
//  std::string password;

};

class User : public Player{
    
};

class Bot : public Player{

    enum class Mode { search, destroy };
    Mode mode = Mode::search;
    std::queue<std::pair<int,int>> targetQueue;

    public:
        Bot();
        void createGrid();
        int genCoordinate();
        void fillTargets(Cell (&gameGrid)[gridSize][gridSize], int row, int col);
        bool takeTurn(Cell (&gameGrid)[gridSize][gridSize], Player& user);
};

#endif