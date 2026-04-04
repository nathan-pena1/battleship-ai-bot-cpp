# C++ Battleship Engine (With AI Bot)

I built this project to tackle more complex system design, get really familiar with OOP, manage game state without memory leaks, create an AI bot, and ultimately just have fun and experiment.

## Highlights

### 1. The "Intelligent" Bot (FSM & Queueing)
Instead of having the bot just guess coordinates at random until it hits something, I implemented a **Finite State Machine (FSM)** with two modes: `search` and `destroy`.  

* **Search Mode:** The bot uses random RNG to find a target.
* **Destroy Mode:** Once a ship is hit, the bot switches states and pushes the adjacent cells into a `std::queue`. It then clears that queue before going back to searching.   

This is *one of* the more efficient strategies when implementing an AI bot. I purposely avoided the most efficient strategy (checkerboard pattern) as I felt the game would not be as fun. I may implement an **EXTREME DIFFICULTY** mode in the future.

### 2. Crash-Proof Input (The `util.h` Template)
To avoid a rouge `std::cin` crashing the whole program if a user types a letter instead of a number. I wrote a header-only template in `util.h` to fix this. It detects when the input stream fails, clears the error, flushes the buffer, and forces a retry. This makes the game virtually impossible to crash via the keyboard.

### 3. Memory & Performance
* **$O(1)$ Combat Lookups:** Every `Cell` on the 10x10 grid holds a raw pointer observer (`Ship*`) to the actual ship object. This means when a coordinate is attacked, the engine doesn't have to search through a list, it just follows the pointer directly to the ship to register damage.

## Project Structure
* `battleship.cpp`: The main game loop and session lifecycle management.
* `player.cpp / .h`: Handles the `User` and `Bot` classes, including the AI's state logic.
* `map.cpp / .h`: Manages the 10x10 grid, `Cell` states, and visual terminal rendering.
* `ships.cpp / .h`: Defines ship attributes like health, naming, and "sunk" status.
* `game_manager.cpp / .h`: Controls the overall game state and win conditions.
* `combat.cpp / .h`: A dedicated class for processing damage and validating attack coordinates.
* `util.h`: Defensive programming template that guarantees a valid *type* input.

## How to Run
**Note:** This engine is designed and tested for Unix-based terminal environments (macOS and Linux). 
* The compilation command below assumes you are using a terminal that supports `g++` or `clang++`. 
* If you are on Windows, you will need to use **WSL (Windows Subsystem for Linux)** or a compiler like **MinGW** to run these commands.
1. Clone the repository 
```bash
   git clone https://github.com/nathan-pena1/battleship-cpp.git
```
2. Compile all `.cpp` files together:

```
g++ *.cpp -o battleship
./battleship
```
  
I may add a CMake file later to make cross platform use easier.