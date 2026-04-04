#include "ships.h"

Ship::Ship(const std::string& name, int health) : name(name), health(health){}

std::string Ship::getName() const{
    return name;
}

int Ship::getHealth() const{
    return health;
}

bool Ship::beenPlaced() const{
    return placed;
}

void Ship::place(){
    placed = true;
}

void Ship::registerHit(){
    health -= 1;
}

bool Ship::isSunk() const{
    return health == 0;
}