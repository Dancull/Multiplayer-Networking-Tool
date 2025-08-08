#ifndef __MY_GAME_H__
#define __MY_GAME_H__

#include <iostream>
#include <vector>
#include <string>
#include "SDL.h"
#include "SDL_image.h"

// MyGame class: handles the game state, player movements, rendering, and network communication
class MyGame {
private:
    // Player and ball positions (rectangles used for drawing)
    SDL_Rect player1 = { 200, 0, 20, 60 };
    SDL_Rect player2 = { 580, 0, 20, 60 };
    SDL_Rect ball = { 390, 0, 20, 20 };

    // Game data structure that stores the game state and assets
    struct GameData {
        int player1Y = 0;  // Y position of player 1
        int player2Y = 0;  // Y position of player 2
        int ballX = 0;     // X position of the ball
        int ballY = 0;     // Y position of the ball
        int player1X = 0;  // X position of player 1
        int player2X = 0;  // X position of player 2

        const double PLAYER_SPEED = 15;  // Speed of player movement
        bool moveDown = false;   // Flag to move player 1 down
        bool moveUp = false;     // Flag to move player 1 up
        int playerID = -1;       // Player's unique ID
        int connectionID = -1;   // Connection ID for the network
        int player1Score = 0;    // Player 1's score
        int player2Score = 0;    // Player 2's score

        SDL_Texture* leftpaddleTexture = nullptr;  // Texture for player 1's paddle
        SDL_Texture* rightpaddleTexture = nullptr; // Texture for player 2's paddle
        SDL_Texture* ballTexture = nullptr;        // Texture for the ball
        SDL_Texture* backgroundTexture = nullptr;  // Texture for the background
    };

    // Game data instance: Holds all the information about the game state
    GameData game_data;

public:
    // Messages to be sent to the server
    std::vector<std::string> messages;

    // Method declarations
    void playerMovement();  // Handles the movement of players
    std::string xor(const std::string& data, char key);  // Encrypt/decrypt messages with XOR
    void on_receive(std::string message, std::vector<std::string>& args);  // Processes incoming messages
    void send(std::string message);  // Sends messages to the server
    void input(SDL_Event& event);  // Handles input events (keyboard presses)
    void update();  // Updates game state (positions, scores, etc.)
    void loadTextures(SDL_Renderer* renderer);  // Loads all game textures
    void loadTexture(SDL_Renderer* renderer, const std::string& path, SDL_Texture*& texture);  // Loads individual textures
    void updateGUI(SDL_Renderer* renderer);  // Updates the graphical user interface (GUI)
    void destroyTextures();  // Frees up memory used by textures
    void render(SDL_Renderer* renderer);  // Renders the game objects to the screen
    void renderText(SDL_Renderer* renderer, std::string text, SDL_Rect& rect);  // Renders text to the screen
    void cleanUp();  // Cleans up game resources
    void checkAndPlayScoreSound();  // Checks score and plays the corresponding sound
    void checkAndPlayBallHitSound();  // Checks for ball collisions and plays the sound
    void checkBallPaddleCollision();  // Checks if the ball collides with a paddle
    void checkBallWallCollision();  // Checks if the ball collides with the wall
    void reconcilePlayerPosition(SDL_Rect& player, int serverPosition);  // Synchronizes player position with server
	

};

#endif  // __MY_GAME_H__
