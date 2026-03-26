#ifndef PLAYER_H
#define PLAYER_H
#include <string>

class Player {

    public:
    int numShips = 5;
    int wins = 0;
    int losses = 0;
    std::string name;
    Player(std::string name);
    static Player createPlayer(std::string name);
//  private:
//  std::string password;

};

class User : public Player{

};

class Bot : public Player{
    
};

#endif