#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

#define GRID_SIZE 4
#define TILE_SIZE 100
#define GAP 10

typedef struct {
    int value;
    SDL_Rect rect;
    SDL_Color color;
    int is_dragging;
} Tile;

extern SDL_Renderer* renderer; // Declare renderer
extern Tile* dragging_tile;    // Declare dragging_tile (only declaration, not definition)
extern Tile grid[GRID_SIZE][GRID_SIZE]; // Declare grid (only declaration, not definition)

void render_rounded_rect(SDL_Renderer* renderer, SDL_Rect* rect, int radius);
void initialize_game();
void render_grid(SDL_Renderer* renderer, int window_width, int window_height);

#endif
