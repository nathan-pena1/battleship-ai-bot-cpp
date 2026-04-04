#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <limits>
#include <string>

template <typename T>
T validateInput(const std::string &prompt)
{
    T input;
    while (true)
    {
        std::cout << prompt;
        std::cin >> input;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please try again.\n";
        }
        else
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return input;
        }
    }
}

#endif