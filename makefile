# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Targets
TARGETS = tcp_client tcp_server

# Default target
all: $(TARGETS)

# Build tcp_client
tcp_client: tcp_client.c
	$(CC) $(CFLAGS) -o tcp_client tcp_client.c

# Build tcp_server
tcp_server: tcp_server.c
	$(CC) $(CFLAGS) -o tcp_server tcp_server.c

# Clean up build artifacts
clean:
	rm -f $(TARGETS)




























