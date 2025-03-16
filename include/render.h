#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "cgame.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL_mixer.h>
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
    PLAYERVSMACHINE_PAGE,
		GAME_PAUSED,
} GameState;
extern void play_music(const char* filepath);
extern AVFormatContext* formatContext;
extern AVCodecContext* codecContext;
extern AVFrame* frame;
extern AVPacket* packet;
extern struct SwsContext* swsContext;
extern SDL_Texture* videoTexture;
extern int videoStreamIndex;
extern Mix_Music* music;
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
extern int game_is_running;
extern bool isHovered;
extern bool resume;
void load_game_state(Game* game);
SDL_Texture* load_background_image(SDL_Renderer* renderer, const char* filepath); 
void play_music(const char* filepath);
void replace_high_score(const char* name, Game* game, int index, const char* filename);
void load_high_scores(const char* filename);
void initialize_game(Game* game);
void render_grid(SDL_Renderer* renderer, int window_width, int window_height, Game* game);
void transition_to_main_menu();
void welcome_page();
void loadMenuTextures();
void renderMainMenu();
void cleanupMenuTextures();
char* render_name_input(char *name);
void renderGamePage(Game* game);
void renderPlayerVSMachine(Game* game);
void renderMachinePage(Game* game);
void renderScorePage();
int initialize_window(void);
void render(Game* game, SDL_Texture* background_texture);
void setup_balls();
void update_balls();
bool loadVideo(const char* filepath, SDL_Renderer* renderer);
void renderVideoFrame(SDL_Renderer* renderer);
void timertext(SDL_Renderer* renderer, Game* game);
#endif
