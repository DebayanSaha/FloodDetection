#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    int recv_len;

    // Initialize Winsock
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock initialized.\n");

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    // Setup server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Server listening on port %d...\n", SERVER_PORT);

    // Open file for writing
    FILE *file = fopen("received.txt", "wb");
    if (!file) {
        printf("Failed to create file.\n");
        return 1;
    }

    // Receive file chunks
    while (1) {
        recv_len = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0,
                            (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (recv_len == SOCKET_ERROR) {
            printf("recvfrom() failed: %d\n", WSAGetLastError());
            break;
        }

        // Check for EOF marker
        if (recv_len == 3 && strncmp(buffer, "EOF", 3) == 0) {
            break;
        }

        // Write received chunk to file
        fwrite(buffer, 1, recv_len, file);
    }

    fclose(file);
    printf("File received successfully.\n");

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}