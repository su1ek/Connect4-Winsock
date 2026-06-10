# Connect 4 Multiplayer

A multiplayer implementation of the classic Connect 4 game written in C++ using Winsock API and TCP sockets.

**Features**
- Multiplayer LAN gameplay
- Client-server architecture
- TCP communication using Winsock
- Win detection:
  - Horizontal
  - Vertical
  - Diagonal
- Draw detection
- Player nicknames
- Custom server IP configuration
- Connection loss handling
- Console-based interface

**Technologies**
- C++
- Winsock2
- TCP/IP
- Visual Studio

**How to run**

Server
1. Build the project
2. Run connect4.exe
3. Enter your nickname
4. Wait for client to connect

Client
1. Run connect4klient.exe
2. Enter your nickname
3. Enter the server IP address
4. Start playing

**Game rules**
Players take turns dropping pieces into one of seven columns. The first player to connect four pieces horizontally, vertically or diagonally wins the game. If the board becomes full and no player has connected four pieces, the game ends in a draw.

**Future improvements**
- Graphical user interface
- Replay system
- Improved network protocol
- Cross-platform support
- Multiple game rooms

**What I learned**

This project helped me practice:
- TCP socket programming
- Client-server architecture
- Winsock API
- Multiplayer game logic
- Error handling
- Git and GitHub workflow
