#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H
#include <random>

class GameManager{
    bool gameOver = false;
    public:
        bool isOver();
        void endGame();
        int flipCoin();
};









#endif