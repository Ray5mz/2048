# 2048
A group project for DSA class.

A 2048 game implementation using SDL2 in C. This project involves creating a simple version of the classic 2048 game, where players slide tiles on a grid to combine them into a tile of 2048.

## Features
- **User**: Classic gameplay for the player.
- **User vs Machine**: Play against an AI opponent.
- **User Interface**: Simple, clean interface with animated tiles.
- **Ranking System**: Keeps track of the best scores.
- **Sound and Music**: Background music and sound effects.

## Requirements
- **SDL2**: Make sure SDL2 is installed on your system. Follow the instructions on the official SDL website to install it: [SDL2 Installation](https://wiki.libsdl.org/Installation).
- **SDL2_ttf**: Required for rendering text. Install it similarly to SDL2.
- **C Compiler**: GCC or Clang recommended for compiling the project.

### Installing Dependencies
To install the dependencies on Linux (for example, Ubuntu), run:
```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

and for Arch linux based systems:
```bash
sudo pacman -S sdl2 sdl2_ttf sdl2_image sdl2_mixer
```
```bash
sudo pacman -S ffmpeg
```


## Directory Structure
```
|-- assets/
|   |-- Letter.ttf              # Font file used for rendering text in the game
|   |-- LilitaOne-Regular.ttf   # Font file used for rendering text in the game
|   |-- NType82.otf             # Font file used for rendering text in the game
|   |-- NType82.ttf             # Font file used for rendering text in the game
|   |-- Ndot-55.ttf             # Font file used for rendering text in the game
|   |-- OFL.txt                 # Font license information
|   |-- assets_watchdogs.mp3    # Background music
|   |-- cyberpunk.mp4           # Video file for the game intro
|   |-- icon.jpg                # Icon used in the game window
|
|-- include/
|   |-- cgame.h                 # Contains game logic declarations
|   |-- input.h                 # Input handling functions
|   |-- render.h                # Rendering functions for drawing the game board and UI
|   |-- utils.h                 # Utility functions for randomization and other helper tasks
|
|-- cgame.c                 # Core game logic implementation
|-- input.c                 # Input handling code
|-- render.c                # Rendering logic using SDL
|-- utils.c                 # Helper functions and utilities
|-- main.c
|
|-- README.md                  # Project documentation
```

## Installation and Compilation
After installing files, we can complie and make the executable file of the game(noting that the Directory structure need to be respected in order to be abke to compile)

we can do this by typing the following command:
```shell
gcc -g -o "NumberSlide.exe" main.c render.c input.c utils.c cgame.c -I include -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lavcodec -lavformat -lavutil -lswscale -lm
```


## How to Play
-  Arrow keys: Move tiles on the grid.
-  Press 'Esc': Exit the game.
-  when you exit without finsing your game, you will be asked if you want to resume your last game or start a new one in your next launch of the game

---

## Special Thanks:
Thank you to @ayahbash for working and contrinuting with awesome ideas into this project
