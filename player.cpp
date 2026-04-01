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
                placeShip(fleet[i], grid, row, col, direction);
                placed = true;
            }
        }
    }
}

int Bot::genCoordinate(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> range(0,9);
    return range(gen);
}

// Returns a reference to a 2D array of Cells of size [gridSize]x[gridSize]
Cell (&Player::getGrid())[gridSize][gridSize] {
    return grid;
}

std::vector<Ship>& Player::getFleet(){
    return fleet;
}

void Player::setFleet(const std::vector<Ship>& ships){
    fleet = ships;
    numShips = fleet.size();
}

std::vector<Ship> Player::defaultFleet(){
    return {
        {"Carrier", 5},
        {"Battleship", 4},
        {"Cruiser", 3},
        {"Submarine", 3},
        {"Destroyer", 2}
    };
}

void Bot::fillTargets(Cell (&gameGrid)[gridSize][gridSize], int row, int col){
    int modifyRow[] = {0, 1, -1, 0};
    int modifyCol[] = {-1, 0, 0, 1};
    for(int i = 0; i < 4; i++){
        int modRow = row + modifyRow[i];
        int modCol = col + modifyCol[i];
        if(validAttack(gameGrid, modRow, modCol)){
            targetQueue.push({modRow, modCol});
        }
    }
}

bool Bot::takeTurn(Cell (&gameGrid)[gridSize][gridSize], Player& user){
    int row;
    int col;
    if(mode == Mode::destroy){
        while(!targetQueue.empty() && !validAttack(gameGrid, targetQueue.front().first, targetQueue.front().second)){
            targetQueue.pop();
        }
        if(targetQueue.empty()){
            mode = Mode::search;
        }
    }

    if(mode == Mode::search){
        do{
            row = genCoordinate();
            col = genCoordinate();
        }   while(!validAttack(gameGrid, row, col));
    }
    else{
        row = targetQueue.front().first;
        col = targetQueue.front().second;
        targetQueue.pop();
    }
    bool hit = attackCoordinate(gameGrid, user, row, col);
        if(hit){
            fillTargets(gameGrid, row, col);
            mode = Mode::destroy;
            return true;
        }
    return false;
}

int Player::getNumShips() const{
    return numShips;
}
void Player::removeShip(){
    numShips--;
}
