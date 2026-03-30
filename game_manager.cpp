#include "game_manager.h"

void GameManager::endGame(){
    gameOver = true;
}

bool GameManager::isOver(){
    return gameOver;
}

int GameManager::flipCoin(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> range(0,1);
    return range(gen);
}