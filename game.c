#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "include/game.h"
#include "include/input.h"

Tile* dragging_tile=NULL;
Tile grid[GRID_SIZE][GRID_SIZE];
void initialize_game() {
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            Tile* tile = &grid[row][col];
            tile->value = 0;
            tile->rect = (SDL_Rect){col * (TILE_SIZE + GAP) + GAP, row * (TILE_SIZE + GAP) + GAP, TILE_SIZE, TILE_SIZE};
            tile->color = (SDL_Color){200, 200, 200, 255};
        }
    }
}

void render_grid(SDL_Renderer* renderer, int window_width, int window_height) {
	  
    //setting the maximum width and height of the grid
    int max_grid_width = window_width * 0.6;
    int max_grid_height = window_height * 0.6;

    // Dynamically calculate tile size and gap based on window size
    int tile_size = (max_grid_width < max_grid_height ? max_grid_width : max_grid_height) / (GRID_SIZE + 1);
    int gap = tile_size / 10; // Adjust gap to be a fraction of the tile size

    int grid_width = GRID_SIZE * tile_size + (GRID_SIZE + 1) * gap;
    int grid_height = GRID_SIZE * tile_size + (GRID_SIZE + 1) * gap;

    int offset_x = (window_width - grid_width) / 2;
    int offset_y = (window_height - grid_height) / 1.65 ;

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            Tile* tile = &grid[row][col];
            tile->rect.w = tile_size;
            tile->rect.h = tile_size;
            tile->rect.x = col * (tile_size + gap) + gap + offset_x;
            tile->rect.y = row * (tile_size + gap) + gap + offset_y;

            SDL_SetRenderDrawColor(renderer, tile->color.r, tile->color.g, tile->color.b, tile->color.a);
            render_rounded_rect(renderer, &tile->rect, tile_size / 10); // Adjust corner radius
        }
    }
}




void render_rounded_rect(SDL_Renderer* renderer, SDL_Rect* rect, int radius) {
    int x1 = rect->x;
    int y1 = rect->y;
    int x2 = rect->x + rect->w;
    int y2 = rect->y + rect->h;

    SDL_Rect inner_rect = {x1 + radius, y1 + radius, rect->w - 2 * radius, rect->h - 2 * radius};
    SDL_RenderFillRect(renderer, &inner_rect);

    for (int w = 0; w < radius; w++) {
        for (int h = 0; h < radius; h++) {
            if (w * w + h * h <= radius * radius) {
                SDL_RenderDrawPoint(renderer, x1 + radius - w, y1 + radius - h); // Top-left
                SDL_RenderDrawPoint(renderer, x2 - radius + w, y1 + radius - h); // Top-right
                SDL_RenderDrawPoint(renderer, x1 + radius - w, y2 - radius + h); // Bottom-left
                SDL_RenderDrawPoint(renderer, x2 - radius + w, y2 - radius + h); // Bottom-right
            }
        }
    }

    SDL_Rect top_rect = {x1 + radius, y1, rect->w - 2 * radius, radius};
    SDL_Rect bottom_rect = {x1 + radius, y2 - radius, rect->w - 2 * radius, radius};
    SDL_Rect left_rect = {x1, y1 + radius, radius, rect->h - 2 * radius};
    SDL_Rect right_rect = {x2 - radius, y1 + radius, radius, rect->h - 2 * radius};

    SDL_RenderFillRect(renderer, &top_rect);
    SDL_RenderFillRect(renderer, &bottom_rect);
    SDL_RenderFillRect(renderer, &left_rect);
    SDL_RenderFillRect(renderer, &right_rect);
}
