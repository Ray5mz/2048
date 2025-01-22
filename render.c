#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "include/render.h"
#include "include/cgame.h"
#include "include/input.h"
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL_mixer.h>


#define MAX_BALLS 30

struct ball {
    float x, y;
    int width, height;
    float vy;
    int alpha;
};

static Uint32 last_frame_time = 0;
struct ball balls[MAX_BALLS];
int ball_count = 0;

void render_movement_arrows(SDL_Renderer* renderer, int window_width, int window_height, Direction direction) {
    // Define arrow size and spacing
    int arrowSize = 50; // Size of each arrow
    int spacing = 20;   // Spacing between arrows
    int thickness = 3;  // Thickness of the arrows (boldness)

    // Define arrow color
    SDL_Color arrowColor = {255, 255, 255, 255}; // White color

    // Calculate the position for the arrows (top-right corner)
    int startX = window_width - arrowSize - spacing; // Start from the right edge
    int startY = spacing; // Start from the top edge

    // Set the render color to the arrow color
    SDL_SetRenderDrawColor(renderer, arrowColor.r, arrowColor.g, arrowColor.b, arrowColor.a);

    // Render arrows based on the direction
    switch (direction) {
        case DIR_UP:
            // Render up arrow (bold)
            for (int i = 0; i < thickness; i++) {
                SDL_RenderDrawLine(renderer, startX + i, startY, startX - arrowSize / 2 + i, startY + arrowSize);
                SDL_RenderDrawLine(renderer, startX + i, startY, startX + arrowSize / 2 + i, startY + arrowSize);
            }
            break;

        case DIR_DOWN:
            // Render down arrow (bold)
            for (int i = 0; i < thickness; i++) {
                SDL_RenderDrawLine(renderer, startX + i, startY + arrowSize, startX - arrowSize / 2 + i, startY);
                SDL_RenderDrawLine(renderer, startX + i, startY + arrowSize, startX + arrowSize / 2 + i, startY);
            }
            break;

        case DIR_LEFT:
            // Render left arrow (bold)
            for (int i = 0; i < thickness; i++) {
                SDL_RenderDrawLine(renderer, startX - arrowSize + i, startY + arrowSize / 2, startX + i, startY);
                SDL_RenderDrawLine(renderer, startX - arrowSize + i, startY + arrowSize / 2, startX + i, startY + arrowSize);
            }
            break;

        case DIR_RIGHT:
            // Render right arrow (bold)
            for (int i = 0; i < thickness; i++) {
                SDL_RenderDrawLine(renderer, startX + i, startY + arrowSize / 2, startX - arrowSize + i, startY);
                SDL_RenderDrawLine(renderer, startX + i, startY + arrowSize / 2, startX - arrowSize + i, startY + arrowSize);
            }
            break;

        default:
            // No direction, do nothing
            break;
    }
}bool loadVideo(const char* filepath, SDL_Renderer* renderer) {
    // Open the video file
    if (avformat_open_input(&formatContext, filepath, NULL, NULL) != 0) {
        printf("Unable to open video file: %s\n", filepath);
        return false;
    }

    // Find stream info
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        printf("Unable to find stream info for video file: %s\n", filepath);
        return false;
    }

    // Find the video stream
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }
    if (videoStreamIndex == -1) {
        printf("No video stream found in file: %s\n", filepath);
        return false;
    }

    // Get the video codec
    AVCodecParameters* codecParams = formatContext->streams[videoStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) {
        printf("Unsupported codec for video file: %s\n", filepath);
        return false;
    }

    // Initialize the codec context
    codecContext = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecContext, codecParams);
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        printf("Unable to open codec for video file: %s\n", filepath);
        return false;
    }

    // Prepare buffers
    frame = av_frame_alloc();
    packet = av_packet_alloc();

    // Initialize the conversion context
    swsContext = sws_getContext(
        codecContext->width, codecContext->height, codecContext->pix_fmt,
        codecContext->width, codecContext->height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR, NULL, NULL, NULL
    );

    // Create the SDL texture for displaying frames
    videoTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        codecContext->width,
        codecContext->height
    );

    return true;
}

// Render video frames
void renderVideoFrame(SDL_Renderer* renderer) {
    static bool isFrameAvailable = false;

    if (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, packet) >= 0) {
                while (avcodec_receive_frame(codecContext, frame) >= 0) {
                    // Convert the frame to RGBA format
                    uint8_t* data[4];
                    int linesize[4];
                    SDL_LockTexture(videoTexture, NULL, (void**)data, linesize);
                    sws_scale(
                        swsContext,
                        (const uint8_t* const*)frame->data, frame->linesize,
                        0, codecContext->height,
                        data, linesize
                    );
                    SDL_UnlockTexture(videoTexture);

                    isFrameAvailable = true; // Mark that a valid frame was rendered
                }
            }
        }
        av_packet_unref(packet);
    } else {
        // Restart the video in a loop
        av_seek_frame(formatContext, videoStreamIndex, 0, AVSEEK_FLAG_BACKWARD);
    }

    // Render the last valid frame if no new frame is available
    if (isFrameAvailable) {
        SDL_RenderCopy(renderer, videoTexture, NULL, NULL);
    }
}

// Setup the balls with initial positions and properties
void setup_balls() {
    srand(time(NULL));
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].x = rand() % window_width;   // Random x position within the window
        balls[i].y = rand() % window_height - window_height; // Start above the visible screen
        balls[i].width = 28;
        balls[i].height = 28;
        balls[i].vy = 50 + rand() % 100;     // Random falling speed between 50 and 150 pixels/second
        balls[i].alpha = 130 + rand() % 250; // Random transparency (130-255)
        ball_count++;
    }
}

// Update the balls' positions and reset them when they go off-screen
void update_balls() {
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();

    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    for (int i = 0; i < ball_count; i++) {
        // Update position with velocity
        balls[i].y += balls[i].vy * delta_time;

        // If the ball goes off the bottom of the screen, reset it to the top with a random x position
        if (balls[i].y > window_height) {
            balls[i].y = -balls[i].height;  // Reset to just above the screen
            balls[i].x = rand() % window_width; // Random x position
            balls[i].vy = 50 + rand() % 100;    // Randomize the falling speed again
            balls[i].alpha = 130 + rand() % 126; // Randomize transparency
        }
    }
}

void render_balls(SDL_Renderer* renderer) {
    for (int i = 0; i < ball_count; i++) {
        SDL_SetRenderDrawColor(renderer, 224, 217, 246, balls[i].alpha); // Set white color
        SDL_Rect ball_rect = {
            (int)balls[i].x,
            (int)balls[i].y,
            (int)balls[i].width,
            (int)balls[i].height
        };
        SDL_RenderFillRect(renderer, &ball_rect);
    }
}
// Define colors
SDL_Color white = {255, 255, 255, 255};
SDL_Color buttonColor = {70, 70, 70, 255};

// Button labels
const char* buttonLabels[] = { "Play", "Machine", "Player VS Machine", "Score", "Quit" };

// Fonts for main menu
TTF_Font* menuFont = NULL;
TTF_Font* buttonFont = NULL;

// Textures for buttons and title
SDL_Texture* buttonTextures[5];
SDL_Texture* titleTexture = NULL;

// Function to render button text into textures
SDL_Texture* createButtonTexture(TTF_Font* font, const char* text, SDL_Color color) {
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, text, color);
    if (!textSurface) {
        printf("Error creating button text surface: %s\n", TTF_GetError());
        return NULL;
    }
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return textTexture;
}

// Load the menu textures (called once when transitioning to the main menu)
void loadMenuTextures() {
    // Load fonts for title and buttons
    menuFont = TTF_OpenFont("assets/NType82.ttf", 72);
    buttonFont = TTF_OpenFont("assets/Ndot-55.ttf", 36);
    if (!menuFont || !buttonFont) {
        printf("Error loading fonts: %s\n", TTF_GetError());
        return;
    }

    // Create title texture
    SDL_Surface* menuTextSurface = TTF_RenderText_Blended(menuFont, "Main Menu", white);
    titleTexture = SDL_CreateTextureFromSurface(renderer, menuTextSurface);
    SDL_FreeSurface(menuTextSurface);

    // Create button textures
    for (int i = 0; i < 5; i++) {
        buttonTextures[i] = createButtonTexture(buttonFont, buttonLabels[i], white);
    }
}

// Render the main menu screen (called in each frame while in main menu)
void renderMainMenu() {
    // Clear screen with black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render "Main Menu" title
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    int textWidth = 0, textHeight = 0;
    SDL_QueryTexture(titleTexture, NULL, NULL, &textWidth, &textHeight);
    SDL_Rect titleRect = {
        (windowWidth - textWidth) / 2,
        20,
        textWidth,
        textHeight
    };
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Render buttons
    int buttonWidth = 300;
    int buttonHeight = 70;
    int buttonSpacing = 60;
    int startY = ((windowHeight - ((buttonHeight + buttonSpacing) * 5 - buttonSpacing)) / 2) + 50;

    SDL_GetMouseState(&mouseX, &mouseY); // Get mouse coordinates

    for (int i = 0; i < 5; i++) {
        SDL_Rect buttonRect = {
            (windowWidth - buttonWidth) / 2,
            startY + i * (buttonHeight + buttonSpacing),
            buttonWidth,
            buttonHeight
        };

        // Change the color when the mouse or keyboard "touches" the buttons
        bool isHovered = (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonWidth &&
                          mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonHeight);
        if (isHovered || i == selectedButton) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255); // Glow color
            if (isHovered) {
                selectedButton = i;
            }
        } else {
            SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        }

        SDL_RenderFillRect(renderer, &buttonRect);

        // Render button text
        SDL_QueryTexture(buttonTextures[i], NULL, NULL, &textWidth, &textHeight);
        SDL_Rect textRect = {
            buttonRect.x + (buttonWidth - textWidth) / 2,
            buttonRect.y + (buttonHeight - textHeight) / 2,
            textWidth,
            textHeight
        };
        SDL_RenderCopy(renderer, buttonTextures[i], NULL, &textRect);
    }
}

// Free resources when exiting the main menu
void cleanupMenuTextures() {
    TTF_CloseFont(menuFont);
    TTF_CloseFont(buttonFont);
    SDL_DestroyTexture(titleTexture);
    for (int i = 0; i < 5; i++) {
        SDL_DestroyTexture(buttonTextures[i]);
    }
}


int initialize_window(void) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return FALSE;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return FALSE;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return FALSE;
    }

    // Load and play the music
    music = Mix_LoadMUS("assets/watchdogs.mp3");
    if (!music) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        return FALSE;
    }

    if (Mix_PlayMusic(music, -1) == -1) { // -1 means loop indefinitely
        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
        return FALSE;
    }

    // Create SDL window in fullscreen mode
    window = SDL_CreateWindow(
        "Number Slide",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_FULLSCREEN
    );

    if (!window) {
        fprintf(stderr, "Error creating SDL Window: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return FALSE;
    }

    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return FALSE;
    }

    // Load the font with a large size for better quality
    largeFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 128);
    if (!largeFont) {
        printf("Error loading font: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return FALSE;
    }

    // Load the font with a small size for better quality
    smallFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 48);
    if (!smallFont) {
        printf("Error loading font: %s\n", TTF_GetError());
        TTF_CloseFont(largeFont);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return FALSE;
    }

    // Define text color
    SDL_Color color = {255, 255, 255, 255};

    // Create the texture for "NUMBER SLIDE" text
    SDL_Surface* textSurface = TTF_RenderText_Blended(largeFont, "NUMBER SLIDE", color);
    if (!textSurface) {
        printf("Error creating text surface: %s\n", TTF_GetError());
        TTF_CloseFont(largeFont);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return FALSE;
    }
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    // Create the texture for "appuyer une touche pour commencer" text
    SDL_Surface* startTextSurface = TTF_RenderText_Blended(smallFont, "appuyer une touche pour commencer", color);
    if (!startTextSurface) {
        printf("Error creating start text surface: %s\n", TTF_GetError());
        TTF_CloseFont(smallFont);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return FALSE;
    }
    startTextTexture = SDL_CreateTextureFromSurface(renderer, startTextSurface);
    SDL_FreeSurface(startTextSurface);

    // Initialize the balls
    setup_balls();

    return TRUE;
}


void render(Game* game) {
    // Enable blending
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);    // Clear the screen with a semi-transparent purple color
    renderVideoFrame(renderer);

    // Render the balls
    render_balls(renderer);    // Get window size for centering the text
    switch (currentState) {
        case WELCOME_PAGE: {
            int windowWidth, windowHeight;
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);

            // Get text texture size
            int textWidth = 0, textHeight = 0;
            SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

            // Define the destination rectangle to center the text
            SDL_Rect titleRect = {
                (windowWidth - textWidth) / 2,
                100,
                textWidth,
                textHeight
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &titleRect);

            // Render "appuyer une touche pour commencer" text near the bottom
            int startTextWidth = 0, startTextHeight = 0;
            SDL_QueryTexture(startTextTexture, NULL, NULL, &startTextWidth, &startTextHeight);
            int padding = 20;  // Space between the text and bottom edge
            SDL_Rect startTextRect = {
                (windowWidth - startTextWidth) / 2,
                windowHeight - startTextHeight - padding,
                startTextWidth,
                startTextHeight
            };
            SDL_RenderCopy(renderer, startTextTexture, NULL, &startTextRect);
            break;
        }
        case MAIN_MENU:
            renderMainMenu();
            break;
        case GAME_PAGE:
            renderGamePage(game);
            break;
        case MACHINE_PAGE:
            renderMachinePage(game);
            break;
        case PLAYERVSMACHINE_PAGE:
            renderPlayerVSMachine(game);
            break;
        case SCORE_PAGE:
            renderScorePage();
            break;
    }

    SDL_RenderPresent(renderer);
}


void renderGamePage(Game* game) {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    SDL_SetRenderDrawColor(renderer, 224, 217, 246, 255);
    SDL_RenderClear(renderer);

    // Update the game state
    update(game, 0.016);

    // Render the game grid
    render_thegrid(renderer, window_width, window_height, game, false, 0);
    rendtimer(game);

    // Check if the player has lost or won
    if (has_lost(game)) {
        SDL_Color bgColor = {255, 0, 0, 200}; // Red background, 200 alpha for semi-transparency
        render_splash_screen(renderer,window_width, window_height, "You Have Lost", bgColor, false, 0);
        game->state = GS_LOST;
        printf("Game lost detected\n");
    } else if (has_won(game)) { // Optionally handle a "win" state
        SDL_Color bgColor = {0, 255, 0, 200}; // Green background, 200 alpha for semi-transparency
        render_splash_screen(renderer,window_width,window_height, "You Have Won", bgColor,true,0);
        game->state = GS_WON;
        printf("Game won detected\n");
    }

    SDL_Event event;
    // Check if the game is won or lost
    if (game->state == GS_WON || game->state == GS_LOST) {
        // Poll for events to detect a space bar press
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) { // Detect space bar
                printf("Space bar is detected\n");

                // Reset the game state to allow the name input prompt
                game->state = GS_PLAYING;

               if (highScoreBoard.count < MAX_HIGH_SCORES) {
                 char playerName[MAX_NAME_LENGTH];
					       ask_for_player_name(game,playerName);
                 add_high_score(playerName, game, highScoreBoard.count, "score.txt");
                highScoreBoard.count++;
              } else {
                for (int i = 0; i < MAX_HIGH_SCORES; i++) {
                  if (game->score > highScoreBoard.highScores[i].score) {
                   char playerName[MAX_NAME_LENGTH];
							    ask_for_player_name(game, playerName);
							    replace_high_score(playerName, game, i, "score.txt");
                  break;
        }
    }
}                               // Transition to SCORE_PAGE after handling high score
                currentState = SCORE_PAGE;
                printf("Transitioned to SCORE_PAGE\n");
                break; // Exit event polling after handling
            }
        }
    }
}void renderMachinePage(Game* game) {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
SDL_SetRenderDrawColor(renderer, 88, 106, 226, 255);
SDL_RenderClear(renderer);
    // Update the game state
    Machine(game, 0.016);

    // Render the game grid
    render_thegrid(renderer, window_width, window_height, game, true, 0);
if (game->last_directions != DIR_NONE) {
        render_movement_arrows(renderer, window_width, window_height, game->last_directions);
    } 
    // Check if the player has lost and render the splash screen if necessary
    if (has_lost(game)) {
        SDL_Color bgColor = {255, 0, 0, 200}; // Red background, 200 alpha for semi-transparency
        render_splash_screen(renderer,window_width, window_height, "Lost", bgColor, false,0);
        game->state = GS_LOST;
    } else if (has_won(game)) { // Optionally handle a "win" state
        SDL_Color bgColor = {0, 255, 0, 200}; // Green background, 200 alpha for semi-transparency
        render_splash_screen(renderer,window_width, window_height, "Won", bgColor, false,0);
        game->state = GS_WON;
    }
}


void renderPlayerVSMachine(Game* game) {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    // Update the game state
    update(game, 0.016);
    int player_grid_offset_x = 0;

    // Render the game grid
    render_thegrid(renderer, window_width / 2, window_height, game, false, player_grid_offset_x);

    // Check if the player has lost and render the splash screen if necessary
    if (has_lost(game)) {
        SDL_Color bgColor = {255, 0, 0, 200}; // Red background, 200 alpha for semi-transparency
        render_splash_screen(renderer, window_width, window_height, "You Have Lost", bgColor, true, 0);
        game->state = GS_LOST;
    } else if (has_won(game)) { // Optionally handle a "win" state
        SDL_Color bgColor = {0, 255, 0, 200}; // Green background, 200 alpha for semi-transparency
        render_splash_screen(renderer, window_width, window_height, "You Have Won", bgColor, true, window_width / 2);
        game->state = GS_WON;
    }

    int ai_grid_offset_x = window_width / 2;
    render_thegrid(renderer, window_width / 2, window_height, game, true, ai_grid_offset_x);

    SDL_Event event;
    // Check if the game is won or lost
    if (game->state == GS_WON || game->state == GS_LOST) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                printf("Space bar is detected\n");

                if (highScoreBoard.count < MAX_HIGH_SCORES) {
                 char playerName[MAX_NAME_LENGTH];
					       ask_for_player_name(game,playerName);
                 add_high_score(playerName, game, highScoreBoard.count, "score.txt");
                highScoreBoard.count++;
              } else {
                for (int i = 0; i < MAX_HIGH_SCORES; i++) {
                  if (game->score > highScoreBoard.highScores[i].score) {
                   char playerName[MAX_NAME_LENGTH];
							    ask_for_player_name(game, playerName);
							    replace_high_score(playerName, game, i, "score.txt");
                  break;
        }
    }
}                game->state = GS_PLAYING;
                currentState = SCORE_PAGE;
                break; // Exit event polling after handling
            }
        }
    }
}




void renderScorePage() {
    // Ensure renderer and window are valid
    if (!renderer) {
        printf("Error: Renderer is NULL\n");
        return;
    }

    if (!window) {
        printf("Error: Window is NULL\n");
        return;
    }

    // Load high scores from the file
    load_high_scores("score.txt");

    if (highScoreBoard.count < 0 || highScoreBoard.count > MAX_HIGH_SCORES) {
        printf("Error: Invalid highScoreBoard.count (%d)\n", highScoreBoard.count);
        return;
    }

    // Load fonts
    TTF_Font* scoreFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 72);
    if (!scoreFont) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }

    TTF_Font* scoreListFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 72);
    if (!scoreListFont) {
        printf("Error loading font for score list: %s\n", TTF_GetError());
        TTF_CloseFont(scoreFont);
        return;
    }

    // Clear screen with background color
    SDL_SetRenderDrawColor(renderer, 253, 29, 117, 255); // Bright pink
    SDL_RenderClear(renderer);

    // Render "SCORE" title
    SDL_Surface* titleSurface = TTF_RenderText_Blended(scoreFont, "SCORE", white);
    if (!titleSurface) {
        printf("Error creating title surface: %s\n", TTF_GetError());
        TTF_CloseFont(scoreFont);
        TTF_CloseFont(scoreListFont);
        return;
    }

    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_FreeSurface(titleSurface);
    if (!titleTexture) {
        printf("Error creating title texture: %s\n", SDL_GetError());
        TTF_CloseFont(scoreFont);
        TTF_CloseFont(scoreListFont);
        return;
    }

    // Center the title
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    int titleWidth, titleHeight;
    SDL_QueryTexture(titleTexture, NULL, NULL, &titleWidth, &titleHeight);

    SDL_Rect titleRect = {
        (windowWidth - titleWidth) / 2, // Center horizontally
        20,                            // Top margin
        titleWidth,
        titleHeight
    };

    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_DestroyTexture(titleTexture); // Free texture after rendering

    // Render high scores
    int yOffset = titleRect.y + titleRect.h + 40; // Start below the title
    for (int i = 0; i < highScoreBoard.count; i++) {
        char scoreText[128];
        snprintf(
            scoreText, sizeof(scoreText), 
            "%d. %s - %d points - %02d:%02d", 
            i + 1, 
            highScoreBoard.highScores[i].name, 
            highScoreBoard.highScores[i].score,
            highScoreBoard.highScores[i].time.mint, 
            highScoreBoard.highScores[i].time.scnd
        );

        SDL_Surface* scoreSurface = TTF_RenderUTF8_Blended(scoreListFont, scoreText, white);
        if (!scoreSurface) {
            printf("Error creating surface for high score %d: %s\n", i, TTF_GetError());
            continue;
        }

        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        SDL_FreeSurface(scoreSurface);
        if (!scoreTexture) {
            printf("Error creating texture for high score %d: %s\n", i, SDL_GetError());
            continue;
        }

        SDL_Rect scoreRect = {
            50,          // Left margin
            yOffset,     // Vertical position
            scoreSurface->w,
            scoreSurface->h
        };

        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        SDL_DestroyTexture(scoreTexture); // Free texture after rendering

        yOffset += 150; // Space between each score
    }

    // Close fonts
    TTF_CloseFont(scoreFont);
    TTF_CloseFont(scoreListFont);

    printf("Rendered Score Page\n");
}

void timertext(SDL_Renderer* renderer,Game* game) {

  SDL_Color white = {42, 35, 86, 255};
 TTF_Font* font = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 48);
    char text[100];
    snprintf(text, sizeof(text), "%.2d:%.2d", game->time.mint,game->time.scnd);
    SDL_Surface* surfacet = TTF_RenderText_Blended(font, text, white);
    SDL_Texture* texturet = SDL_CreateTextureFromSurface(renderer, surfacet);

    SDL_Rect dest = {100, 100, surfacet->w, surfacet->h};
    SDL_RenderCopy(renderer, texturet, NULL, &dest);

    SDL_DestroyTexture(texturet);
    SDL_FreeSurface(surfacet);

 TTF_CloseFont(font);

}
