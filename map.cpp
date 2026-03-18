#include "map.h"
#include <iostream>
#include <vector>

void placeShip(vector<Ship>& fleet, Cell& grid[gridSize][gridSize]){

        int unplacedCount = fleet.size();
        while(unplacedCount != 0){
            int selectedShip;
            bool found = false;
            for(int i = 0; i < fleet.size(); i++){
                Ship current = fleet[i];
                if(!(current.beenPlaced)){
                std::cout << "Select " << i << " to place " << current.name << std::endl;
                }
            }    
            std::cin >> selectedShip;
            int index = -1;
            for(int i = 0; i < fleet.size(); i++){
                if(selectedShip == i && !fleet[i].beenPlaced){
                    found = true;
                    index = i;
                    Ship current =  fleet[i]; 
                    for(int j = 0; j < current.health; j++){
                        bool cellPlaced = false;
                        while(!cellPlaced){
                            int row, col;
                            int curr = j + 1;
                            std::cout << "Select row " << curr << " for " << current.name << "(0-9): ";
                            std::cin >> row;
                            std::cout << "Select column " << curr << " for " << current.name << "(0-9): ";
                            std::cin >> col;

                            if(row < 0 || row >= gridSize || col < 0 || col >= gridSize){
                                std::cout << "Out of bounds! Please make a selection 0-9" << std::endl;
                            }
                            else if(grid[row][col].hasShip == false){   
                                grid[row][col].hasShip = true;
                                cellPlaced = true;
                            }
                            else{
                                std::cout << "Coordinate occupied. Try again." << std::endl;
                            }
                        }
                    }
                break;
                }
            }
                if(!found){
                    std::cout << "Please make a valid selection." << std::endl;
                }
                else{
                    fleet[index].beenPlaced = true;
                    unplacedCount--;
                }
            
        }
}