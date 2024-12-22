#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "include/input.h"
#include "include/render.h"
#include "include/game.h"

extern int game_is_running;
extern GameState currentState;
extern int mouseX;
extern int mouseY;
extern int selectedButton;
extern char *ply_name; // Ensure ply_name is declared as an external variable

void return_back() {
    if (currentState != WELCOME_PAGE && currentState != MAIN_MENU) {
        currentState = MAIN_MENU;
        loadMenuTextures(); // Load menu textures when transitioning to the main menu
    }
}

void process_input(Game* game) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game_is_running = FALSE; // Stop the game loop
        } else if (event.type == SDL_KEYDOWN) {
            if (currentState == WELCOME_PAGE) {
                // Any key press transitions to the main menu
                currentState = MAIN_MENU;
                loadMenuTextures(); // Load menu textures when transitioning to the main menu
            } else if (currentState == MAIN_MENU) {
                // Handle specific key presses for main menu navigation
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (selectedButton > 0) {
                            selectedButton--; // Move up
                        }
                        break;
                    case SDLK_DOWN:
                        if (selectedButton < 4) {
                            selectedButton++; // Move down
                        }
                        break;
                    case SDLK_RETURN: // Enter key
                        printf("Button %d selected!\n", selectedButton);
                        switch (selectedButton) {
                            case 0: currentState = GAME_PAGE; break;
                            case 1: currentState = MACHINE_PAGE; break;
                            case 2: currentState = PLAYERVSMACHINE_PAGE; break;
                            case 3: currentState = SCORE_PAGE; break;
                            case 4: game_is_running = FALSE; break;
                        }
                        break;
                    case SDLK_ESCAPE:
                        return_back();
                        break;
                }
				    } else if (currentState == GAME_PAGE){
                  switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:
                        game->action = A_MOVE_RIGHT;
                        break;
                    case SDLK_LEFT:
                        game->action = A_MOVE_LEFT;
                        break;
                    case SDLK_DOWN:
                        game->action = A_MOVE_DOWN;
                        break;
                    case SDLK_UP:
                        game->action = A_MOVE_UP;
                        break;
                    case SDLK_ESCAPE:
                        return_back();
                        break;
                }            
				       
            } else {
                // Handle ESC key to return to the main menu
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return_back();
                }
            }
        } else if (event.type == SDL_MOUSEMOTION) {
            mouseX = event.motion.x;
            mouseY = event.motion.y;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            switch (currentState) {
                case WELCOME_PAGE:
                    currentState = MAIN_MENU;
                    loadMenuTextures(); // Load menu textures when transitioning to the main menu
                    break;
                case MAIN_MENU:
                    handleMainMenuEvent(&event); // New function for the main menu
                    break;
                case GAME_PAGE:
					          initialize_game(game); // Initialize the game when transitioning to the game page
                    handleGamePageEvent(game); // Use the game variabl 
                      break;
                case SCORE_PAGE:
                    handleScorePageEvent(&event); // Placeholder for future
                    break;
                case PLAYERVSMACHINE_PAGE:
                    handlePlayerVSMachineEvent(&event);
                    break;
                case MACHINE_PAGE:
                    handleMachinePageEvent(&event); // Placeholder for future
                    break;
            }
        }
    }
}

void handleMainMenuEvent(SDL_Event* event) {
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        // Retrieve mouse position
        int mouseX = event->button.x;
        int mouseY = event->button.y;

        // Retrieve window dimensions
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        // Define button dimensions and spacing
        int buttonWidth = 300;
        int buttonHeight = 70;
        int buttonSpacing = 60;
        int startY = ((windowHeight - ((buttonHeight + buttonSpacing) * 5 - buttonSpacing)) / 2) + 50;

        for (int i = 0; i < 5; i++) {
            SDL_Rect buttonRect = {
                (windowWidth - buttonWidth) / 2,
                startY + i * (buttonHeight + buttonSpacing),
                buttonWidth,
                buttonHeight
            };

            // Check if mouse click is inside this button
            if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonWidth &&
                mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonHeight) {
                printf("Button %d clicked!\n", i);

                // Switch state or quit based on the button clicked
                switch (i) {
                    case 0: currentState = GAME_PAGE; break;   // Play button
                    case 1: currentState = MACHINE_PAGE; break; // Machine button
                    case 2: currentState = PLAYERVSMACHINE_PAGE; break; // Player vs Machine button
                    case 3: currentState = SCORE_PAGE; break; // Score button
                    case 4: game_is_running = FALSE; break;   // Quit button
                }
                break; // Exit loop once the clicked button is handled
            }
        }
    }
}

void handleGamePageEvent(Game* game) {
    // No need to poll for events here since we already have the event in process_input
    if (currentState == GAME_PAGE) {
        switch (game->action) {
            case A_MOVE_RIGHT:
                move_right(game);
                break;
            case A_MOVE_LEFT:
                move_left(game);
                break;
            case A_MOVE_DOWN:
                move_down(game);
                break;
            case A_MOVE_UP:
                move_up(game);
                break;
            default:
                break;
        }
    }
}
void handleScorePageEvent(SDL_Event* event){}
void handleMachinePageEvent(SDL_Event* event){}
void handlePlayerVSMachineEvent(SDL_Event* event){}
