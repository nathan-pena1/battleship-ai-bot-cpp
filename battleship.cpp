#include <iostream>
#include <vector>
#include "map.h"
#include "ships.h"
#include "player.h"
#include "game_manager.h"
#include "combat.h"
using namespace std;

int main(){

    string name;
    cout << "Please enter a username: ";
    cin >> name;
    cout << endl;
    Player user = Player::createPlayer(name);
    user.setFleet(Player::defaultFleet());
    Cell (&grid)[gridSize][gridSize] = user.getGrid();
    vector<Ship>& fleet = user.getFleet();

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
                Ship& current =  fleet[i]; 
                int row, col;
                string direction;
                displaySelection(grid);
                cout << "Select starting row for " << current.getName() << " (0-9): ";
                cin >> row;
                cout << endl;
                displaySelection(grid);
                cout << "Select starting column for " << current.getName() << " (0-9): ";
                cin >> col;
                cout << endl;
                displaySelection(grid);
                cout << "Horizontal or vertical? (H/V): ";
                cin >> direction;


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
        enemyBot.setFleet(Bot::defaultFleet());
        enemyBot.createGrid();
        Cell (&gameGrid)[gridSize][gridSize] = enemyBot.getGrid();
        vector<Ship>& enemyFleet = enemyBot.getFleet();
        GameManager game;
        int row; 
        int col;
        while(!(game.isOver())){
            displayMap(gameGrid);
            do{
                cout << "Select row to strike (0-9): ";
                cin >> row;
                cout << "Select col to strike (0-9): "; 
                cin >> col;
            }    while(!user.validAttack(gameGrid, row, col));
            if(user.attackCoordinate(gameGrid, enemyBot, row, col)){
                cout << "Enemy ship hit!\n";
            }
            if(enemyBot.takeTurn(grid, user)){
                cout << "Your ship has been hit!\n";
            }
            if(game.winner(user, enemyBot)){
                if(user.getNumShips() > enemyBot.getNumShips()){
                cout << user.getName() << " has won the game!\n";
                }
                else{
                    cout << enemyBot.getName() << " has won the game!\n";
                }
                game.endGame();
            }
        }
        
        
    return 0;
}
