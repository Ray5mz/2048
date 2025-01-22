#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include "cgame.h"
#include "render.h"
#define GRID_SIZE 4
extern HighScoreBoard highScoreBoard;
// Function to fill the grid with random "2" values and render them
int add_random_tile(Game* game);
int add_random_tileM(Game* game);
#endif // UTILS_H
