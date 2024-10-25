
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("Invalid address");
        close(clientSocket);
        return 1;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        close(clientSocket);
        return 1;
    }

    printf("Connected to the server. Start guessing numbers (1-100):\n");

    char buffer[1024];
    while (1) {
        printf("Enter your guess: ");
        fgets(buffer, sizeof(buffer), stdin);

        send(clientSocket, buffer, strlen(buffer), 0);

        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            printf("Connection lost!\n");
            break;
        }

        printf("Server response: %s\n", buffer);
if (strcmp(buffer, "Угадал") == 0) {
            printf("Congratulations! You guessed it!\n");
            break;
        }
    }

    close(clientSocket);
    return 0;
}
