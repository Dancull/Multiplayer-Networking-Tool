#include "MyGame.h"
#include "SDL_ttf.h"
#include <iostream>
#include <SDL_image.h>
#include <SDL_mixer.h>

// -------------------------------------------------
// Global Variables and Constants
// -------------------------------------------------

// Font and sound resources
TTF_Font* font = nullptr;  // Font for rendering text
Mix_Chunk* ballHitSound = nullptr;  // Sound when ball hits paddle or wall
Mix_Chunk* scoreSound = nullptr;    // Sound when player scores
static SDL_Color TEXT_COLOUR = { 255, 255, 255 };  // White text color

// Strings for player scores
std::string player1ScoreTextString;
std::string player2ScoreTextString;

// Textures for background, ball, and paddles
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* ballTexture = nullptr;

// -------------------------------------------------
// MyGame Class Methods
// -------------------------------------------------

// Helper function to smoothly reconcile the player's position
void MyGame::reconcilePlayerPosition(SDL_Rect& player, int serverPosition) {
    const float lerpFactor = 0.1f;  // Adjust for responsiveness (0.1 is smooth)
    player.y = player.y + (serverPosition - player.y) * lerpFactor;  // Smooth the position
}

// Handle received game data from server
void MyGame::on_receive(std::string cmd, std::vector<std::string>& args) {
    if (cmd == "GAME_DATA") {
        if (args.size() == 9) {
            // Update the game state with server data (player positions, scores, etc.)
            game_data.player1Y = std::stoi(args.at(0));
            game_data.player2Y = std::stoi(args.at(1));
            game_data.ballX = std::stoi(args.at(2));
            game_data.ballY = std::stoi(args.at(3));
            game_data.player1X = std::stoi(args.at(4));
            game_data.player2X = std::stoi(args.at(5));
            game_data.connectionID = std::stoi(args.at(6));
            game_data.player1Score = std::stoi(args.at(7));
            game_data.player2Score = std::stoi(args.at(8));

            // After receiving server data, update player positions, ball position, etc.
            player1.y = game_data.player1Y;
            player2.y = game_data.player2Y;
            ball.x = game_data.ballX;
            ball.y = game_data.ballY;
            player1.x = game_data.player1X;
            player2.x = game_data.player2X;
        }
    }
}

// Send messages to the server
void MyGame::send(std::string message) {
    messages.push_back(message);  // Add message to queue
}

// Handle player input (keyboard events)
void MyGame::input(SDL_Event& event) {
    switch (event.key.keysym.sym) {
    case SDLK_w:
        if (event.type == SDL_KEYDOWN) {
            send("W_DOWN");
            game_data.moveUp = true;
        }
        else if (event.type == SDL_KEYUP) {
            send("W_UP");
            game_data.moveUp = false;
        }
        break;

    case SDLK_s:
        if (event.type == SDL_KEYDOWN) {
            send("S_DOWN");
            game_data.moveDown = true;
        }
        else if (event.type == SDL_KEYUP) {
            send("S_UP");
            game_data.moveDown = false;
        }
        break;
    }
}

// Check and play score sound when score changes
void MyGame::checkAndPlayScoreSound() {
    static int previousPlayer1Score = 0;
    static int previousPlayer2Score = 0;

    // Check if Player 1's score has increased
    if (game_data.player1Score > previousPlayer1Score) {
        Mix_PlayChannel(-1, scoreSound, 0);  // Play score sound
        previousPlayer1Score = game_data.player1Score;
    }

    // Check if Player 2's score has increased
    if (game_data.player2Score > previousPlayer2Score) {
        Mix_PlayChannel(-1, scoreSound, 0);  // Play score sound
        previousPlayer2Score = game_data.player2Score;
    }
}

// Update the graphical user interface (GUI)
void MyGame::updateGUI(SDL_Renderer* renderer) {
    checkAndPlayScoreSound();

    SDL_Rect textRect = { 20, 8, 0, 0 };
    std::string screenText = "Player 1: " + std::to_string(game_data.player1Score);
    renderText(renderer, screenText, textRect);

    textRect = { 624, 8, 0, 0 };
    screenText = "Player 2: " + std::to_string(game_data.player2Score);
    renderText(renderer, screenText, textRect);
}

// Helper function to render text to the screen
void MyGame::renderText(SDL_Renderer* renderer, std::string text, SDL_Rect& rect) {
    if (!font) {
        std::cerr << "Font is not initialized!" << std::endl;
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), TEXT_COLOUR);
    if (!textSurface) {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_QueryTexture(textTexture, NULL, NULL, &rect.w, &rect.h);
    SDL_RenderCopy(renderer, textTexture, NULL, &rect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

// Handle player movement based on input and server data
void MyGame::playerMovement() {
    // Player 1 Movement (based on input and prediction)
    if (game_data.connectionID == 1) {
        if (game_data.moveUp) {
            player1.y += 1 * game_data.PLAYER_SPEED;  // Predictive movement
        }
        if (game_data.moveDown) {
            player1.y -= 1 * game_data.PLAYER_SPEED;  // Predictive movement
        }
    }

    // Player 2 Movement (based on input and prediction)
    if (game_data.connectionID == 2) {
        if (game_data.moveUp) {
            player2.y += 1 * game_data.PLAYER_SPEED;  // Predictive movement
        }
        if (game_data.moveDown) {
            player2.y -= 1 * game_data.PLAYER_SPEED;  // Predictive movement
        }
    }
}

// Load textures and other assets (background, ball, paddles)
void MyGame::loadTextures(SDL_Renderer* renderer) {
    // Initialize TTF and font
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }
    font = TTF_OpenFont("E:/Dan_Code_Folder/FINAL SERVER PROJECT/CI628/assets/Supercharge-JRgPo.otf", 22);
    if (!font) {
        std::cerr << "Failed to load font! TTF Error: " << TTF_GetError() << std::endl;
        return;
    }

    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "SDL_mixer could not open audio! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }

    ballHitSound = Mix_LoadWAV("E:/Dan_Code_Folder/FINAL SERVER PROJECT/CI628/assets/ball_hit.wav");
    if (!ballHitSound) {
        std::cerr << "Failed to load ball hit sound: " << Mix_GetError() << std::endl;
    }
    else {
        Mix_VolumeChunk(ballHitSound, MIX_MAX_VOLUME);  // Set to maximum volume
    }

    scoreSound = Mix_LoadWAV("E:/Dan_Code_Folder/FINAL SERVER PROJECT/CI628/assets/score.wav");
    if (!scoreSound) {
        std::cerr << "Failed to load score sound: " << Mix_GetError() << std::endl;
    }

    loadTexture(renderer, "E:/Dan_Code_Folder/FINAL SERVER PROJECT/CI628/assets/bg.png", backgroundTexture);
    loadTexture(renderer, "E:/Dan_Code_Folder/FINAL SERVER PROJECT/CI628/assets/ball.png", ballTexture);
    loadTexture(renderer, "E:/Dan_Code_Folder/FINAL SERVER PROJECT/CI628/assets/paddle2.png", game_data.leftpaddleTexture);
    loadTexture(renderer, "E:/Dan_Code_Folder/FINAL SERVER PROJECT/CI628/assets/paddle.png", game_data.rightpaddleTexture);
}

// Helper function to load textures
void MyGame::loadTexture(SDL_Renderer* renderer, const std::string& path, SDL_Texture*& texture) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
    }
}

// -------------------------------------------------
// Rendering and Cleanup
// -------------------------------------------------

// Render game objects and update GUI
void MyGame::render(SDL_Renderer* renderer) {
    if (backgroundTexture) {
        SDL_Rect backgroundRect = { 0, 0, 800, 600 };
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, &backgroundRect);
    }
    if (game_data.leftpaddleTexture) {
        SDL_RenderCopy(renderer, game_data.leftpaddleTexture, nullptr, &player1);
    }
    if (game_data.rightpaddleTexture) {
        SDL_RenderCopy(renderer, game_data.rightpaddleTexture, nullptr, &player2);
    }
    if (ballTexture) {
        SDL_RenderCopy(renderer, ballTexture, nullptr, &ball);
    }
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    updateGUI(renderer);  // Update GUI elements (scores)
}

// Clean up textures and audio resources
void MyGame::cleanUp() {
    Mix_FreeChunk(ballHitSound);
    Mix_FreeChunk(scoreSound);
    Mix_CloseAudio();
    Mix_Quit();

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyTexture(game_data.leftpaddleTexture);
    SDL_DestroyTexture(game_data.rightpaddleTexture);
    SDL_DestroyTexture(ballTexture);
    SDL_DestroyTexture(backgroundTexture);
}

// Update the game state based on server data and player input
void MyGame::update() {
    if (abs(player1.y - game_data.player1Y) > 5) {
        player1.y += (game_data.player1Y - player1.y) * 0.3;
    }
    if (abs(player2.y - game_data.player2Y) > 5) {
        player2.y += (game_data.player2Y - player2.y) * 0.3;
    }
    playerMovement();  // Handle movement logic
}

// Destroy and release textures
void MyGame::destroyTextures() {
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(game_data.leftpaddleTexture);
    SDL_DestroyTexture(game_data.rightpaddleTexture);
    SDL_DestroyTexture(ballTexture);
}
