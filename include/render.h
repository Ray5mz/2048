#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

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

extern int selectedButton;
extern int mouseX,mouseY;
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
extern void initialize_game();

extern void render_grid(SDL_Renderer* renderer, int window_width,int window_height);
extern void render_rounded_rect(SDL_Renderer* renderer, SDL_Rect* rect, int radius);
extern void handle_tile_drag(SDL_Event* event);

int initialize_window(void);
void render();
void transition_to_main_menu();
void welcome_page();
void loadMenuTextures();
void renderMainMenu();
void cleanupMenuTextures();

void renderGamePage();
void renderPlayerVSMachine();
void renderMachinePage();
void renderScorePage();

#endif
