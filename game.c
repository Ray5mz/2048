#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h> // Include the math header
#include "include/cgame.h"
#include "include/utils.h"
#include "include/input.h"
// Function to draw a rounded rectangle
void render_rounded_rect(SDL_Renderer* renderer, SDL_Rect* rect, int radius) {
    int x = rect->x;
    int y = rect->y;
    int w = rect->w;
    int h = rect->h;

    // Draw the main rectangle
    SDL_RenderDrawRect(renderer, rect);

    // Draw the corners
    SDL_RenderDrawLine(renderer, x + radius, y, x + w - radius, y);
    SDL_RenderDrawLine(renderer, x + radius, y + h, x + w - radius, y + h);
    SDL_RenderDrawLine(renderer, x, y + radius, x, y + h - radius);
    SDL_RenderDrawLine(renderer, x + w, y + radius, x + w, y + h - radius);

    // Draw the arcs (using small circles to approximate)
    for (int i = 0; i < 90; i += 5) {
        int angle = i * (M_PI / 180.0);
        int x1 = x + radius + (int)(radius * cos(angle));
        int y1 = y + radius - (int)(radius * sin(angle));
        SDL_RenderDrawPoint(renderer, x1, y1);

        angle = (90 + i) * (M_PI / 180.0);
        x1 = x + w - radius + (int)(radius * cos(angle));
        y1 = y + radius - (int)(radius * sin(angle));
        SDL_RenderDrawPoint(renderer, x1, y1);

        angle = (180 + i) * (M_PI / 180.0);
        x1 = x + w - radius + (int)(radius * cos(angle));
        y1 = y + h - radius - (int)(radius * sin(angle));
        SDL_RenderDrawPoint(renderer, x1, y1);

        angle = (270 + i) * (M_PI / 180.0);
        x1 = x + radius + (int)(radius * cos(angle));
        y1 = y + h - radius - (int)(radius * sin(angle));
        SDL_RenderDrawPoint(renderer, x1, y1);
    }
}

void render_thegrid(SDL_Renderer* renderer, int window_width, int window_height, Game* game) {
    // Clear the screen with a background color
    SDL_SetRenderDrawColor(renderer, 107, 107, 223, 255);
    SDL_RenderClear(renderer);

    // Calculate the position and size of each tile
    int tile_size = TILE_SIZE;
    int gap = GAP;
    int start_x = (window_width - (GRID_SIZE * tile_size + (GRID_SIZE - 1) * gap)) / 2;
    int start_y = (window_height - (GRID_SIZE * tile_size + (GRID_SIZE - 1) * gap)) / 2;

    // Render each tile
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            int value = game->board[y * GRID_SIZE + x];
            int tile_x = start_x + x * (tile_size + gap);
            int tile_y = start_y + y * (tile_size + gap);

            SDL_Rect tile_rect = {tile_x, tile_y, tile_size, tile_size};

            // Set the tile color based on the value
            SDL_Color tile_color = {255, 255, 255, 255}; // White color
            SDL_SetRenderDrawColor(renderer, tile_color.r, tile_color.g, tile_color.b, tile_color.a);

            // Render the rounded rectangle
            render_rounded_rect(renderer, &tile_rect, 10);

            // Optionally, render the value inside the tile
            if (value > 0) {
                TTF_Font* font = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 48);
                if (font) {
                    char value_str[10];
                    snprintf(value_str, sizeof(value_str), "%d", value);
                    SDL_Color text_color = {0, 0, 0, 255}; // Black color
                    SDL_Surface* text_surface = TTF_RenderText_Blended(font, value_str, text_color);
                    if (text_surface) {
                        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
                        SDL_Rect text_rect = {
                            tile_x + (tile_size - text_surface->w) / 2,
                            tile_y + (tile_size - text_surface->h) / 2,
                            text_surface->w,
                            text_surface->h
                        };
                        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
                        SDL_DestroyTexture(text_texture);
                        SDL_FreeSurface(text_surface);
                    }
                    TTF_CloseFont(font);
                }
            }
        }
    }
}

void initialize_game(Game* game) {
    // Initialize the game state
    game->state = GS_PLAYING;
    game->action = A_NONE;
    game->score = 0;
    game->moves = 0;
    game->last_inserted = -1;
    game->has_moved = false;

    // Initialize the board with zeros
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        game->board[i] = 0;
    }

    // Add initial tiles
    add_random_tile(game);
    add_random_tile(game);
}

bool has_won(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (game->board[y * GRID_SIZE + x] == 11) {
                return true;
            }
        }
    }
    return false;
}

bool has_lost(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (game->board[y * GRID_SIZE + x] == 0) {
                return false;
            }
        }
    }
    for (int a = 0; a < GRID_SIZE - 1; a++) {
        for (int b = 0; b < GRID_SIZE; b++) {
            if (game->board[a * GRID_SIZE + b] == game->board[(a + 1) * GRID_SIZE + b] ||
                game->board[b * GRID_SIZE + a] == game->board[b * GRID_SIZE + a + 1]) {
                return false;
            }
        }
    }
    return true;
}

int empty_tiles(Game* game) {
    int amount = 0;
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (game->board[y * GRID_SIZE + x] == 0) {
                amount++;
            }
        }
    }
    return amount;
}

bool move_cell_maybe_break(Game* game, int* target, int* src) {
    if (*target == 0 && *src != 0) {
        *target = *src;
        *src = 0;
        game->has_moved = true;
    } else if (*target == *src && *target != 0 && *src != 0) {
        *target = *target + *target;
        *src = 0;
        game->score += pow(2, *target);
        game->has_moved = true;
        return true;
    } else if (*src != 0) {
        return true;
    }
    return false;
}

void move_right(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int ix = GRID_SIZE - 1; ix >= 0; ix--) {
            for (int jx = ix - 1; jx >= 0; jx--) {
                if (move_cell_maybe_break(game, &game->board[y * GRID_SIZE + ix], &game->board[y * GRID_SIZE + jx])) {
                    break;
                }
            }
        }
    }
}

void move_left(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int ix = 0; ix < GRID_SIZE - 1; ix++) {
            for (int jx = ix + 1; jx < GRID_SIZE; jx++) {
                if (move_cell_maybe_break(game, &game->board[y * GRID_SIZE + ix], &game->board[y * GRID_SIZE + jx])) {
                    break;
                }
            }
        }
    }
}

void move_down(Game* game) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int iy = GRID_SIZE - 1; iy >= 0; iy--) {
            for (int jy = iy - 1; jy >= 0; jy--) {
                if (move_cell_maybe_break(game, &game->board[iy * GRID_SIZE + x], &game->board[jy * GRID_SIZE + x])) {
                    break;
                }
            }
        }
    }
}

void move_up(Game* game) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int iy = 0; iy < GRID_SIZE - 1; iy++) {
            for (int jy = iy + 1; jy < GRID_SIZE; jy++) {
                if (move_cell_maybe_break(game, &game->board[iy * GRID_SIZE + x], &game->board[jy * GRID_SIZE + x])) {
                    break;
                }
            }
        }
    }
}

void update(Game* game, double delta) {
    if (game->state == GS_PLAYING) {
        game->has_moved = false;
        handleGamePageEvent(game);

        if (game->action != A_NONE) {
            game->action = A_NONE;

            if (game->has_moved) {
                game->last_inserted = add_random_tile(game);
            } else {
                game->last_inserted = -1;
            }

            game->moves++;
        }

        if (has_won(game)) {
            game->state = GS_WON;
        } else if (has_lost(game)) {
            game->state = GS_LOST;
        }
    }
}
