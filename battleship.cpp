#include <iostream>
#include <vector>
#include "map.h"
#include "ships.h"
#include "user.h"
using namespace std;

int main(){
    User player = User::createUser();
    Cell grid[gridSize][gridSize];
    vector<Ship> fleet{
        {"Carrier", 5},
        {"Battleship", 4},
        {"Cruiser", 3},
        {"Submarine", 3},
        {"Destroyer", 2}
    };
    placeShip(fleet, grid);

    return 0;
}
