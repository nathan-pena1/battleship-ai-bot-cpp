#include "ships.h"

Ship::Ship(std::string name, int health) : name(name), health(health){}

std::string Ship::getName(){
    return name;
}

int Ship::getHealth(){
    return health;
}

bool Ship::beenPlaced(){
    if (placed){
        return true;
    }
    return false;
}

void Ship::place(){
    placed = true;
}

void Ship::registerHit(){
    health -= 1;
}

bool Ship::isSunk(){
    if(health == 0){
        return true;
    }
    return false;
}