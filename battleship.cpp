#include <iostream>
#include <vector>
#include "map.h"
#include "ships.h"
#include "player.h"
#include "game_manager.h"
using namespace std;

int main(){

    string name;
    cout << "Please enter a username: ";
    cin >> name;
    cout << endl;
    Player user = Player::createPlayer(name);
    Cell grid[gridSize][gridSize];
    vector<Ship> fleet{
        {"Carrier", 5},
        {"Battleship", 4},
        {"Cruiser", 3},
        {"Submarine", 3},
        {"Destroyer", 2}
    };

    int unplacedCount = fleet.size();
    while(unplacedCount != 0){
            int selectedShip;
            bool found = false;
            displaySelection(grid);
            for(int i = 0; i < fleet.size(); i++){
                Ship current = fleet[i];
                if(!(current.beenPlaced())){
                std::cout << "Enter: " << i << " to place " << current.getName() << std::endl;
                }
            }    
            cin >> selectedShip;
//          DELETE ME: Temp Special Top Secret Dev Debugging Tool
            if(selectedShip == -1){
                break;
            }
            cout << endl;
            int index;
        
            for(int i = 0; i < fleet.size(); i++){
                if(selectedShip != i || fleet[i].beenPlaced()){
                    continue;
                }
        
                found = true;
                index = i;
                Ship current =  fleet[i]; 
                int row, col;
                string direction;
                std::cout << "Select starting row for " << current.getName() << " (0-9): ";
                std::cin >> row;
                std::cout << "Select starting column for " << current.getName() << " (0-9): ";
                std::cin >> col;
                std::cout << "Horizontal or vertical? (H/V): ";
                std::cin >> direction;

                if(!validPlacement(current, grid, row, col, direction)){
                    std::cout << "Please make a valid selection.\n";
                    i--;
                    continue;
                }   
                fleet[i].place();
                placeShip(current, grid, row, col, direction);
                unplacedCount--;
                cout << endl;
                break;
                }
            if (!found){
                std::cout << "Please make a valid selection.\n";
            }
        }
        cout << "Ships placed successfully!\n\nBeginning game!\n";
        for(int i = 3; i > 0; i--){
            cout << i <<"...\n";
        }
        cout << endl;

        Bot enemyBot;
        enemyBot.createGrid();
        Cell (&gameGrid)[gridSize][gridSize] = enemyBot.getGrid();
        vector<Ship>& enemyFleet = enemyBot.getFleet();
        GameManager game;
        while(!(game.isOver())){
        displayMap(gameGrid);
        int row, col;
        cout << "Select row to strike (0-9): ";
        cin >> row;
        cout << "Select col to strike (0-9): "; 
        cin >> col;
        user.attackCoordinate(fleet, gameGrid, row, col);
        }

        
    return 0;
}
