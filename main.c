#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "include/render.h"
#include "include/input.h"
#include "include/game.h"
#include <SDL2/SDL_mixer.h>
#define FALSE 0
#define TRUE 1
Mix_Music* music = NULL; // Global music object

// VIDEO
AVFormatContext* formatContext = NULL;
AVCodecContext* codecContext = NULL;
AVFrame* frame = NULL;
AVPacket* packet = NULL;
struct SwsContext* swsContext = NULL;
SDL_Texture* videoTexture = NULL;
int videoStreamIndex = -1;

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
HighScoreBoard highScoreBoard = { .count = 0 }; // Initialize high score board

void cleanup() {
    if (videoTexture) SDL_DestroyTexture(videoTexture);
    if (swsContext) sws_freeContext(swsContext);
    if (frame) av_frame_free(&frame);
    if (packet) av_packet_free(&packet);
    if (codecContext) avcodec_free_context(&codecContext);
    if (formatContext) avformat_close_input(&formatContext);
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

void play_music(const char* filepath) {
    // Load the music file
    Mix_Music* music = Mix_LoadMUS(filepath);
    if (!music) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        return;
    }

    // Play the music
    if (Mix_PlayMusic(music, -1) == -1) { // -1 means loop indefinitely
        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
    }
}

void cleanup_music() {
    Mix_HaltMusic(); // Stop any playing music
  // Free the music file
    Mix_CloseAudio(); // Close SDL_mixer
}
int main(int argc, char* argv[]) {
    game_is_running = initialize_window();
    if (!game_is_running) {
        cleanup(); // Ensure cleanup is called if initialization fails
        return -1;
    }

    const char* videoPath = "assets/cyberpunk.mp4";
    if (!loadVideo(videoPath, renderer)) {
        printf("Failed to load video\n");
        cleanup();
        return -1;
    }

    // Main loop
    while (game_is_running) {
        process_input(&game); // Pass the game variable
        render(&game); // Pass the game variable
		    if (currentState == WELCOME_PAGE){
            update_balls();
            			      
        } else if (currentState == GAME_PAGE) {
            update(&game, 0.016); // Update the game state
        } else if (currentState == PLAYERVSMACHINE_PAGE) {
            PlayerVSMachine(&game, 0.016); // Update the game state for Player vs Machine
			      update_balls();
        } else if (currentState == MACHINE_PAGE){
            Machine(&game, 0.016);
			      update_balls();
        }

        SDL_Delay(16); // Delay to limit CPU usage (~60 frames per second)
    }

    // Clean up resources
    cleanup();
    return 0;
}
