#include "user.h"

User::User(std::string name) : name(name){}

User::createUser(){
    std::string name;
    std::cout << "Please enter a username: ";
    std::cin >> name;
    return User(name);
}