#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 49152

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;

    // 1. Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Fill server info
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // 3. Bind socket with server address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server is listening on port %d...\n", PORT);

    // 4. Loop to receive messages continuously
    while (1) {
        len = sizeof(cliaddr);
        memset(buffer, 0, sizeof(buffer));  // Clear buffer before each recv

        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&cliaddr, &len);

        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0'; // Null terminate

        // Trim newline if present
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }

        // Check for exit command
        if (strcmp(buffer, "exit") == 0) {
            printf("!!!!!!!Shutting down server!!!!!!!\n");
            sendto(sockfd, "Server shutting down", strlen("Server shutting down"), 0,
                   (struct sockaddr *)&cliaddr, len);
            break;
        }s

        if (strlen(buffer) == 0) {
            printf("No message received.\n");
            sendto(sockfd, "No message received", strlen("No message received"), 0,
                   (struct sockaddr *)&cliaddr, len);
        } else {
            printf("***Client Message: %s\n", buffer);
            sendto(sockfd, buffer, strlen(buffer), 0,
                   (struct sockaddr *)&cliaddr, len);
        }

        printf("\n...Waiting for next message...\n");
    }

    close(sockfd);
    return 0;
}

