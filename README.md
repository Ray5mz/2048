# 2048
2048 game using SDL library in C, group project for DSA class 
```
project-root/

├── main.c          // Entry point: initializes SDL, main game loop, coordinates game components
├── game.c          // Core game logic: board setup, tile movement, merging, game-over checks
├── input.c         // Input handling: processes keyboard events and directs movements
├── render.c        // SDL rendering: draws board, tiles, score, and any animations
├── utils.c         // Helper functions: random number generation, utility functions for board management
├── include/
│   ├── game.h          // Header for game logic functions and GameState struct
│   ├── input.h         // Header for input handling functions
│   ├── render.h        // Header for rendering functions
│   ├── utils.h         // Header for utility/helper functions
├── assets/ (optional) 
│   ├── font.ttf        // Font file for displaying text (optional)
│   ├── icon.png        // Icon or image assets if needed for the game interface
├── build/              // Directory for compiled binaries and object files (generated after building)
├── Makefile            // Build automation: compiles and links files for easy project management
``` 

## Command for compilation
`gcc -g -o "2048v0.2.exe" main.c render.c input.c utils.c cgame.c -I include -lSDL2 -lSDL2_ttf -lSDL2_image -lm`
