#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "game.h"

#define GRID_SIZE 4
#define MAX_NAME_LEN 20
#define FALSE 0
#define TRUE 1

typedef enum {
    WELCOME_PAGE,
    MAIN_MENU,
    MACHINE_PAGE,
    SCORE_PAGE,
    GAME_PAGE,
    PLAYERVSMACHINE_PAGE
} GameState;
extern HighScoreBoard highScoreBoard;
extern int selectedButton;
extern int mouseX, mouseY;
extern GameState currentState;
extern SDL_Renderer* renderer;
extern TTF_Font* largeFont;
extern TTF_Font* smallFont;
extern SDL_Texture* textTexture;
extern SDL_Window* window;
extern SDL_Texture* startTextTexture;
extern int is_welcome_page;
extern int game_is_running;
extern bool isHovered;
extern void initialize_game(Game* game);

extern void render_grid(SDL_Renderer* renderer, int window_width, int window_height, Game* game);

extern void transition_to_main_menu();
extern void welcome_page();
extern void loadMenuTextures();
extern void renderMainMenu();
extern void cleanupMenuTextures();
extern char* render_name_input(char *name);
void renderGamePage(Game* game);
void renderPlayerVSMachine(Game* game);
extern void renderMachinePage();
extern void renderScorePage();

int initialize_window(void);
void render(Game* game);
void transition_to_main_menu();
void welcome_page();
void loadMenuTextures();
void renderMainMenu();
void cleanupMenuTextures();
void renderMachinePage();
void renderScorePage();
void setup_ball();
void update_balls();
#endif
