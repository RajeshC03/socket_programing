#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <time.h>


#define PORT 49152

int main() {
    int server_fd, new_socket;
    char buffer[1024];
    struct sockaddr_in servaddr, cliaddr;
    socklen_t addr_len = sizeof(cliaddr);



    // 1. Create TCP socket

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }




    // 2. Fill server info

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);




    // 3. Bind socket with server address

    if (bind(server_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }


    // 4. Listen for incoming connections

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("TCP server is listening on port %d...\n", PORT);


    // 5. Accept incoming connection from client

    new_socket = accept(server_fd, (struct sockaddr *)&cliaddr, &addr_len);
    if (new_socket < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected.\n");


    // 6. Loop to receive messages continuously
    while (1) {
        memset(buffer, 0, sizeof(buffer));  // Clear buffer before each recv

        int n = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        if (n < 0) {
            perror("recv failed");
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
            send(new_socket, "Server shutting down", strlen("Server shutting down"), 0);
            break;
        }

        if (strlen(buffer) == 0) {
            printf("No message received.\n");
            send(new_socket, "No message received", strlen("No message received"), 0);
        } else {
            printf("***Client Message: %s\n", buffer);
            send(new_socket, buffer, strlen(buffer), 0);
        }

        printf("\n...Waiting for next message...\n");

    // Handle client messages
    while (1) {
        memset(buffer, 0, sizeof(buffer));

        int n = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("Connection closed or error.\n");
            break;
        }

        buffer[n] = '\0';

        // Remove newline
        if (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = '\0';

        printf("Client Message: %s\n", buffer);

        // Respond based on message
        if (strcmp(buffer, "exit") == 0) {
            char *msg = "Server shutting down";
            send(new_socket, msg, strlen(msg), 0);
            printf("!!!!!!!Shutting down server!!!!!!!\n");
            break;
        } else if (strcmp(buffer, "hello") == 0) {
            char *msg = "Hi there!";
            send(new_socket, msg, strlen(msg), 0);
        } else if (strcmp(buffer, "time") == 0) {
            time_t now = time(NULL);
            char *time_str = ctime(&now);
            send(new_socket, time_str, strlen(time_str), 0);
        } else {
            char response[1050];
            snprintf(response, sizeof(response), "Server received: %s", buffer);
            send(new_socket, response, strlen(response), 0);
        }

        printf("...Waiting for next message...\n");

    }

    close(new_socket);
    close(server_fd);
    return 0;
}

