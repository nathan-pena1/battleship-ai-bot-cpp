#include "user.h"

User::User(std::string name) : name(name){}

User User::createUser(std::string name){
    return User(name);
}
