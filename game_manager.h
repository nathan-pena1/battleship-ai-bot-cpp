#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include <random>
#include "player.h"

class GameManager{
    bool gameOver = false;
    public:
        bool isOver();
        void endGame();
        int flipCoin();
        bool winner(const Player& user, const Player& bot);

};









#endif