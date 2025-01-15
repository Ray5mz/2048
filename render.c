#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "include/render.h"
#include "include/game.h"
#include "include/input.h"
#include <time.h>

#define FPS 120
#define FRAME_TARGET_TIME (1000 / FPS) // How many frames in milliseconds

#define MAX_BALLS 30

struct ball {
    float x, y;
    float width, height;
    float vx, vy; // Velocity components
    Uint8 alpha; // Alpha value for transparency
    float ax, ay;
};

int last_frame_time = 0;
struct ball balls[MAX_BALLS];
int ball_count = 0;

void setup_balls() {
    srand(time(NULL)); // Initialize random number generator (do this once in your program, not every time)
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    // Define game boundaries or range for random values
    int max_x = window_width; // Replace with your game width
    int max_y = 50; // Replace with your game height

    // Generate random positions and velocities for each ball
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].x = rand() % max_x;
        balls[i].y = rand() % max_y;
        balls[i].width = 28;
        balls[i].height = 28;
        //balls[i].vx = (rand() % 200 - 100) * 0.1; // Random velocity in x direction
        balls[i].vy = (rand() % 200 - 100) * 0.1; // Random velocity in y direction
        balls[i].ax = 0; // No acceleration in x direction
        balls[i].ay = 0.05; // Constant acceleration in y direction (gravity)
        balls[i].alpha = 130; // Set initial alpha value for transparency
        ball_count++;
    }
}

void update_balls() {
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f; // delta time is used to think in pixels per second and not pixels per frame
    last_frame_time = SDL_GetTicks(); // this function let us know how many milliseconds have passed since the start of our SDL initialization in SDL_INIT
    int time_to_sleep = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

    if (time_to_sleep > 0 && time_to_sleep <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_sleep); // call only if we are too fast to process this frame, it would let the processor do what it has to do and call for its interruption to resume the execution
    }

    for (int i = 0; i < ball_count; i++) {
        // Update velocity with acceleration
        //balls[i].vx += balls[i].ax * delta_time;
        //balls[i].vy += balls[i].ay * delta_time;

        // Update position with velocity
        //balls[i].x += 70 * delta_time;
        balls[i].y += 30 * delta_time;

        // Ensure the ball stays within the window bounds
        int window_width, window_height;
        SDL_GetWindowSize(window, &window_width, &window_height);
        //if (balls[i].x < 0) balls[i].x = 0;
        //if (balls[i].x > window_width - balls[i].width) balls[i].x = window_width - balls[i].width;
        if (balls[i].y < 0) balls[i].y = 0;
        if (balls[i].y > window_height - balls[i].height) balls[i].y = window_height - balls[i].height;

        // Apply damping to velocity to smooth the movement
        //balls[i].vx *= 0.99;
        balls[i].vy *= 0.99;
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
    menuFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 72);
    buttonFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 36);
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
        200,
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

    // Clear the screen with a semi-transparent purple color
    SDL_SetRenderDrawColor(renderer, 164, 38, 232, 128);
    SDL_RenderClear(renderer);

    // Render the balls
    for (int i = 0; i < ball_count; i++) {
        SDL_Rect ball_rect = {
            (int)balls[i].x,
            (int)balls[i].y,
            (int)balls[i].width,
            (int)balls[i].height
        };
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, balls[i].alpha);
        SDL_RenderFillRect(renderer, &ball_rect);
    }

    // Get window size for centering the text
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

    // Update the game state
    update(game, 0.016);

    // Render the game grid
    render_thegrid(renderer, window_width, window_height, game, false,0);

    // Check if the player has lost and render the splash screen if necessary
    if (has_lost(game)) {
        SDL_Color bgColor = {255, 0, 0, 200}; // Red background, 200 alpha for semi-transparency
        render_splash_screen(renderer, "You Have Lost", bgColor);
        game->state = GS_LOST;
    } else if (has_won(game)) { // Optionally handle a "win" state
        SDL_Color bgColor = {0, 255, 0, 200}; // Green background, 200 alpha for semi-transparency
        render_splash_screen(renderer, "You Have Won", bgColor);
        game->state = GS_WON;
    }
SDL_Event event;
    // Check if the game is won or lost
    if (game->state == GS_WON || game->state == GS_LOST) {
        // Poll for events to detect a space bar press
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) { // Detect space bar
				      printf("so yeah basically the space ar is detected \n");
                if (highScoreBoard.count < MAX_HIGH_SCORES || 
                    game->score > highScoreBoard.highScores[MAX_HIGH_SCORES - 1].score) {
                    // Call ask_for_player_name when space bar is pressed
                    char playerName[MAX_NAME_LENGTH];
                    ask_for_player_name(game, playerName);
                    add_high_score(playerName, game->score);
                }

                // Transition to SCORE_PAGE after handling high score
                currentState = SCORE_PAGE;
                break; // Exit event polling after handling
            }
        }
    }
}

void renderMachinePage(Game* game) {
  
 int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    // Update the game state
    Machine(game, 0.016);

    // Render the game grid
    render_thegrid(renderer, window_width, window_height, game, true,0);

    // Check if the player has lost and render the splash screen if necessary
    if (has_lost(game)) {
        SDL_Color bgColor = {255, 0, 0, 200}; // Red background, 200 alpha for semi-transparency
        render_splash_screen(renderer, "Lost", bgColor);
        game->state = GS_LOST;
    } else if (has_won(game)) { // Optionally handle a "win" state
        SDL_Color bgColor = {0, 255, 0, 200}; // Green background, 200 alpha for semi-transparency
        render_splash_screen(renderer, "Won", bgColor);
        game->state = GS_WON;
    }
              
}


void renderPlayerVSMachine(Game* game) {
   
 int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    // Update the game state
    update(game, 0.016);
int player_grid_offset_x=0;
    // Render the game grid
    render_thegrid(renderer, window_width/2, window_height, game, false, player_grid_offset_x);

    // Check if the player has lost and render the splash screen if necessary
    if (has_lost(game)) {
        SDL_Color bgColor = {255, 0, 0, 200}; // Red background, 200 alpha for semi-transparency
        render_splash_screen(renderer, "You Have Lost", bgColor);
        game->state = GS_LOST;
    } else if (has_won(game)) { // Optionally handle a "win" state
        SDL_Color bgColor = {0, 255, 0, 200}; // Green background, 200 alpha for semi-transparency
        render_splash_screen(renderer, "You Have Won", bgColor);
        game->state = GS_WON;
    }
int ai_grid_offset_x=window_width/2;
	render_thegrid(renderer, window_width/2, window_height, game, true, ai_grid_offset_x);

SDL_Event event;
    // Check if the game is won or lost
    if (game->state == GS_WON || game->state == GS_LOST) {
        // Poll for events to detect a space bar press
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) { // Detect space bar
				      printf("so yeah basically the space ar is detected \n");
                if (highScoreBoard.count < MAX_HIGH_SCORES || 
                    game->score > highScoreBoard.highScores[MAX_HIGH_SCORES - 1].score) {
                    // Call ask_for_player_name when space bar is pressed
                    char playerName[MAX_NAME_LENGTH];
                    ask_for_player_name(game, playerName);
                    add_high_score(playerName, game->score);
                }

                // Transition to SCORE_PAGE after handling high score
                currentState = SCORE_PAGE;
                break; // Exit event polling after handling
            }
        }
    }
}
void renderScorePage() {
    // Check renderer 	  PlayerVSMachine(game, 0.nd window
    if (!renderer) {
        printf("Error: Renderer is NULL\n");
        return;
    }

    if (!window) {
        printf("Error: Window is NULL\n");
        return;
    }

    if (highScoreBoard.count < 0 || highScoreBoard.count > MAX_HIGH_SCORES) {
        printf("Error: Invalid highScoreBoard.count (%d)\n", highScoreBoard.count);
        return;
    }

    // Load the font for the score title
    TTF_Font* scoreFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 72);
    if (!scoreFont) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }

    // Render the title "SCORE"
    SDL_Surface* scoreTextSurface = TTF_RenderText_Blended(scoreFont, "SCORE", white);
    if (!scoreTextSurface) {
        printf("Error creating score text surface: %s\n", TTF_GetError());
        TTF_CloseFont(scoreFont);
        return;
    }

    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreTextSurface);
    SDL_FreeSurface(scoreTextSurface); // Free the surface after creating the texture
    if (!scoreTexture) {
        printf("Error creating score texture: %s\n", TTF_GetError());
        TTF_CloseFont(scoreFont);
        return;
    }

    // Clear the screen with a background color
    SDL_SetRenderDrawColor(renderer, 107, 107, 223, 255); // Your specified color
    SDL_RenderClear(renderer);

    // Get window dimensions for centering
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Get the dimensions of the score title texture
    int textWidth, textHeight;
    SDL_QueryTexture(scoreTexture, NULL, NULL, &textWidth, &textHeight);

    // Define the rectangle for rendering the score title
    SDL_Rect titleRect = {
        (windowWidth - textWidth) / 2,  // Center horizontally
        20,                            // Position near the top
        textWidth,
        textHeight
    };

    // Render the score title
    SDL_RenderCopy(renderer, scoreTexture, NULL, &titleRect);

    // Free the title texture
    SDL_DestroyTexture(scoreTexture);

    // Render the high scores
    int yOffset = 150;
    for (int i = 0; i < highScoreBoard.count; i++) {
        char scoreText[64];
        snprintf(scoreText, sizeof(scoreText), "%d. %s - %d", i + 1, highScoreBoard.highScores[i].name, highScoreBoard.highScores[i].score);
        printf("HighScore %d: %s - %d\n", i, highScoreBoard.highScores[i].name, highScoreBoard.highScores[i].score);

        SDL_Surface* scoreSurface = TTF_RenderUTF8_Blended(scoreFont, scoreText, white);
        if (!scoreSurface) {
            printf("Error creating score surface for high score %d: %s\n", i, TTF_GetError());
            continue;
        }

        SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        if (!scoreTexture) {
            printf("Error creating score texture for high score %d: %s\n", i, TTF_GetError());
            SDL_FreeSurface(scoreSurface);
            continue;
        }

        SDL_Rect scoreRect = {100, yOffset, scoreSurface->w, scoreSurface->h};
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);

        SDL_FreeSurface(scoreSurface);
        SDL_DestroyTexture(scoreTexture);

        yOffset += 50; // Space between scores
    }

    // Clean up font
    TTF_CloseFont(scoreFont);

    printf("Rendering Score Page\n");
}
