#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include "cgame.h"
#include "render.h"
#define GRID_SIZE 4

// Function to fill the grid with random "2" values and render them
int add_random_tile(Game* game);
#endif // UTILS_H
