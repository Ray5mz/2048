#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "include/cgame.h"

#define GRID_SIZE 4

int add_random_tile(Game* game) {
    int tiles = empty_tiles(game);
    int tile = ((rand() / 16) * tiles) / (RAND_MAX / 16);
    int n = 0;
    for (int i = 0; i < 16; i++) {
        if (game->board[i] == 0) {
            if (n == tile) {
                game->board[i] = rand() > RAND_MAX / 3 * 2 ? 2 : 4;
                return i;
            } else {
                n++;
            }
        }
    }
    return -1;
}
