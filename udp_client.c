#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 49152

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr;
    socklen_t addr_len = sizeof(servaddr);

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Server info
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Loop to send messages
    while (1) {
        printf("Enter message to send to server (blank to retry, 'exit' to quit): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("Error reading input. Exiting.\n");
            break;
        }

        // Remove trailing newline character, if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        // Check for 'exit' command
        if (strcmp(buffer, "exit") == 0) {
            sendto(sockfd, buffer, len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
            printf("Exit command sent to server.\n");
        }

        // Check for blank input
        if (len == 0) {
            printf("You entered a Blank message. Please enter a valid message or 'exit' to quit.\n");
            continue;
        }

        // Send message
        sendto(sockfd, buffer, len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        // Receive response from server
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&servaddr, &addr_len);
        if (n > 0) {
            buffer[n] = '\0'; // Null-terminate received message
            //printf("Server: %s\n", buffer);

            // If server is shutting down
            if (strcmp(buffer, "Server shutting down") == 0) {
                printf("Server has shut down. Exiting client.\n");
                break;
            }
        } else {
            printf("No response received from server.\n");
        }
    }

    close(sockfd);
    return 0;
}

