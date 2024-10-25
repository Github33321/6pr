
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define MAX_CONNECTIONS 5

void handleClient(int clientSocket, struct sockaddr_in clientAddr) {

    int target = rand() % 100 + 1;
    printf("Generated number for client: %d\n", target);

    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {
            printf("%s: Client disconnected.\n", inet_ntoa(clientAddr.sin_addr));
            break;
        }

        int guess = atoi(buffer);
        printf("%s: Guess %d\n", inet_ntoa(clientAddr.sin_addr), guess);

        char response[1024];
        if (guess < target) {
            strcpy(response, "Больше");
        } else if (guess > target) {
            strcpy(response, "Меньше");
        } else {
            strcpy(response, "Угадал");
            send(clientSocket, response, strlen(response), 0);
            printf("%s: %s\n", inet_ntoa(clientAddr.sin_addr), response);
            break;
        }

        send(clientSocket, response, strlen(response), 0);
        printf("%s: %s\n", inet_ntoa(clientAddr.sin_addr), response);
    }

    close(clientSocket);
}

int main() {
    srand(time(0));

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Binding failed");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, MAX_CONNECTIONS) < 0) {
        perror("Listening failed");
        close(serverSocket);
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket < 0) {
            perror("Failed to accept connection");
            continue;
        }

        printf("%s: Connected\n", inet_ntoa(clientAddr.sin_addr));
        handleClient(clientSocket, clientAddr);
    }

    close(serverSocket);
    return 0;
}
