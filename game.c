#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <math.h> // Include the math header
#include "include/game.h"
#include "include/utils.h"
#include "include/input.h"

// Function to draw a rounded rectangle
void render_rounded_rect(SDL_Renderer* renderer, SDL_Rect* rect, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    // Create a temporary surface for anti-aliased drawing
    SDL_Surface* surface = SDL_CreateRGBSurface(0, rect->w, rect->h, 32, 0, 0, 0, 0);
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    // Enable alpha blending for smooth edges
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    // Draw filled rounded rectangle
    SDL_RenderCopy(renderer, texture, NULL, rect);
    SDL_DestroyTexture(texture);
}

void render_thegrid(SDL_Renderer* renderer, int grid_width, int grid_height, Game* game, bool is_ai, int offset_x) {
    const int tile_size = TILE_SIZE;
    const int gap = GAP;
    const int corner_radius = 10;
    
    int start_x = offset_x + (grid_width - (GRID_SIZE * tile_size + (GRID_SIZE - 1) * gap)) / 2;
    int start_y = (grid_height - (GRID_SIZE * tile_size + (GRID_SIZE - 1) * gap)) / 2;
    
    // Dark background color for tiles
    SDL_Color tile_color = {28, 28, 35, 255}; // Dark background
    
    // Custom color palette for numbers based on the image
    SDL_Color number_colors[] = {
        {110, 203, 245, 255},  // Light Sky Blue #6ECBF5
        {194, 82, 225, 255},   // Medium Orchid #C252E1
        {224, 217, 246, 255},  // Lavender #E0D9F6
        {88, 106, 226, 255},   // Royal Blue #586AE2
        {42, 35, 86, 255}      // Midnight Blue #2A2356
    };
    
    // Render grid
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            int value = is_ai ? game->ai_board[y * GRID_SIZE + x] : game->board[y * GRID_SIZE + x];
            int tile_x = start_x + x * (tile_size + gap);
            int tile_y = start_y + y * (tile_size + gap);
            SDL_Rect tile_rect = {tile_x, tile_y, tile_size, tile_size};
            
            // Render dark tile background
            render_rounded_rect(renderer, &tile_rect, corner_radius, tile_color);
            
            // Render value text if tile is not empty
            if (value > 0) {
                TTF_Font* font = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 48);
                if (font) {
                    char value_str[10];
                    snprintf(value_str, sizeof(value_str), "%d", value);
                    
                    // Select color based on value
                    SDL_Color text_color;
                    int color_index = ((int)log2(value) - 1) % 5;  // Cycle through the 5 colors
                    text_color = number_colors[color_index];
                    
                    SDL_Surface* text_surface = TTF_RenderText_Blended(font, value_str, text_color);
                    if (text_surface) {
                        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
                        SDL_Rect text_rect = {
                            tile_x + (tile_size - text_surface->w) / 2,
                            tile_y + (tile_size - text_surface->h) / 2,
                            text_surface->w,
                            text_surface->h
                        };
                        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
                        SDL_DestroyTexture(text_texture);
                        SDL_FreeSurface(text_surface);
                    }
                    TTF_CloseFont(font);
                }
            }
        }
    }
}
void render_score_and_moves(Game* game, SDL_Renderer* renderer)
{
    TTF_Font* font = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 48);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    char text[32] = { 0 };
    snprintf(text, 32, "Score: %d", game->score);
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, text, (SDL_Color) { 0, 0, 0, 255 });
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, &(SDL_Rect) { 50, 200, 400, 80 });
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);

    snprintf(text, 32, "Moves: %d", game->moves);
    textSurface = TTF_RenderUTF8_Blended(font, text, (SDL_Color) { 0, 0, 0, 0 });
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_RenderCopy(renderer, textTexture, NULL, &(SDL_Rect) { 50, 280, 400, 80 });
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}
void initialize_game(Game* game) {
    // Initialize the game state
    game->state = GS_PLAYING;
    game->action = A_NONE;
    game->score = 0;
    game->moves = 0;
    game->last_inserted = -1;
    game->has_moved = false;

    // Initialize the board with zeros
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        game->board[i] = 0;

    }

    // Add initial tiles
    add_random_tile(game);
    add_random_tile(game);

}

void initialize_gamePVM(Game* game) {
    // Initialize the game state
   					        
game->state = GS_PLAYING;
    game->action = A_NONE;
    game->score = 0;
    game->moves = 0;
    game->last_inserted = -1;
    game->has_moved = false; 
    // Initialize the board with zeros
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        game->board[i] = 0;
	}
 for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        game->ai_board[i] = 0; // Initialize AI board
    }

    // Add initial tiles
    add_random_tile(game);
    add_random_tile(game);
    add_random_tileM(game); // Add initial tiles for AI
    add_random_tileM(game);
}
void initialize_gameM(Game* game) {
    // Initialize the game state
    game->Mstate = GS_PLAYING;
    game->Maction = A_NONE;
    game->Mscore = 0;
    game->Mmoves = 0;
    game->Mlast_inserted = -1;
    game->Mhas_moved = false;

    // Initialize the board with zeros
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
     
        game->ai_board[i] = 0; // Initialize AI board
    }


    add_random_tileM(game); // Add initial tiles for AI
    add_random_tileM(game);
}

bool has_won(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (game->board[y * GRID_SIZE + x] == 11) {
                return true;
            }
        }
    }
    return false;
}

bool has_lost(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (game->board[y * GRID_SIZE + x] == 0) {
                return false;
            }
        }
    }
    for (int a = 0; a < GRID_SIZE - 1; a++) {
        for (int b = 0; b < GRID_SIZE; b++) {
            if (game->board[a * GRID_SIZE + b] == game->board[(a + 1) * GRID_SIZE + b] ||
                game->board[b * GRID_SIZE + a] == game->board[b * GRID_SIZE + a + 1]) {
                return false;
            }
        }
    }
    return true;
}

int empty_tiles(Game* game) {
    int amount = 0;
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (game->board[y * GRID_SIZE + x] == 0) {
                amount++;
            }
        }
    }
    return amount;
}

bool move_cell_maybe_break(Game* game, int* target, int* src) {
    if (*target == 0 && *src != 0) {
        *target = *src;
        *src = 0;
        game->has_moved = true;
    } else if (*target == *src && *target != 0 && *src != 0) {
        *target = *target + *target;
        *src = 0;
       game->score += 1LL << *target; // 1LL ensures the result is a long long
        game->has_moved = true;
        return true;
    } else if (*src != 0) {
        return true;
    }
    return false;
}

void move_right(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int ix = GRID_SIZE - 1; ix >= 0; ix--) {
            for (int jx = ix - 1; jx >= 0; jx--) {
                if (move_cell_maybe_break(game, &game->board[y * GRID_SIZE + ix], &game->board[y * GRID_SIZE + jx])) {
                    break;
                }
            }
        }
    }
}

void move_left(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int ix = 0; ix < GRID_SIZE - 1; ix++) {
            for (int jx = ix + 1; jx < GRID_SIZE; jx++) {
                if (move_cell_maybe_break(game, &game->board[y * GRID_SIZE + ix], &game->board[y * GRID_SIZE + jx])) {
                    break;
                }
            }
        }
    }
}

void move_down(Game* game) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int iy = GRID_SIZE - 1; iy >= 0; iy--) {
            for (int jy = iy - 1; jy >= 0; jy--) {
                if (move_cell_maybe_break(game, &game->board[iy * GRID_SIZE + x], &game->board[jy * GRID_SIZE + x])) {
                    break;
                }
            }
        }
    }
}

void move_up(Game* game) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int iy = 0; iy < GRID_SIZE - 1; iy++) {
            for (int jy = iy + 1; jy < GRID_SIZE; jy++) {
                if (move_cell_maybe_break(game, &game->board[iy * GRID_SIZE + x], &game->board[jy * GRID_SIZE + x])) {
                    break;
                }
            }
        }
    }
}
void Mmove_right(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int ix = GRID_SIZE - 1; ix >= 0; ix--) {
            for (int jx = ix - 1; jx >= 0; jx--) {
                if (move_cell_maybe_break(game, &game->ai_board[y * GRID_SIZE + ix], &game->ai_board[y * GRID_SIZE + jx])) {
                    break;
                }
            }
        }
    }
}

void Mmove_left(Game* game) {
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int ix = 0; ix < GRID_SIZE - 1; ix++) {
            for (int jx = ix + 1; jx < GRID_SIZE; jx++) {
                if (move_cell_maybe_break(game, &game->ai_board[y * GRID_SIZE + ix], &game->ai_board[y * GRID_SIZE + jx])) {
                    break;
                }
            }
        }
    }
}

void Mmove_down(Game* game) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int iy = GRID_SIZE - 1; iy >= 0; iy--) {
            for (int jy = iy - 1; jy >= 0; jy--) {
                if (move_cell_maybe_break(game, &game->ai_board[iy * GRID_SIZE + x], &game->ai_board[jy * GRID_SIZE + x])) {
                    break;
                }
            }
        }
    }
}

void Mmove_up(Game* game) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int iy = 0; iy < GRID_SIZE - 1; iy++) {
            for (int jy = iy + 1; jy < GRID_SIZE; jy++) {
                if (move_cell_maybe_break(game, &game->ai_board[iy * GRID_SIZE + x], &game->ai_board[jy * GRID_SIZE + x])) {
                    break;
                }
            }
        }
    }
}


void update(Game* game, double delta) {
    if (game->state == GS_PLAYING) {
        game->has_moved = false;
        handleGamePageEvent(game);

        if (game->action != A_NONE) {
            game->action = A_NONE;

            if (game->has_moved) {
                game->last_inserted = add_random_tile(game);
            } else {
                game->last_inserted = -1;
            }

            game->moves++;
        }

        if (has_won(game)) {
            game->state = GS_WON;
        } else if (has_lost(game)) {
            game->state = GS_LOST;
        }
    }
}

void PlayerVSMachine(Game* game, double delta) {    
 if (game->state == GS_PLAYING) {
        game->has_moved = false;
        handleGamePageEvent(game);

        if (game->action != A_NONE) {
            game->action = A_NONE;

            if (game->has_moved) {
                game->last_inserted = add_random_tile(game);
            } else {
                game->last_inserted = -1;
            }

            game->moves++;
        }

        if (has_won(game)) {
            game->state = GS_WON;
        } else if (has_lost(game)) {
            game->state = GS_LOST;
        }

 static double ai_timer=0.0;
 ai_timer += delta;
   
 if (game->Mstate == GS_PLAYING) {
        game->Mhas_moved = false;
        if (ai_timer >= 0.5) {
            ai_move(game);
            ai_timer = 0.0; // Reset the timer
        }
      

        if (game->Maction != A_NONE) {
            game->Maction = A_NONE;

            if (game->Mhas_moved) {
                game->Mlast_inserted = add_random_tileM(game);
            } else {
                game->Mlast_inserted = -1;
            }

            game->Mmoves++;
        }

        if (has_won(game)) {
            game->Mstate = GS_WON;
        } else if (has_lost(game)) {
            game->Mstate = GS_LOST;
        }

        // AI makes a move every 0.5 seconds
            }
} 
}

void Machine(Game* game, double delta){
static double ai_timer=0.0;
ai_timer += delta;
 if (game->Mstate == GS_PLAYING) {
        game->Mhas_moved = false;
        if (ai_timer >= 0.5) {
            ai_move(game);
            ai_timer = 0.0; // Reset the timer
        }
      

        if (game->Maction != A_NONE) {
            game->Maction = A_NONE;

            if (game->Mhas_moved) {
                game->Mlast_inserted = add_random_tileM(game);
            } else {
                game->Mlast_inserted = -1;
            }

            game->Mmoves++;
        }

        if (has_won(game)) {
            game->Mstate = GS_WON;
        } else if (has_lost(game)) {
            game->Mstate = GS_LOST;
        }

        // AI makes a move every 0.5 seconds
            }

} 
void render_splash_screen(SDL_Renderer* renderer, const char* text, SDL_Color color)
{
    // Open the font with a larger size for smoother text
    TTF_Font* font = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 96);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return;
    }

    // Get renderer's output size to calculate the center
    int screenWidth, screenHeight;
    SDL_GetRendererOutputSize(renderer, &screenWidth, &screenHeight);

    // Set background color and fill a centered rectangle
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    int rectWidth = 512;
    int rectHeight = 512;
    SDL_Rect rect = {
        (screenWidth - rectWidth) / 2, // Center X
        (screenHeight - rectHeight) / 2, // Center Y
        rectWidth,
        rectHeight
    };
    SDL_RenderFillRect(renderer, &rect);

    // Render the text and center it
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, text, (SDL_Color){ 0, 0, 0, 255 });
    if (!textSurface) {
        SDL_Log("Failed to render text: %s", TTF_GetError());
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

    // Center the text inside the rectangle
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;
    SDL_Rect textRect = {
        rect.x + (rect.w - textWidth) / 2, // Center X
        rect.y + (rect.h - textHeight) / 2, // Center Y
        textWidth,
        textHeight
    };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    // Clean up
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    TTF_CloseFont(font);
}

void ask_for_player_name(Game* game, char* playerName) {
    SDL_StartTextInput();

    char input[MAX_NAME_LENGTH] = "";
    SDL_Event e;
    bool done = false;

    // Dimensions for the input box
    int windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    int boxWidth = 400;
    int boxHeight = 200;
    SDL_Rect inputBox = {
        (windowWidth - boxWidth) / 2, // Center horizontally
        (windowHeight - boxHeight) / 2, // Center vertically
        boxWidth,
        boxHeight
    };

    // Colors
    SDL_Color backgroundColor = {30, 30, 30, 255}; // Dark gray
    SDL_Color borderColor = {200, 200, 200, 255};  // Light gray
    SDL_Color textColor = {255, 255, 255, 255};    // White

    while (!done) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                done = true;
                break;
            } else if (e.type == SDL_TEXTINPUT || e.type == SDL_KEYDOWN) {
                if (e.type == SDL_TEXTINPUT) {
                    if (strlen(input) < MAX_NAME_LENGTH - 1) {
                        strncat(input, e.text.text, MAX_NAME_LENGTH - strlen(input) - 1);
                    }
                } else if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(input) > 0) {
                    input[strlen(input) - 1] = '\0';
                } else if (e.key.keysym.sym == SDLK_RETURN && strlen(input) > 0) {
                    done = true;
                }
            }
        }

        // Clear the screen (but keep the existing background)
        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        // SDL_RenderClear(renderer);

        // Render the existing background (assuming it's already rendered)
        // For example, if you're on the game page, the grid is already rendered.

        // Render input box background
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
        SDL_RenderFillRect(renderer, &inputBox);

        // Render input box border
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(renderer, &inputBox);

        // Load font for title and input
        TTF_Font* fontTitle = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 36);
        TTF_Font* fontInput = TTF_OpenFont("assets/LilitaOne-Regular.ttf", 28);
        if (!fontTitle || !fontInput) {
            printf("Error loading font: %s\n", TTF_GetError());
            SDL_StopTextInput();
            return;
        }

        // Render title "Name:"
        SDL_Surface* titleSurface = TTF_RenderUTF8_Blended(fontTitle, "Name:", textColor);
        SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
        SDL_Rect titleRect = {
            inputBox.x + 20,  // Slightly left of input box
            inputBox.y + 20,  // Slightly above input box
            titleSurface->w,
            titleSurface->h
        };
        SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);
        SDL_FreeSurface(titleSurface);
        SDL_DestroyTexture(titleTexture);

        // Render the input text
        SDL_Surface* inputSurface = TTF_RenderUTF8_Blended(fontInput, input, textColor);
        SDL_Texture* inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
        SDL_Rect inputRect = {
            inputBox.x + 20,  // Left padding
            inputBox.y + 80,  // Centered vertically in the box
            inputSurface->w,
            inputSurface->h
        };
        SDL_RenderCopy(renderer, inputTexture, NULL, &inputRect);
        SDL_FreeSurface(inputSurface);
        SDL_DestroyTexture(inputTexture);

        // Clean up fonts
        TTF_CloseFont(fontTitle);
        TTF_CloseFont(fontInput);

        // Update the screen
        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
    strcpy(playerName, input);
}
void add_high_score(const char* name, Game* game) {

    if (highScoreBoard.count < MAX_HIGH_SCORES) {
        // Add new score if there's space
        strcpy(highScoreBoard.highScores[highScoreBoard.count].name, name);
        highScoreBoard.highScores[highScoreBoard.count].score = game->score; // Ensure this is correct
        highScoreBoard.count++;
    } else {
        // Replace the lowest score if the new score is higher
        int minIndex = 0;
        for (int i = 1; i < MAX_HIGH_SCORES; i++) {
            if (highScoreBoard.highScores[i].score < highScoreBoard.highScores[minIndex].score) {
                minIndex = i;
            }
        }

        if (game->score > highScoreBoard.highScores[minIndex].score) {
            strcpy(highScoreBoard.highScores[minIndex].name, name);
            highScoreBoard.highScores[minIndex].score = game->score; // Ensure this is correct
        }
    }

    // Sort high scores in descending order
    for (int i = 0; i < highScoreBoard.count - 1; i++) {
        for (int j = i + 1; j < highScoreBoard.count; j++) {
            if (highScoreBoard.highScores[j].score > highScoreBoard.highScores[i].score) {
                HighScore temp = highScoreBoard.highScores[i];
                highScoreBoard.highScores[i] = highScoreBoard.highScores[j];
                highScoreBoard.highScores[j] = temp;
            }
        }
  }
printf("Adding high score: Name = %s, Score = %d\n", name, game->score);
}
void init_grille_vs_machine(Game* game) {
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        game->board[i] = 0;
        game->ai_board[i] = 0;
    }

    add_random_tile(game);
    add_random_tile(game);
    add_random_tileM(game); // Add initial tiles for AI
    add_random_tileM(game);
}
