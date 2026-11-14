# C89 Treasure Hunt

A terminal-based grid game written in C89. Guide the player (`P`) through a map, avoid a randomly moving snake (`~`), collect the lantern (`@`) and reach the treasure (`$`). The game includes an optional limited-visibility mode and an undo feature backed by a linked-list stack.

## Features

- **Grid-based movement:** Move with `W`, `A`, `S` and `D` while avoiding obstacles (`O`) and map boundaries.
- **Random enemy movement:** The snake moves to a randomly selected valid neighbouring floor cell after each successful player move.
- **Treasure objective:** Reach `$` to win. Collision with the snake ends the game.
- **Lantern:** Collect `@`. In dark mode, this increases the player's Manhattan-distance visibility range from 3 to 6 cells.
- **Dark mode:** Build with `DARK=1` to hide map cells outside the player's visibility range.
- **Undo:** Press `U` to restore the previous player and snake positions and the lantern/visibility state. Press `Q` or `Ctrl+C` to quit cleanly. The history is stored in a linked-list stack.
- **Validated map loading:** Checks map dimensions, cell values, special-object counts, wall count and unexpected trailing data.
- **C89 source:** Compiles with strict C89 language mode and warning checks.

## Requirements

- GCC or another C compiler that supports C89.
- GNU Make.
- A POSIX-like terminal environment (Linux, macOS, or WSL). Terminal input uses `termios`, and the short game delay uses POSIX `nanosleep`.

Native Windows command prompt/PowerShell is not supported by the terminal-input implementation. Use WSL or a Unix-like environment.

## Build and run

Run the commands from the repository root.

### Normal mode

```bash
make clean
make
./treasure-hunt maps/default-map.txt
```

### Dark mode

```bash
make clean
make DARK=1
./treasure-hunt maps/default-map.txt
```

The `make dark` target is also available and performs a clean dark-mode build:

```bash
make dark
./treasure-hunt maps/default-map.txt
```

To return to normal mode:

```bash
make normal
./treasure-hunt maps/default-map.txt
```

### Run the test suite

```bash
make test
```

The included tests cover sample-map loading, dimensions and wall-count validation, movement, blocked/out-of-bounds movement, undo, lantern collection/undo, treasure completion, snake collision, malformed map data and missing map files. The normal-mode suite contains 21 checks; the dark-mode suite contains 23 checks, including visibility-range behaviour.

### Clean generated files

```bash
make clean
```

## Controls

| Key | Action |
|---|---|
| `W` | Move up |
| `A` | Move left |
| `S` | Move down |
| `D` | Move right |
| `U` | Undo the previous successful move |
| `Q` | Quit |

An invalid key or blocked movement does not advance the snake. A successful player movement advances the game by one turn, after which the snake attempts to move.

## Map format

Map files are whitespace-separated integers. The first line contains the number of rows and columns. The second line contains the declared number of obstacles. The remaining values describe each cell.

| Value | Meaning |
|---:|---|
| `0` | Empty floor |
| `1` | Obstacle |
| `2` | Lantern (exactly one required) |
| `3` | Player starting position (exactly one required) |
| `4` | Snake starting position (exactly one required) |
| `5` | Treasure (exactly one required) |

The map loader validates that dimensions are between 1 and 200, all cell values are in the range 0–5, the declared obstacle count matches the map, and exactly one player, snake, lantern and treasure are present.

You can use `maps/default-map.txt` as the sample map.

## Project structure

```text
c89-treasure-game/
├── Makefile
├── README.md
├── .gitignore
├── maps/
│   └── default-map.txt
├── tests/
│   ├── invalid-map.txt
│   └── test_game.c
└── src/
    ├── main.c
    ├── game.c
    ├── game.h
    ├── map.c
    ├── map.h
    ├── linkedlist.c
    ├── linkedlist.h
    ├── random.c
    ├── random.h
    ├── terminal.c
    ├── terminal.h
    ├── newSleep.c
    ├── newSleep.h
    ├── darkmode.c
    └── darkmode.h
```

## Design and implementation notes

- **Modular design:** Game rules, map parsing, terminal handling, random movement, delay handling and undo history are separated into focused modules.
- **Game state:** The `Game` structure owns the map, entity positions, status flags, visibility range and undo-stack reference.
- **Undo stack:** A linked-list stack stores snapshots of the state needed to reverse a successful move.
- **Memory ownership:** Map rows and stack nodes are dynamically allocated and released during cleanup.
- **Defensive input handling:** The map loader rejects malformed dimensions, invalid cell values, missing/duplicate special cells, mismatched obstacle counts and trailing non-whitespace data.
- **Terminal restoration:** The original terminal settings are saved and restored on normal exit, through explicit cleanup and `atexit` cleanup.

## Known limitations

- The snake uses random movement rather than pathfinding, so it does not deliberately pursue the player.
- The game is designed for a POSIX terminal and uses ANSI escape sequences for screen clearing.
- Dark mode uses Manhattan distance as its visibility calculation; it does not model line-of-sight occlusion by obstacles.
- The game has one fixed sample map, although additional maps can be supplied using the documented format.

## Attribution and refactoring note

This repository is a refactored version of a previously submitted coursework game. The original game concept and submitted implementation were used as the starting point. Original collaborators should be credited in accordance with any applicable sharing requirements.

## Licence

No open-source licence is currently specified. Contact the original contributors before reusing or redistributing the project outside the intended portfolio context.
