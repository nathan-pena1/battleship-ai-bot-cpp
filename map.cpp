#include "map.h"
#include <iostream>

bool Cell::containsShip() const
{
    return hasShip;
}

bool Cell::beenAttacked() const
{
    return hasAttack;
}

void Cell::placeShip()
{
    hasShip = true;
}
void Cell::attackCell()
{
    hasAttack = true;
}

bool validPlacement(const Ship &current, Cell grid[gridSize][gridSize], int row, int col, char direction)
{
    if (direction == 'h' || direction == 'H')
    {
        bool allowRight = true;
        for (int i = 0; i < current.getHealth(); i++)
        {
            int c = col + i;
            if (row < 0 || row >= gridSize || c < 0 || c >= gridSize || grid[row][c].containsShip())
            {
                allowRight = false;
                break;
            }
        }
        if (allowRight)
        {
            return true;
        }
        for (int i = 0; i < current.getHealth(); i++)
        {
            int c = col - i;
            if (row < 0 || row >= gridSize || c < 0 || c >= gridSize || grid[row][c].containsShip())
            {
                return false;
            }
        }
        return true;
    }

    if (direction == 'v' || direction == 'V')
    {
        bool allowDown = true;
        for (int i = 0; i < current.getHealth(); i++)
        {
            int r = row + i;
            if (r < 0 || r >= gridSize || col < 0 || col >= gridSize || grid[r][col].containsShip())
            {
                allowDown = false;
                break;
            }
        }
        if (allowDown)
        {
            return true;
        }
        for (int i = 0; i < current.getHealth(); i++)
        {
            int r = row - i;
            if (r < 0 || r >= gridSize || col < 0 || col >= gridSize || grid[r][col].containsShip())
            {
                return false;
            }
        }
        return true;
    }

    return false;
}

void placeShip(Ship &current, Cell (&grid)[gridSize][gridSize], int row, int col, char direction)
{
    if (direction == 'h' || direction == 'H')
    {
        bool placeRight = true;
        for (int i = 0; i < current.getHealth(); i++)
        {
            int c = col + i;
            if (row < 0 || row >= gridSize || c < 0 || c >= gridSize || grid[row][c].containsShip())
            {
                placeRight = false;
                break;
            }
        }

        if (placeRight)
        {
            for (int i = 0; i < current.getHealth(); i++)
            {
                grid[row][col + i].placeShip();
                grid[row][col + i].setShip(&current);
            }
        }
        else
        {
            for (int i = 0; i < current.getHealth(); i++)
            {
                grid[row][col - i].placeShip();
                grid[row][col - i].setShip(&current);
            }
        }
        return;
    }

    if (direction == 'v' || direction == 'V')
    {
        bool placeDown = true;
        for (int i = 0; i < current.getHealth(); i++)
        {
            int r = row + i;
            if (r < 0 || r >= gridSize || col < 0 || col >= gridSize || grid[r][col].containsShip())
            {
                placeDown = false;
                break;
            }
        }

        if (placeDown)
        {
            for (int i = 0; i < current.getHealth(); i++)
            {
                grid[row + i][col].placeShip();
                grid[row + i][col].setShip(&current);
            }
        }
        else
        {
            for (int i = 0; i < current.getHealth(); i++)
            {
                grid[row - i][col].placeShip();
                grid[row - i][col].setShip(&current);
            }
        }
    }
}

void displaySelection(Cell grid[gridSize][gridSize])
{
    std::cout << "   ";
    for (int i = 0; i < gridSize; i++)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "   ";
    for (int i = 0; i < gridSize; i++)
    {
        std::cout << "_ ";
    }
    std::cout << std::endl;
    for (int i = 0; i < gridSize; i++)
    {
        std::cout << i << " |";
        for (int j = 0; j < gridSize; j++)
        {
            if (grid[i][j].containsShip())
            {
                if (grid[i][j].beenAttacked())
                {
                    std::cout << "x ";
                }
                else
                {
                    std::cout << "* ";
                }
            }
            else
            {
                std::cout << "~ ";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "   ";
    for (int i = 0; i < gridSize; i++)
    {
        std::cout << "- ";
    }
    std::cout << std::endl;
}

void displayMap(Cell (&grid)[gridSize][gridSize])
{
    std::cout << "   ";
    for (int i = 0; i < gridSize; i++)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "   ";
    for (int i = 0; i < gridSize; i++)
    {
        std::cout << "_ ";
    }
    std::cout << std::endl;
    for (int i = 0; i < gridSize; i++)
    {
        std::cout << i << " |";
        for (int j = 0; j < gridSize; j++)
        {
            if (grid[i][j].beenAttacked())
            {
                if (grid[i][j].containsShip())
                {
                    std::cout << "X ";
                }
                else
                {
                    std::cout << "O ";
                }
            }
            else
            {
                std::cout << "~ ";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "   ";
    for (int i = 0; i < gridSize; i++)
    {
        std::cout << "- ";
    }
    std::cout << std::endl;
}

Ship *Cell::getShip() const
{
    return ship;
}

void Cell::setShip(Ship *ship)
{
    this->ship = ship;
}
