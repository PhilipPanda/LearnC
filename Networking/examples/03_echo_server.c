/*
 * 03_echo_server.c
 * 
 * TCP echo server that handles multiple clients sequentially.
 * More robust than 01_simple_server with better error handling.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define CLOSE_SOCKET close
#endif

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_PENDING 10

// Handle a single client connection
void handle_client(int client_fd, struct sockaddr_in client_addr) {
    char *client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);
    
    printf("\n[Client %s:%d] Connected\n", client_ip, client_port);
    
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        
        // Log message
        printf("[Client %s:%d] Received %d bytes: %s", 
               client_ip, client_port, bytes_received, buffer);
        
        // Echo back
        if (send(client_fd, buffer, bytes_received, 0) < 0) {
            perror("send failed");
            break;
        }
        
        // Check for quit command
        if (strncmp(buffer, "quit", 4) == 0) {
            printf("[Client %s:%d] Requested disconnect\n", client_ip, client_port);
            break;
        }
    }
    
    if (bytes_received == 0) {
        printf("[Client %s:%d] Disconnected\n", client_ip, client_port);
    } else if (bytes_received < 0) {
        perror("recv failed");
    }
    
    CLOSE_SOCKET(client_fd);
}

int main(void) {
    // Initialize Winsock (Windows)
    #ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
    #endif
    
    printf("=== Enhanced Echo Server ===\n");
    
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Allow address reuse
    int opt = 1;
    #ifdef _WIN32
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    #else
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    #endif
    
    // Set up address
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        CLOSE_SOCKET(server_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Listen
    if (listen(server_fd, MAX_PENDING) < 0) {
        perror("listen failed");
        CLOSE_SOCKET(server_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    printf("Server listening on port %d\n", PORT);
    printf("Waiting for clients... (Ctrl+C to stop)\n");
    
    // Accept loop - handle clients sequentially
    while (1) {
        struct sockaddr_in client_addr;
        #ifdef _WIN32
        int addrlen = sizeof(client_addr);
        #else
        socklen_t addrlen = sizeof(client_addr);
        #endif
        
        // Accept new client (blocks until client connects)
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            continue;  // Try to accept next client
        }
        
        // Handle this client (blocks until client disconnects)
        handle_client(client_fd, client_addr);
    }
    
    // Cleanup (never reached in this version, but good practice)
    CLOSE_SOCKET(server_fd);
    
    #ifdef _WIN32
    WSACleanup();
    #endif
    
    return 0;
}

/*
 * Improvements over 01_simple_server:
 * 
 * - Handles multiple clients (sequentially)
 * - Better logging with client IP and port
 * - Proper error handling
 * - Continues running after client disconnects
 * - More robust message handling
 * 
 * Limitation:
 * - Only handles ONE client at a time
 * - Other clients must wait until current client disconnects
 * 
 * Test:
 * 1. Run this server
 * 2. Connect with multiple clients (02_simple_client or telnet)
 * 3. Notice second client waits for first to disconnect
 * 
 * Next step:
 * See 04_chat_server for handling multiple clients simultaneously
 * using select() to multiplex connections.
 */
