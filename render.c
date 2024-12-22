#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "include/render.h"
#include "include/cgame.h"
#include "include/input.h"

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

    int textWidth, textHeight;
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

    return TRUE;
}

void render(Game* game) {
    // Clear the screen with a purple color
    SDL_SetRenderDrawColor(renderer, 164, 38, 232, 255);
    SDL_RenderClear(renderer);

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
            renderMachinePage();
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

char* render_name_input(char* name) {
    // Load the font
    TTF_Font* font = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 72);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        return NULL;
    }

    SDL_Color bg_color = {0, 0, 0, 200};  // Semi-transparent black for input box
    SDL_Color text_color = {255, 255, 255, 255};

    SDL_Rect input_box = {200, 150, 400, 100};  // Input box size and position
    char user_input[MAX_NAME_LEN + 1] = "";     // Input buffer
    int input_len = 0;
    bool input_active = true;

    SDL_StartTextInput();  // Start text input mode

    // Main event loop
    SDL_Event event;
    while (input_active) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                input_active = false;
                break;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {  // Confirm input
                    input_active = false;
                    strncpy(name, user_input, MAX_NAME_LEN);
                    name[MAX_NAME_LEN] = '\0';  // Null-terminate
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && input_len > 0) {
                    user_input[--input_len] = '\0';  // Remove last character
                }
            } else if (event.type == SDL_TEXTINPUT) {
                if (input_len < MAX_NAME_LEN) {
                    strcat(user_input, event.text.text);  // Append input
                    input_len += strlen(event.text.text);
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the input box
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderFillRect(renderer, &input_box);

        // Render the user input text
        SDL_Surface* text_surface = TTF_RenderText_Solid(font, user_input, text_color);
        if (text_surface) {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            SDL_Rect text_rect = {input_box.x + 10, input_box.y + 25, text_surface->w, text_surface->h};
            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

            SDL_DestroyTexture(text_texture);
            SDL_FreeSurface(text_surface);
        }

        // Update the screen
        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();  // Stop text input mode
    TTF_CloseFont(font);  // Clean up the font
    return name;
}

void renderGamePage(Game* game) {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    update(game, 0.016); // Update the game state
    render_thegrid(renderer, window_width, window_height, game);
}

void renderMachinePage() {}

void renderPlayerVSMachine(Game* game) {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    render_thegrid(renderer, window_width, window_height, game);
}

void renderScorePage() {
    // Load the font for the score title
    TTF_Font* scoreFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 72);
    if (!scoreFont) {
        printf("Error loading fonts: %s\n", TTF_GetError());
        return;
    }

    // Create the surface for the score title
    SDL_Surface* scoreTextSurface = TTF_RenderText_Blended(scoreFont, "SCORE", white);
    if (!scoreTextSurface) {
        printf("Error creating score text surface: %s\n", TTF_GetError());
        TTF_CloseFont(scoreFont);
        return;
    }

    // Create the texture from the surface
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreTextSurface);
    SDL_FreeSurface(scoreTextSurface);  // Free the surface after creating the texture
    if (!scoreTexture) {
        printf("Error creating score texture: %s\n", SDL_GetError());
        TTF_CloseFont(scoreFont);
        return;
    }

    // Clear the screen with a background color
    SDL_SetRenderDrawColor(renderer, 107, 107, 223, 255);
    SDL_RenderClear(renderer);

    // Get window dimensions for centering
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // Get the texture's dimensions
    int textWidth, textHeight;
    SDL_QueryTexture(scoreTexture, NULL, NULL, &textWidth, &textHeight);

    // Define the rectangle to render the score title
    SDL_Rect titleRect = {
        (windowWidth - textWidth) / 2,  // Center horizontally
        20,                            // Position near the top
        textWidth,
        textHeight
    };

    // Render the score title
    SDL_RenderCopy(renderer, scoreTexture, NULL, &titleRect);

    // Clean up resources
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(scoreFont);
}


