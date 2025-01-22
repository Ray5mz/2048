#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#define GRID_SIZE 4
#define TILE_SIZE 100
#define GAP 10
#define MAX_HIGH_SCORES 5
#define MAX_NAME_LENGTH 32
typedef enum {
    DIR_NONE, // No direction
    DIR_UP,   // Up direction
    DIR_DOWN, // Down direction
    DIR_LEFT, // Left direction
    DIR_RIGHT // Right direction
} Direction;

typedef struct {
    int scnd, mint;
} Time;
typedef struct {
    char name[MAX_NAME_LENGTH];
    int score;
Time time;
} HighScore;

typedef struct {
    HighScore highScores[MAX_HIGH_SCORES];
    int count; // Current number of high scores stored
} HighScoreBoard;

extern HighScoreBoard highScoreBoard;

static Time t,rect = {0, 0};
static Uint32 lastTime = 0;
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
		GS_NOTPLAYING,
} GameStates;

typedef struct {
    int board[GRID_SIZE * GRID_SIZE];
    int ai_board[GRID_SIZE * GRID_SIZE]; // AI grid
    GameStates state, Mstate;
    Actions action, Maction;
    int score, Mscore, moves, Mmoves;
    int last_inserted, Mlast_inserted;
    bool has_moved, Mhas_moved;
		Time time;
		Direction last_directions;
} Game;
void rendtimer(Game* game);
extern void create_file_if_not_exists(const char* filename);

bool has_won(Game* game);
bool has_lost(Game* game);
int empty_tiles(Game* game);
bool move_cell_maybe_break(Game* game, int* target, int* src);
void move_right(Game* game);
void move_left(Game* game);
void move_down(Game* game);
void move_up(Game* game);
void update(Game* game, double delta);
void render_thegrid(SDL_Renderer* renderer, int window_width, int window_height, Game* game, bool is_ai, int offset_x);
void initialize_game(Game* game);
void initialize_gamePVM(Game* game);
void render_score_and_moves(Game* game, SDL_Renderer* renderer);
void render_splash_screen(SDL_Renderer* renderer, int window_width, int window_height, const char* text, SDL_Color color, bool playerVmachine, int offsetx);
void add_high_score(const char* name, Game* game, int i, const char* filename);
void ask_for_player_name(Game* game, char* playerName);
void initialize_gameM(Game* game);
// AI move function
void PlayerVSMachine(Game* game, double delta); // Player vs Machine function
void Machine(Game* game, double delta);
void Mmove_right(Game*game);
void Mmove_left(Game* game);
void Mmove_down(Game* game);
void Mmove_up(Game* game);
#endif // GAME_H
