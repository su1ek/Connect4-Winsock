#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512
#define DEFAULT_SERVER "127.0.0.1"

using namespace std;

const int ROWS = 6;
const int COLS = 7;
const char CLIENT = 'X';
const char SERVER = 'O';
char board[ROWS][COLS];

void printBoard() {
    cout << "\n";
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            cout << "|" << board[r][c];
        }
        cout << "|\n";
    }
    for (int c = 0; c < COLS; c++) cout << " " << c;
    cout << "\n";
}

bool makeMove(int col, char player) {
    if (col < 0 || col >= COLS || board[0][col] != ' ') return false;
    for (int row = ROWS - 1; row >= 0; row--) {
        if (board[row][col] == ' ') {
            board[row][col] = player;
            return true;
        }
    }
    return false;
}

bool checkLine(char a, char b, char c, char d, char player) {
    return (a == player && b == player && c == player && d == player);
}

bool checkWin(char player) {
    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col <= COLS - 4; ++col)
            if (checkLine(board[row][col], board[row][col + 1],
                board[row][col + 2], board[row][col + 3], player))
                return true;

    for (int col = 0; col < COLS; ++col)
        for (int row = 0; row <= ROWS - 4; ++row)
            if (checkLine(board[row][col], board[row + 1][col],
                board[row + 2][col], board[row + 3][col], player))
                return true;

    for (int row = 0; row <= ROWS - 4; ++row)
        for (int col = 0; col <= COLS - 4; ++col)
            if (checkLine(board[row][col], board[row + 1][col + 1],
                board[row + 2][col + 2], board[row + 3][col + 3], player))
                return true;

    for (int row = 3; row < ROWS; ++row)
        for (int col = 0; col <= COLS - 4; ++col)
            if (checkLine(board[row][col], board[row - 1][col + 1],
                board[row - 2][col + 2], board[row - 3][col + 3], player))
                return true;

    return false;
}

void clearBoard() {
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            board[i][j] = ' ';
}

int main() {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    int iResult;

    clearBoard();

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(DEFAULT_SERVER, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo failed: " << iResult << endl;
        WSACleanup();
        return 1;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "socket failed: " << WSAGetLastError() << endl;
            WSACleanup();
            return 1;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Nie udalo sie polaczyc z serwerem." << endl;
        WSACleanup();
        return 1;
    }

    while (true) {
        printBoard();
        int move;
        cout << "Wybierz kolumne (0-6): ";
        cin >> move;

        if (!makeMove(move, CLIENT)) {
            cout << "Nieprawidlowy ruch, sprobuj ponownie.\n";
            continue;
        }

        send(ConnectSocket, (char*)board, sizeof(board), 0);

        if (checkWin(CLIENT)) {
            printBoard();
            cout << "Klient (X) wygrywa!\n";
            break;
        }

        recv(ConnectSocket, (char*)board, sizeof(board), 0);

        if (checkWin(SERVER)) {
            printBoard();
            cout << "Serwer (O) wygrywa!\n";
            break;
        }
    }

    shutdown(ConnectSocket, SD_SEND);
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
