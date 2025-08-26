// Save as client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    int serverAddrLen = sizeof(serverAddr);
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }

    // Get server IP from user
    char serverIP[50];
    printf("Enter server IP: ");
    scanf("%s", serverIP);

    // Setup server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    // Get filename from user
    char filename[200];
    printf("Enter filename (with path if not in current folder): ");
    scanf("%s", filename);
    

    // Open file
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        perror("Error reason"); // Shows detailed system error
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Send file in chunks
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (sendto(clientSocket, buffer, bytesRead, 0,
                   (struct sockaddr *)&serverAddr, serverAddrLen) == SOCKET_ERROR) {
            printf("sendto() failed: %d\n", WSAGetLastError());
            fclose(file);
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    }

    // Send EOF marker
    strcpy(buffer, "EOF");
    sendto(clientSocket, buffer, strlen(buffer), 0,
           (struct sockaddr *)&serverAddr, serverAddrLen);

    fclose(file);
    printf("File sent successfully: %s\n", filename);

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
