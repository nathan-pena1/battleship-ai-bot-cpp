#include <iostream>
#include <vector>
#include "map.h"
#include "ships.h"
#include "user.h"
using namespace std;

int main(){

    string name;
    cout << "Please enter a username: ";
    cin >> name;
    cout << endl;
    User player = User::createUser(name);
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
            for(int i = 0; i < fleet.size(); i++){
                Ship current = fleet[i];
                if(!(current.beenPlaced())){
                std::cout << "Select " << i << " to place " << current.getName() << std::endl;
                }
            }    
            cin >> selectedShip;
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
                setShip(current, grid, row, col, direction);
                unplacedCount--;
                cout << endl;
                break;
                }
            if (!found){
                std::cout << "Please make a valid selection.\n";
            }
        }

        
    return 0;
}
