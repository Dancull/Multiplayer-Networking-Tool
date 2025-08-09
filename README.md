# Multiplayer Pong – C++ Client & Java Server with Encrypted Networking
A cross-language multiplayer reimagining of the classic Pong, featuring a C++ SDL2 client and a Java FXGL server, with real-time networking, XOR-encrypted communication, and responsive gameplay across platforms.

## Features
* Two-Player Online Multiplayer: Play classic Pong head-to-head over the network.
* Server-Authoritative Architecture: Prevents cheating and ensures game state consistency.
* XOR Encryption: Secures all server-client communications (upgradeable to AES).
* Smooth Player Movement: Client-side prediction with server reconciliation for minimal latency effects.
* Optimised Rendering: SDL2 textures for efficient, clean visuals.
* Cross-Language Networking: C++ client and Java server designed for interoperability.

## Prerequisites
* Java 17+ (for the FXGL server)
* C++17 compiler (for the SDL2 client)
* SDL2 and SDL2_Mixer installed
* Maven (for Java server build)

## Installation 

```bash
# Clone the repository
git clone https://github.com/yourusername/multiplayer-pong.git

# Build the Java server
cd multiplayer-pong/server
mvn clean package

# Build the C++ client (example with g++)
cd ../client
g++ -std=c++17 *.cpp -lSDL2 -lSDL2_image -lSDL2_mixer -o pong-client
```

## Usage
This project supports running the Java server and C++ client separately.

| Command                     | Description                                             |
| --------------------------- | ------------------------------------------------------- |
| `java -jar pong-server.jar` | Starts the FXGL TCP server on port 55555                |
| `./pong-client`             | Launches the C++ SDL2 client and connects to the server |

Controls:

* Player 1: W (up), S (down)
* Player 2: I (up), K (down)

## License

Including a LICENSE file clarifies how others may use, modify, and distribute your code. By choosing an open-source license such as MIT, you grant permission for use under defined terms while protecting your intellectual property and limiting liability.

This project is licensed under the MIT License. See [LICENSE](LICENSE) for full details.

## Project Structure

```plaintext
Multiplayer-Networking-Tool/
├── client/
│   ├── assets/
│   │   ├── ball.png
│   │   ├── ball_hit.wav
│   │   ├── bg.png
│   │   ├── paddle.png
│   │   ├── paddle2.png
│   │   ├── score.wav
│   │   └── supercharge-JRgPo.otf
│   ├── libs/
│   │   ├── SDL2
│   │   ├── SDL2_image
│   │   ├── SDL2_mixer
│   │   ├── SDL2_net
│   │   └── SDL2_ttf
│   └── src/
│       ├── main.cpp
│       ├── mygame.cpp
│       └── mygame.h
│
├── server/
│   ├── src/
│   │   ├── main/
│   │   │   ├── java/
│   │   │   │   └── com/
│   │   │   │       └── almasb/
│   │   │   │           └── fxglgames/
│   │   │   │               └── pong/
│   │   │   │                   ├── BallComponent.java
│   │   │   │                   ├── BatComponent.java
│   │   │   │                   ├── EntityType.java
│   │   │   │                   ├── MainUIController.java
│   │   │   │                   ├── NetworkMessages.java
│   │   │   │                   ├── PlayerCharacterController.java
│   │   │   │                   ├── PongApp.java
│   │   │   │                   └── PongFactory.java
│   │   │   └── resources/
│   └── pom.xml

```

## Roadmap
* Client-Side Prediction improvements for even smoother gameplay.
* AES Encryption with dynamic key management.
* Dynamic Player Handling for more than two players.
* GUI Lobby System for player matchmaking.

## Author
Daniel Cullinane – daniel.cullinane@outlook.com

GitHub: @dancull
