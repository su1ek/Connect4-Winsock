#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib") //linkowanie do biblioteki Ws2_32.lib ktora ma funkcje winsocka

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

using namespace std;

const int ROWS = 6;                                         //rozmiary planszy
const int COLS = 7;
const char SERVER = 'O';                                    //tu i nizej symbole dla graczy
const char CLIENT = 'X';
char board[ROWS][COLS];                                     //dwuwymiarowa tablica

void printBoard() {
    cout << "\n";
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            cout << "|" << board[r][c];                     // wyswietla aktualny stan planszy z pionowymi kreskami
        }
        cout << "|\n";
    }
    for (int c = 0; c < COLS; c++) cout << " " << c;        //pokazywanie indeksów kolumn od 0-6 aby uzytkownik wiedzial co wpisac
    cout << "\n";
}

void clearBoard() {                                         //ustawia na poczatku wszystkie pola planszy na puste, wywoluje raz na poczatku w mainie
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            board[i][j] = ' ';
}

bool makeMove(int col, char player) {                       
    if (col < 0 || col >= COLS || board[0][col] != ' ') return false;   //sprawdza czy kolumna jest poprawna i czy nie jest pe³na
    for (int row = ROWS - 1; row >= 0; row--) {                         //szuka pierwszego wolnego miejsca od do³u w danej kolumnie
        if (board[row][col] == ' ') {                                   //wstawia tam znak gracza i pozniej dostajemy true jak sie udalo a false jak nie
            board[row][col] = player;
            return true;
        }
    }
    return false;
}

bool checkLine(char a, char b, char c, char d, char player) {           //funkcja sprawdzajaca czy 4 znaki obok siebie sa tego samego gracza
    return (a == player && b == player && c == player && d == player);
}

bool checkWin(char player) {
    //poziomo
    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col <= COLS - 4; ++col)
            if (checkLine(board[row][col], board[row][col + 1],
                board[row][col + 2], board[row][col + 3], player))
                return true;
    //pionowo
    for (int col = 0; col < COLS; ++col)
        for (int row = 0; row <= ROWS - 4; ++row)
            if (checkLine(board[row][col], board[row + 1][col],
                board[row + 2][col], board[row + 3][col], player))
                return true;
    //ukoœnie z lewej górnej do prawej dolnej
    for (int row = 0; row <= ROWS - 4; ++row)
        for (int col = 0; col <= COLS - 4; ++col)
            if (checkLine(board[row][col], board[row + 1][col + 1],
                board[row + 2][col + 2], board[row + 3][col + 3], player))
                return true;
    //ukoœnie z lewej dolnej do prawej górnej
    for (int row = 3; row < ROWS; ++row)
        for (int col = 0; col <= COLS - 4; ++col)
            if (checkLine(board[row][col], board[row - 1][col + 1],
                board[row - 2][col + 2], board[row - 3][col + 3], player))
                return true;

    return false;
}

int main() {
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;   //gniazdo nas³uchuj¹ce
    SOCKET ClientSocket = INVALID_SOCKET;   // gniazdo po zaakceptowaniu klienta

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    clearBoard();

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); //inicjalizacja winsocka
    if (iResult != 0) {                             // jak sie nie uda to konczymy program
        cout << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);     //pobieranie adresu i portu
    if (iResult != 0) {
        cout << "getaddrinfo failed: " << iResult << endl;
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);     //tworzymy nowe gniazdo
    if (ListenSocket == INVALID_SOCKET) {
        cout << "socket failed: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);     // przypisujemy lokalny adres ip i port do gniazda
    if (iResult == SOCKET_ERROR) {
        cout << "bind failed: " << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        cout << "listen failed: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Oczekiwanie na klienta...\n";
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "accept failed: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket);

    while (true) {
        recv(ClientSocket, (char*)board, sizeof(board), 0); //odbieramy tablice od klienta po jego ruchu

        if (checkWin(CLIENT)) {                             //sprawdzamy czy po ruchu klienta jest zwyciêstwo
            printBoard();
            cout << "Klient (X) wygrywa!\n";
            break;
        }

        printBoard();

        int move;                                           //ruch serwera
        cout << "Twoj ruch, wybierz kolumne (0-6): ";
        cin >> move;

        if (!makeMove(move, SERVER)) {
            cout << "Nieprawidlowy ruch, sprobuj ponownie.\n";
            continue;
        }

        send(ClientSocket, (char*)board, sizeof(board), 0);

        if (checkWin(SERVER)) {                             //sprawdzanie wygranej serwera
            printBoard();
            cout << "Serwer (O) wygrywa!\n";
            break;
        }
    }
    
    shutdown(ClientSocket, SD_SEND);                        //konczy wysylac dane do kleinta
    closesocket(ClientSocket);                              //zamyka gniazdo klienta
    WSACleanup();                                           //zwalnia zasoby winsocka
    return 0;
}




