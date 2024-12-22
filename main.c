#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "include/render.h"
#include "include/input.h"
#include "include/game.h"

#define FALSE 0
#define TRUE 1

int game_is_running;
GameState currentState = WELCOME_PAGE;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* smallFont = NULL;
TTF_Font* largeFont = NULL;
SDL_Texture* textTexture = NULL;
SDL_Texture* startTextTexture = NULL;

int mouseX, mouseY;
int selectedButton = -1;
bool isHovered;

char *ply_name = NULL; // Initialize ply_name

Game game; // Declare the game variable

void cleanup() {
    if (startTextTexture) {
        SDL_DestroyTexture(startTextTexture);
        startTextTexture = NULL;
    }
    if (textTexture) {
        SDL_DestroyTexture(textTexture);
        textTexture = NULL;
    }
    if (largeFont) {
        TTF_CloseFont(largeFont);
        largeFont = NULL;
    }
    if (smallFont) {
        TTF_CloseFont(smallFont);
        smallFont = NULL;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    game_is_running = initialize_window();
    if (!game_is_running) {
        cleanup(); // Ensure cleanup is called if initialization fails
        return -1;
    }

    // Main loop
    while (game_is_running) {
        process_input(&game); // Pass the game variable
        render(&game); // Pass the game variable
        if (currentState == GAME_PAGE) {
            update(&game, 0.016); // Update the game state
        }
        SDL_Delay(16); // Delay to limit CPU usage (~60 frames per second)
    }

    // Clean up resources
    cleanup();
    return 0;
}
