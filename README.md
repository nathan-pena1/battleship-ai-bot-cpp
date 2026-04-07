# C++ Battleship — Game Engine with FSM Opponent AI Bot

I built this game to deepen my understanding of OOP, practice complex system design in C++, manage game state without any memory leaks, and ultimately just have fun.
<img width="1710" height="1073" alt="BattleshipThumbnail" src="https://github.com/user-attachments/assets/f8fb3551-cd8f-4f95-bf80-6b23aa1d1ce5" />

---

## Transparency note for recruiters and engineers

This project has two clearly separated layers.

### Backend - written by me, unassisted

Everything in the game engine is code I wrote by hand:

- The 10×10 `Cell`/grid model and all placement rules
- Combat resolution and attack validation
- Win/loss state management (`GameManager`)
- The opponent **game-playing AI bot**: a **Finite State Machine** I designed myself (see Highlights below)
- All supporting classes: `Ship`, `Player`, `User`, `Bot`, `Combat`

When this README says "AI" in a technical context (e.g. *"AI Bot"*, *"FSM AI"*), it always refers to **the in-game opponent bot** - an algorithm I wrote in C++.

### Frontend - built with an AI coding assistant

The graphical client (`battleship_ui.cpp` / `battleship_ui.h`) was implemented using **Cursor with an LLM** as a coding assistant. I reviewed and integrated all of it; it calls the engine APIs I designed without touching any game logic. A raylib UI in raw C++ is mostly tedious layout work. I chose to use an assistant for that layer so I could spend my time on the engine, the bot, and other projects. The file split makes it easy to see exactly what was assisted: everything in `battleship_ui.cpp` is presentation only.

---

## Highlights

### 1. Opponent AI Bot - FSM + Priority Queue (written by me)

> **Important:** this "AI" is the *game opponent*, not the coding assistant. Two completely different things.

I implemented a **Finite State Machine** bot with two modes:

- **Search mode:** the bot picks random coordinates via RNG until it finds a ship.
- **Destroy mode:** once a hit is registered, the bot pushes all four adjacent cells into a `std::queue` and drains that queue before returning to search. This is a classic *hunt/target* strategy.

I purposely implemented a semi efficient attack approach for the AI. I avoided implementing the mathematically most efficient approach (the checkerboard parity pattern) because it makes the game less fun. I may in the future implement an **EXTREME** difficulty mode for presentation purposes, as the algorithm differs in only a couple lines, but I am unsure how adding a mode with an unbeatable bot would add to the user experience.

### 2. Crash-proof input (`util.h` template)

A header-only template that wraps `std::cin`. If a user types a letter where a number is expected, `std::cin` normally sets an error flag and the program loops or crashes. This template clears the flag, flushes the buffer, and forces a retry — making the terminal-driven build virtually impossible to crash via keyboard.

### 3. O(1) combat lookups

Every `Cell` holds a raw observer pointer (`Ship`*) to the ship sitting on it. When a coordinate is attacked, the engine follows the pointer directly to call `registerHit()`. This requires no linear search through a fleet list. Sunk-ship tracking (`removeShip()`) is O(1) for the same reason.

---

## Project structure


| File                       | Author      | Purpose                                          |
| -------------------------- | ----------- | ------------------------------------------------ |
| `battleship.cpp`           | Me          | `main` — calls `run_raylib_battleship_ui()`      |
| `battleship_ui.cpp` / `.h` | AI-assisted | All raylib drawing, input, and game-flow wiring  |
| `player.cpp` / `.h`        | Me          | `Player`, `User`, `Bot` — includes FSM bot logic |
| `map.cpp` / `.h`           | Me          | 10×10 grid, `Cell` state, placement helpers      |
| `ships.cpp` / `.h`         | Me          | `Ship` — health, name, sunk state                |
| `game_manager.cpp` / `.h`  | Me          | Win condition, game-over state                   |
| `combat.cpp` / `.h`        | Me          | Attack execution and coordinate validation       |
| `util.h`                   | Me          | Crash-proof `std::cin` input template            |


---

## Dependencies

- **raylib** — graphical client only (`battleship_ui.cpp`)

### macOS (Homebrew)

```bash
brew install raylib
```

### Linux

Install from your distro packages or build from source via the [raylib wiki](https://github.com/raysan5/raylib/wiki).

## How to Build & Run

**macOS** (Apple Silicon; swap `/opt/homebrew` → `/usr/local` for Intel):

```bash
g++ -std=c++17 -O2 \
  -I/opt/homebrew/include -L/opt/homebrew/lib \
  battleship.cpp battleship_ui.cpp combat.cpp game_manager.cpp map.cpp player.cpp ships.cpp \
  -o battleship -lraylib \
  -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
./battleship
```

**Linux** (with `pkg-config`):

```bash
g++ -std=c++17 -O2 *.cpp -o battleship $(pkg-config --libs --cflags raylib)
./battleship
```

**Windows:** Install raylib via [MSYS2](https://www.msys2.org/) or [vcpkg](https://vcpkg.io/), then link `-lraylib` with the required system libs for your toolchain.

### Controls


| Input                   | Action                  |
| ----------------------- | ----------------------- |
| Type + **Enter**        | Confirm callsign        |
| Click ship → click grid | Place ship              |
| **R**                   | Rotate ship orientation |
| Click enemy grid        | Fire                    |
| **Enter** / button      | Play again              |
| **Esc**                 | Quit                    |


---

## Terminal-only build

The engine has no raylib dependency. Remove or replace `battleship_ui.cpp` and wire the game loop to a terminal UI using the same public types (`Player`, `User`, `Bot`, `GameManager`, etc.) and `util.h` for safe input.
