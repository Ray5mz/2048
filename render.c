#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include "include/render.h"
#include "include/cgame.h"
#include "include/input.h"
#include <stdio.h>
#include <time.h>
#include <SDL2/SDL_image.h>

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
    int arrowSize = 50;
    int spacing = 20;
    int thickness = 3;
    SDL_Color arrowColor = {255, 255, 255, 255};
    int startX = window_width - arrowSize - spacing;
    int startY = spacing;
    SDL_SetRenderDrawColor(renderer, arrowColor.r, arrowColor.g, arrowColor.b, arrowColor.a);

    switch (direction) {
        case DIR_UP:
            for (int i = 0; i < thickness; i++) {
                SDL_RenderDrawLine(renderer, startX + i, startY, startX - arrowSize / 2 + i, startY + arrowSize);
                SDL_RenderDrawLine(renderer, startX + i, startY, startX + arrowSize / 2 + i, startY + arrowSize);
            }
            break;
        case DIR_DOWN:
            for (int i = 0; i < thickness; i++) {
                SDL_RenderDrawLine(renderer, startX + i, startY + arrowSize, startX - arrowSize / 2 + i, startY);
                SDL_RenderDrawLine(renderer, startX + i, startY + arrowSize, startX + arrowSize / 2 + i, startY);
            }
            break;
        case DIR_LEFT:
            for (int i = 0; i < thickness; i++) {
                SDL_RenderDrawLine(renderer, startX - arrowSize + i, startY + arrowSize / 2, startX + i, startY);
                SDL_RenderDrawLine(renderer, startX - arrowSize + i, startY + arrowSize / 2, startX + i, startY + arrowSize);
            }
            break;
        case DIR_RIGHT:
            for (int i = 0; i < thickness; i++) {
                SDL_RenderDrawLine(renderer, startX + i, startY + arrowSize / 2, startX - arrowSize + i, startY);
                SDL_RenderDrawLine(renderer, startX + i, startY + arrowSize / 2, startX - arrowSize + i, startY + arrowSize);
            }
            break;
        default:
            break;
    }
}

SDL_Texture* load_background_image(SDL_Renderer* renderer, const char* filepath) {
    SDL_Surface* surface = IMG_Load(filepath);
    if (!surface) {
        printf("Error loading background image: %s\n", IMG_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Error creating texture from surface: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }
    SDL_FreeSurface(surface);
    return texture;
}

void render_background(SDL_Renderer* renderer, SDL_Texture* background_texture, int window_width, int window_height) {
    if (!background_texture) {
        return;
    }
    SDL_Rect dest_rect = {0, 0, window_width, window_height};
    SDL_RenderCopy(renderer, background_texture, NULL, &dest_rect);
}

void setup_balls() {
    srand(time(NULL));
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].x = rand() % window_width;
        balls[i].y = rand() % window_height - window_height;
        balls[i].width = 28;
        balls[i].height = 28;
        balls[i].vy = 50 + rand() % 100;
        balls[i].alpha = 130 + rand() % 250;
        ball_count++;
    }
}

void update_balls() {
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();

    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    for (int i = 0; i < ball_count; i++) {
        balls[i].y += balls[i].vy * delta_time;
        if (balls[i].y > window_height) {
            balls[i].y = -balls[i].height;
            balls[i].x = rand() % window_width;
            balls[i].vy = 50 + rand() % 100;
            balls[i].alpha = 130 + rand() % 126;
        }
    }
}

void render_balls(SDL_Renderer* renderer) {
    for (int i = 0; i < ball_count; i++) {
        SDL_SetRenderDrawColor(renderer, 224, 217, 246, balls[i].alpha);
        SDL_Rect ball_rect = {(int)balls[i].x, (int)balls[i].y, (int)balls[i].width, (int)balls[i].height};
        SDL_RenderFillRect(renderer, &ball_rect);
    }
}

void render_pause_screen(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderFillRect(renderer, NULL);

    int SCREEN_WIDTH, SCREEN_HEIGHT;
    SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    TTF_Font* font = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 64);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return;
    }

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, "PAUSE", textColor);
    if (!textSurface) {
        SDL_Log("Failed to render text surface: %s", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {
        (SCREEN_WIDTH - textSurface->w) / 2,
        (SCREEN_HEIGHT - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

bool render_restart_dialog(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color boxColor = {0, 0, 0, 200};
    SDL_Color buttonColor = {50, 50, 200, 255};
    SDL_Color hoverColor = {100, 100, 255, 255};

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    SDL_Rect dialogBox = {
        (windowWidth - 400) / 2,
        (windowHeight - 200) / 2,
        400,
        200
    };
    SDL_Rect yesButton = {
        dialogBox.x + 30,
        dialogBox.y + 100,
        150,
        50
    };
    SDL_Rect noButton = {
        dialogBox.x + 220,
        dialogBox.y + 100,
        150,
        50
    };

    bool running = true;
    bool choice = false;
    SDL_Event event;

    while (running) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
        SDL_RenderFillRect(renderer, NULL);

        SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
        SDL_RenderFillRect(renderer, &dialogBox);

        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &yesButton);
        SDL_RenderFillRect(renderer, &noButton);

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &yesButton)) {
            SDL_SetRenderDrawColor(renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
            SDL_RenderFillRect(renderer, &yesButton);
        }
        if (SDL_PointInRect(&(SDL_Point){mouseX, mouseY}, &noButton)) {
            SDL_SetRenderDrawColor(renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
            SDL_RenderFillRect(renderer, &noButton);
        }

        SDL_Surface* yesSurface = TTF_RenderUTF8_Blended(font, "YES", textColor);
        SDL_Surface* noSurface = TTF_RenderUTF8_Blended(font, "NO", textColor);

        SDL_Texture* yesTexture = SDL_CreateTextureFromSurface(renderer, yesSurface);
        SDL_Texture* noTexture = SDL_CreateTextureFromSurface(renderer, noSurface);

        SDL_Rect yesTextRect = {
            yesButton.x + (yesButton.w - yesSurface->w) / 2,
            yesButton.y + (yesButton.h - yesSurface->h) / 2,
            yesSurface->w,
            yesSurface->h
        };
        SDL_Rect noTextRect = {
            noButton.x + (noButton.w - noSurface->w) / 2,
            noButton.y + (noButton.h - noSurface->h) / 2,
            noSurface->w,
            noSurface->h
        };

        SDL_RenderCopy(renderer, yesTexture, NULL, &yesTextRect);
        SDL_RenderCopy(renderer, noTexture, NULL, &noTextRect);

        SDL_DestroyTexture(yesTexture);
        SDL_DestroyTexture(noTexture);
        SDL_FreeSurface(yesSurface);
        SDL_FreeSurface(noSurface);

        SDL_Surface* messageSurface = TTF_RenderUTF8_Blended(font, "Do you want to resume?", textColor);
        SDL_Texture* messageTexture = SDL_CreateTextureFromSurface(renderer, messageSurface);
        SDL_Rect messageRect = {
            dialogBox.x + (dialogBox.w - messageSurface->w) / 2,
            dialogBox.y + 40,
            messageSurface->w,
            messageSurface->h
        };
        SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);

        SDL_DestroyTexture(messageTexture);
        SDL_FreeSurface(messageSurface);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &yesButton)) {
                        choice = true;
                        running = false;
                    } else if (SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &noButton)) {
                        choice = false;
                        running = false;
                    }
                }
            }
        }
    }

    return choice;
}

SDL_Color white = {255, 255, 255, 255};
SDL_Color buttonColor = {70, 70, 70, 255};
const char* buttonLabels[] = { "Play", "Machine", "Player VS Machine", "Score", "Quit" };
TTF_Font* menuFont = NULL;
TTF_Font* buttonFont = NULL;
SDL_Texture* buttonTextures[5];
SDL_Texture* titleTexture = NULL;

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

void loadMenuTextures() {
    menuFont = TTF_OpenFont("assets/NType82.ttf", 72);
    buttonFont = TTF_OpenFont("assets/Ndot-55.ttf", 36);
    if (!menuFont || !buttonFont) {
        printf("Error loading fonts: %s\n", TTF_GetError());
        return;
    }

    SDL_Surface* menuTextSurface = TTF_RenderText_Blended(menuFont, "Main Menu", white);
    titleTexture = SDL_CreateTextureFromSurface(renderer, menuTextSurface);
    SDL_FreeSurface(menuTextSurface);

    for (int i = 0; i < 5; i++) {
        buttonTextures[i] = createButtonTexture(buttonFont, buttonLabels[i], white);
    }
}

void renderMainMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

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

    int buttonWidth = 300;
    int buttonHeight = 70;
    int buttonSpacing = 60;
    int startY = ((windowHeight - ((buttonHeight + buttonSpacing) * 5 - buttonSpacing)) / 2) + 50;

    SDL_GetMouseState(&mouseX, &mouseY);

    for (int i = 0; i < 5; i++) {
        SDL_Rect buttonRect = {
            (windowWidth - buttonWidth) / 2,
            startY + i * (buttonHeight + buttonSpacing),
            buttonWidth,
            buttonHeight
        };

        bool isHovered = (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonWidth &&
                          mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonHeight);
        if (isHovered || i == selectedButton) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
            if (isHovered) {
                selectedButton = i;
            }
        } else {
            SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        }

        SDL_RenderFillRect(renderer, &buttonRect);

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

void cleanupMenuTextures() {
    TTF_CloseFont(menuFont);
    TTF_CloseFont(buttonFont);
    SDL_DestroyTexture(titleTexture);
    for (int i = 0; i < 5; i++) {
        SDL_DestroyTexture(buttonTextures[i]);
    }
}

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return FALSE;
    }

    if (TTF_Init() == -1) {
        printf("Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return FALSE;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return FALSE;
    }

    music = Mix_LoadMUS("assets/Undertale.mp3");
    if (!music) {
        printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
        return FALSE;
    }

    if (Mix_PlayMusic(music, -1) == -1) {
        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
        return FALSE;
    }

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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return FALSE;
    }

    largeFont = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 128);
    if (!largeFont) {
        printf("Error loading font: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return FALSE;
    }

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

    SDL_Color color = {255, 255, 255, 255};

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

    setup_balls();

    return TRUE;
}

void load_game_state(Game* game) {
    FILE* file = fopen("save.txt", "r");
    if (file != NULL) {
        printf("Loading game state from save.txt...\n");

        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (fscanf(file, "%d", &game->board[i*GRID_SIZE+j]) != 1) {
                    printf("Error: Failed to read data from save.txt at (%d, %d).\n", i, j);
                    fclose(file);
                    return;
                }
                printf("Loaded value at (%d, %d): %d\n", i, j, game->board[i*GRID_SIZE+j]);
            }
        }
        if (fscanf(file, "%d, %d, %d", &game->score, &game->time.mint, &game->time.scnd) != 3) {
            printf("Error: Failed to read time data from save.txt.\n");
            fclose(file);
            return;
        }

        fclose(file);
        printf("Game state loaded successfully.\n");
    } else {
        printf("File is empty or does not exist.\n");
    }
}

void render(Game* game, SDL_Texture* background_texture) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (background_texture) {
        render_background(renderer, background_texture, windowWidth, windowHeight);
    }

    render_balls(renderer);

    static bool restartDialogShown = false;

    switch (currentState) {
        case WELCOME_PAGE: {
            int textWidth = 0, textHeight = 0;
            SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

            SDL_Rect titleRect = {
                (windowWidth - textWidth) / 2,
                100,
                textWidth,
                textHeight
            };
            SDL_RenderCopy(renderer, textTexture, NULL, &titleRect);

            int startTextWidth = 0, startTextHeight = 0;
            SDL_QueryTexture(startTextTexture, NULL, NULL, &startTextWidth, &startTextHeight);
            int padding = 20;
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
        case GAME_PAGE: {
            if (!restartDialogShown) {
                FILE* file = fopen("save.txt", "r");
                if (file == NULL) {
                    printf("Error: Could not open file '%s'\n", "save.txt");
                    return;
                }

                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                rewind(file);

                TTF_Font* gameFont = TTF_OpenFont("assets/Ndot-55.ttf", 72);
                bool resume = (file_size != 0) ? render_restart_dialog(renderer, gameFont) : false;
                fclose(file);

                if (resume) {

					 
                    load_game_state(game);
                } else if(!resume){

				           game->time.mint=0;
					         game->time.scnd=0;
				          }

                restartDialogShown = true;
            }

            renderGamePage(game);
            break;
        }
        case MACHINE_PAGE:
            renderMachinePage(game);
            break;
        case PLAYERVSMACHINE_PAGE:
            renderPlayerVSMachine(game);
            break;
        case SCORE_PAGE:
            renderScorePage();
            break;
        case GAME_PAUSED:
            renderGamePage(game);
            render_pause_screen(renderer);
            break;
    }

    SDL_RenderPresent(renderer);
}

void render_esc(SDL_Renderer* renderer, GameState currentState) {
    TTF_Font* font = TTF_OpenFont("assets/Ndot-55.ttf", 32);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return;
    }

    SDL_Color textColor;
    if (currentState == GAME_PAGE) {
        textColor = (SDL_Color){0, 0, 0, 255};
    } else {
        textColor = (SDL_Color){255, 255, 255, 255};
    }

    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, "ESC", textColor);
    if (!textSurface) {
        SDL_Log("Failed to render text surface: %s", TTF_GetError());
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_Log("Failed to create text texture: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        return;
    }

    int padding_x = 65;
    int padding_y = 35;
    SDL_Rect textRect = {
        padding_x,
        padding_y,
        textSurface->w,
        textSurface->h
    };

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

void renderGamePage(Game* game) {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    SDL_SetRenderDrawColor(renderer, 224, 217, 246, 255);
    SDL_RenderClear(renderer);

    update(game, 0.016);
    render_thegrid(renderer, window_width, window_height, game, false, 0);
    rendtimer(game);
    render_movement_arrows(renderer, window_width / 10, window_height, DIR_LEFT);
    render_esc(renderer, currentState);

    if (has_lost(game)) {
        SDL_Color bgColor = {255, 0, 0, 200};
        render_splash_screen(renderer, window_width, window_height, "You Have Lost", bgColor, false, 0);
        game->state = GS_LOST;

        FILE* file = fopen("save.txt", "w");
        if (!file) {
            perror("Failed to open the file");
            return;
        }
        fclose(file);
        printf("File %s has been cleared.\n", "save.txt");

        printf("Game lost detected\n");
    } else if (has_won(game)) {
        SDL_Color bgColor = {0, 255, 0, 200};
        render_splash_screen(renderer, window_width, window_height, "You Have Won", bgColor, true, 0);
        game->state = GS_WON;

        FILE* file = fopen("save.txt", "w");
        if (!file) {
            perror("Failed to open the file");
            return;
        }
        fclose(file);
        printf("File %s has been cleared.\n", "save.txt");

        printf("Game won detected\n");
    }

    SDL_Event event;
    if (game->state == GS_WON || game->state == GS_LOST) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                printf("Space bar is detected\n");

                game->state = GS_PLAYING;

                if (highScoreBoard.count < MAX_HIGH_SCORES) {
                    char playerName[MAX_NAME_LENGTH];
                    ask_for_player_name(game, playerName);
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
                }

                currentState = SCORE_PAGE;
                printf("Transitioned to SCORE_PAGE\n");
                break;
            }
        }
    }
}


void renderMachinePage(Game* game) {
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
    render_movement_arrows(renderer, window_width / 10, window_height, DIR_LEFT);
    render_esc(renderer, currentState);

    update(game, 0.016);
    int player_grid_offset_x = 0;

    render_thegrid(renderer, window_width / 2, window_height, game, false, player_grid_offset_x);

    if (has_lost(game)) {
        SDL_Color bgColor = {255, 0, 0, 200};
        render_splash_screen(renderer, window_width, window_height, "You Have Lost", bgColor, true, 0);
        game->state = GS_LOST;
    } else if (has_won(game)) {
        SDL_Color bgColor = {0, 255, 0, 200};
        render_splash_screen(renderer, window_width, window_height, "You Have Won", bgColor, true, window_width / 2);
        game->state = GS_WON;
    }

    int ai_grid_offset_x = window_width / 2;
    render_thegrid(renderer, window_width / 2, window_height, game, true, ai_grid_offset_x);
    if (game->last_directions != DIR_NONE) {
        render_movement_arrows(renderer, window_width, window_height, game->last_directions);
    }

    SDL_Event event;
    if (game->state == GS_WON || game->state == GS_LOST) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                printf("Space bar is detected\n");

                if (highScoreBoard.count < MAX_HIGH_SCORES) {
                    char playerName[MAX_NAME_LENGTH];
                    ask_for_player_name(game, playerName);
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
                }

                game->state = GS_PLAYING;
                currentState = SCORE_PAGE;
                break;
            }
        }
    }
}

void renderScorePage() {
    GameState game;
    game = SCORE_PAGE;
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    if (!renderer) {
        printf("Error: Renderer is NULL\n");
        return;
    }

    if (!window) {
        printf("Error: Window is NULL\n");
        return;
    }

    load_high_scores("score.txt");

    if (highScoreBoard.count < 0 || highScoreBoard.count > MAX_HIGH_SCORES) {
        printf("Error: Invalid highScoreBoard.count (%d)\n", highScoreBoard.count);
        return;
    }

    TTF_Font* scoreFont = TTF_OpenFont("assets/Ndot-55.ttf", 72);
    if (!scoreFont) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }

    TTF_Font* scoreListFont = TTF_OpenFont("assets/Ndot-55.ttf", 72);
    if (!scoreListFont) {
        printf("Error loading font for score list: %s\n", TTF_GetError());
        TTF_CloseFont(scoreFont);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 168, 4, 61, 255);
    SDL_RenderClear(renderer);

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

    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    int titleWidth, titleHeight;
    SDL_QueryTexture(titleTexture, NULL, NULL, &titleWidth, &titleHeight);

    SDL_Rect titleRect = {
        (windowWidth - titleWidth) / 2,
        20,
        titleWidth,
        titleHeight
    };

    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
    SDL_DestroyTexture(titleTexture);

    int yOffset = titleRect.y + titleRect.h + 40;
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
            50,
            yOffset,
            scoreSurface->w,
            scoreSurface->h
        };

        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        SDL_DestroyTexture(scoreTexture);

        yOffset += 150;
    }

    TTF_CloseFont(scoreFont);
    TTF_CloseFont(scoreListFont);

    printf("Rendered Score Page\n");
    render_movement_arrows(renderer, window_width / 10, window_height, DIR_LEFT);
    render_esc(renderer, game);
}

void timertext(SDL_Renderer* renderer, Game* game) {
    static TTF_Font* font = NULL;
    if (font == NULL) {
        font = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 48);
        if (font == NULL) {
            printf("Error: Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
            return;
        }
    }

    char timeStr[20];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", game->time.mint, game->time.scnd);

    SDL_Color white = {42, 35, 86, 255};
    SDL_Surface* surfacet = TTF_RenderText_Blended(font, timeStr, white);
    if (surfacet == NULL) {
        printf("Error: Failed to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture* texturet = SDL_CreateTextureFromSurface(renderer, surfacet);
    if (texturet == NULL) {
        printf("Error: Failed to create texture from surface! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surfacet);
        return;
    }

    SDL_Rect dest = {100, 100, surfacet->w, surfacet->h};
    SDL_RenderCopy(renderer, texturet, NULL, &dest);

    SDL_DestroyTexture(texturet);
    SDL_FreeSurface(surfacet);

    printf("Rendered time: %s\n", timeStr);
}
