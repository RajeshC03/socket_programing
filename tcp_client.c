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

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Server info
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 4. Loop to send messages
    while (1) {
        printf("\nEnter message to send to server (blank to retry, 'exit' to quit):\n");
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
            send(sockfd, buffer, len, 0);
            printf("\nExit command sent to server\n Server has Shut down!!!!!\n");
            break;
        }

        // Check for blank input
        if (len == 0) {
            printf("\nYou entered a blank message. Please enter a valid message or 'exit' to quit.\n");
            continue;
        }

        // Send message
        send(sockfd, buffer, len, 0);
        printf("Message sent!\n");

        // Receive response from server
        int n = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0'; // Null-terminate received message
            printf("Server: %s\n", buffer);

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

