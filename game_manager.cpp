#include "game_manager.h"

void GameManager::endGame(){
    gameOver = true;
}

bool GameManager::isOver(){
    return gameOver;
}