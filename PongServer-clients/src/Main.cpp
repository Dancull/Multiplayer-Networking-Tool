#include "SDL_net.h"
#include "MyGame.h"
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

// Constants for server connection
const char* IP_NAME = "localhost";  // IP address of the server
const Uint16 PORT = 55555;  // Port to connect to

// Flag to control the main game loop
bool is_running = true;

// Instance of the game object
MyGame* game = new MyGame();

// Encryption key for XOR encryption
const char* KEY = "jnmvk!_!aU5N_3iKdodDD6Z3JzbWSMUiNnnG_b8IGuGcJgQPPajpWR8y6YWqz29n";

// XOR encryption function to decode messages
// Encrypts/decrypts the text using the provided XOR key
char* xorCypher(char* text, const char* XOR_KEY) {
    size_t keyLength = strlen(XOR_KEY);  // Get the length of the XOR key
    for (size_t i = 0; i < strlen(text); i++) {
        text[i] ^= XOR_KEY[i % keyLength];  // XOR each character with the key
    }
    return text;  // Return the modified text
}

// Network thread to handle received data from the server
// This function is run in a separate thread to receive messages from the server continuously
static int on_receive(void* socket_ptr) {
    TCPsocket socket = (TCPsocket)socket_ptr;
    const int message_length = 1024;
    char message[message_length];
    int received;

    do {
        // Receive data from the server
        received = SDLNet_TCP_Recv(socket, message, message_length);
        message[received] = '\0';  // Null-terminate the received message
        std::cout << "Data Recieved " << message << std::endl;

        // Decrypt the received message
        char* newMessage = xorCypher(message, KEY);
        std::cout << "Data Decrypted: " << newMessage << std::endl;

        // Split the decrypted message into command and arguments
        char* pch = strtok(newMessage, ",");
        string cmd(pch);  // Get the command

        vector<string> args;
        while (pch != NULL) {
            pch = strtok(NULL, ",");
            if (pch != NULL) {
                args.push_back(string(pch));  // Add each argument to the vector
            }
        }

        // Pass the command and arguments to the game object
        game->on_receive(cmd, args);

        // Exit the loop if the command is "exit"
        if (cmd == "exit") {
            break;
        }

    } while (received > 0 && is_running);

    return 0;  // Return when done
}

// Network thread to send data to the server
// Continuously sends data from the game to the server
static int on_send(void* socket_ptr) {
    TCPsocket socket = (TCPsocket)socket_ptr;

    while (is_running) {
        if (game->messages.size() > 0) {
            string message = "CLIENT_DATA";

            // Add each message in the queue to the data string
            for (auto m : game->messages) {
                message += "," + m;
            }

            game->messages.clear();  // Clear the message queue after sending

            cout << "Sending_TCP: " << message << endl;
            SDLNet_TCP_Send(socket, message.c_str(), message.length());  // Send the message to the server
        }

        SDL_Delay(1);  // Small delay to prevent blocking
    }

    return 0;  // Return when done
}

// Main game loop, handles input, updates, and rendering
void loop(SDL_Renderer* renderer) {
    SDL_Event event;

    while (is_running) {
        // Handle all SDL events (keyboard, quit)
        while (SDL_PollEvent(&event)) {
            if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && event.key.repeat == 0) {
                game->input(event);  // Pass the event to the game for processing

                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:  // Exit game if Escape is pressed
                    is_running = false;
                    break;
                default:
                    break;
                }
            }

            if (event.type == SDL_QUIT) {
                is_running = false;  // Exit game if the window is closed
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Set the draw color to black
        SDL_RenderClear(renderer);  // Clear the screen

        game->update();  // Update the game state
        game->render(renderer);  // Render the game scene

        SDL_RenderPresent(renderer);  // Present the rendered frame

        SDL_Delay(17);  // Delay to achieve ~60fps
    }
}

// Initializes the game window and renderer, then starts the game loop
int run_game() {
    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow(
        "Multiplayer Pong Client",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (nullptr == window) {
        std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
        return -1;  // Window creation failure
    }

    // Create an SDL renderer for drawing
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (nullptr == renderer) {
        std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
        return -1;  // Renderer creation failure
    }

    game->loadTextures(renderer);  // Load textures and assets for the game

    loop(renderer);  // Start the main game loop

    game->destroyTextures();  // Clean up resources after the game loop ends

    // Clean up SDL resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}

// Main function to initialize SDL, SDL_net, and manage network communication
int main(int argc, char** argv) {
    // Initialize SDL
    if (SDL_Init(0) == -1) {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);  // SDL initialization failure
    }

    // Initialize SDL_net (networking)
    if (SDLNet_Init() == -1) {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(2);  // SDL_net initialization failure
    }

    IPaddress ip;

    // Resolve host (IP and port) into an IPaddress type
    if (SDLNet_ResolveHost(&ip, IP_NAME, PORT) == -1) {
        printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(3);  // Host resolution failure
    }

    // Open a TCP connection to the server
    TCPsocket socket = SDLNet_TCP_Open(&ip);

    if (!socket) {
        printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(4);  // TCP socket open failure
    }

    // Start separate threads for receiving and sending data
    SDL_CreateThread(on_receive, "ConnectionReceiveThread", (void*)socket);
    SDL_CreateThread(on_send, "ConnectionSendThread", (void*)socket);

    run_game();  // Start the game

    delete game;  // Clean up game instance

    // Close the TCP connection to the server
    SDLNet_TCP_Close(socket);

    // Shutdown SDL_net and SDL
    SDLNet_Quit();
    SDL_Quit();

    return 0;  // Program completed successfully
}