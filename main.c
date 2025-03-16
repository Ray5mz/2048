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
#include "include/cgame.h"
#include <SDL2/SDL_image.h>
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
bool resume;
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
HighScoreBoard highScoreBoard = { .count = 0 };
Game game; // Declare the game variable
void create_file_if_not_exists(const char* filename) {
 
	FILE* file = fopen(filename, "a"); // Open in append mode to create the file if it doesn't exist
    if (!file) {
        printf("Error creating file: %s\n", strerror(errno));
        return;
    }
    fclose(file);
    printf("File %s created or already exists\n", filename);
}

void load_high_scores(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file for reading: %s\n", strerror(errno));
        return;
    }

    highScoreBoard.count = 0;
    while (highScoreBoard.count < MAX_HIGH_SCORES) {
        if (fscanf(file, "%31s %d %d %d",
                   highScoreBoard.highScores[highScoreBoard.count].name,
                   &highScoreBoard.highScores[highScoreBoard.count].score,
                   &highScoreBoard.highScores[highScoreBoard.count].time.mint,
                   &highScoreBoard.highScores[highScoreBoard.count].time.scnd) == 4) {
            highScoreBoard.count++;
        } else {
            break;
        }
    }

    fclose(file);
    printf("High scores loaded from %s\n", filename);
}
void replace_high_score(const char* name, Game* game, int index, const char* filename) {
    if (index < 0 || index >= MAX_HIGH_SCORES) {
        printf("Error: Invalid index %d for highScoreBoard\n", index);
        return;
    }

    // Read all high scores into memory
    HighScoreBoard tempBoard = { .count = 0 };
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file for reading: %s\n", strerror(errno));
        return;
    }

    while (tempBoard.count < MAX_HIGH_SCORES) {
        if (fscanf(file, "%31s %d %d %d",
                   tempBoard.highScores[tempBoard.count].name,
                   &tempBoard.highScores[tempBoard.count].score,
                   &tempBoard.highScores[tempBoard.count].time.mint,
                   &tempBoard.highScores[tempBoard.count].time.scnd) == 4) {
            tempBoard.count++;
        } else {
            break;
        }
    }
    fclose(file);

    // Replace the score at the specified index
    strcpy(tempBoard.highScores[index].name, name);
    tempBoard.highScores[index].score = game->score;
    tempBoard.highScores[index].time = game->time;

    // Write all high scores back to the file
    file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file for writing: %s\n", strerror(errno));
        return;
    }

    for (int i = 0; i < tempBoard.count; i++) {
        fprintf(file, "%s %d %d %d\n",
                tempBoard.highScores[i].name,
                tempBoard.highScores[i].score,
                tempBoard.highScores[i].time.mint,
                tempBoard.highScores[i].time.scnd);
    }

    fclose(file);
    printf("High score replaced at index %d\n", index);
}

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
Mix_VolumeMusic(10);
}

void cleanup_music() {
    Mix_HaltMusic(); // Stop any playing music
  // Free the music file
    Mix_CloseAudio(); // Close SDL_mixer
}
int main(int argc, char* argv[]) {
    // Create the score file if it doesn't exist
    create_file_if_not_exists("score.txt");

    // Load high scores at startup
    load_high_scores("score.txt");

    // Initialize the game window
    game_is_running = initialize_window();
    if (!game_is_running) {
        cleanup(); // Ensure cleanup is called if initialization fails
        return -1;
    }

    // Load the background image
    SDL_Texture* background_texture = load_background_image(renderer, "assets/background.png");
    if (!background_texture) {
        printf("Failed to load background image!\n");
        cleanup(); // Clean up resources before exiting
        return -1;
    }

    // Initialize the game state
    Game game;
    initialize_game(&game); // Initialize the game

    // Main loop
    while (game_is_running) {
        process_input(&game); // Handle input
        render(&game, background_texture); // Render the game with the background texture

        // Update game state based on the current state
        switch (currentState) {
            case WELCOME_PAGE:
                update_balls();
                break;

            case GAME_PAGE:
                update(&game, 0.016); // Update the game state
                break;

            case PLAYERVSMACHINE_PAGE:
                PlayerVSMachine(&game, 0.016); // Update the game state for Player vs Machine
                update_balls();
                break;

            case MACHINE_PAGE:
                Machine(&game, 0.016); // Update the game state for Machine
                update_balls();
                break;

            default:
                break;
        }

        SDL_Delay(16); // Delay to limit CPU usage (~60 frames per second)
    }

    // Clean up resources
    SDL_DestroyTexture(background_texture); // Free the background texture
    cleanup(); // Clean up other resources
    return 0;
}
