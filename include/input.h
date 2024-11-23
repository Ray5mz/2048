#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "render.h"
#include "game.h"
#define FALSE 0
#define TRUE 1

extern int is_welcome_page;
extern int game_is_running;
extern int mouseX;
extern int mouseY;
extern GameState currentState;
extern int selectedButton;
extern bool isHovered;

void return_back();
void handleMainMenuEvent(SDL_Event* event);
void handleGamePageEvent(SDL_Event* event);
void handleScorePageEvent(SDL_Event* event);
void handleMachinePageEvent(SDL_Event* event);
void handlePlayerVSMachineEvent(SDL_Event* event);
void process_input();

#endif
