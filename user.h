#ifndef USER_H
#define USER_H
#include <string>

class User {

    public:
    int numShips = 5;
    int wins = 0;
    int losses = 0;
    std::string name;
    User(std::string name);
    static User createUser(std::string name);
//  private:
//  std::string password;

};

#endif