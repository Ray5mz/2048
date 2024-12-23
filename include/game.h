#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define GRID_SIZE 4
#define TILE_SIZE 100
#define GAP 10
#define MAX_SCORES 5 //this is for the maximum score that can be saved

typedef enum {
    A_NONE,
    A_MOVE_RIGHT,
    A_MOVE_LEFT,
    A_MOVE_DOWN,
    A_MOVE_UP,
} Actions;

typedef enum {
    GS_PLAYING,
    GS_LOST,
    GS_WON,
} GameStates;

typedef struct {  //this has evrything we need concerning the Gameplay
    int board[4 * 4];
    GameStates state;
    Actions action;
    int score, moves;
    int last_inserted;
    bool has_moved;
} Game;

bool has_won(Game* game);
bool has_lost(Game* game);
int empty_tiles(Game* game);
bool move_cell_maybe_break(Game* game, int* target, int* src);
void move_right(Game* game);
void move_left(Game* game);
void move_up(Game* game);
void move_down(Game* game);
void update(Game* game, double delta);
void render_thegrid(SDL_Renderer* renderer, int window_width, int window_height, Game* game);
void initialize_game(Game* game);


#endif // GAME_H
