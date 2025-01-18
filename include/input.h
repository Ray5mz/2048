#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "render.h"
#include "game.h"
#include "utils.h"

#define FALSE 0
#define TRUE 1

extern int is_welcome_page;
extern int game_is_running;
extern int mouseX;
extern int mouseY;
extern GameState currentState;
extern int selectedButton;
extern bool isHovered;
extern char *ply_name;
extern int score;
extern HighScoreBoard highScoreBoard;
void return_back(Game* game);
void handleMainMenuEvent(SDL_Event* event);
void handleGamePageEvent(Game* game);
void handleScorePageEvent(SDL_Event* event);
void handleMachinePageEvent(SDL_Event* event);
void handlePlayerVSMachineEvent(SDL_Event* event);
void process_input(Game* game);
void ai_move(Game* game); 
#endif // INPUT_H
