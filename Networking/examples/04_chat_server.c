/*
 * 04_chat_server.c
 * 
 * Multi-client chat server using select() to handle multiple connections.
 * Clients can send messages that are broadcast to all other clients.
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
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main(void) {
    // Initialize Winsock (Windows)
    #ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
    #endif
    
    printf("=== Multi-Client Chat Server ===\n");
    
    // Create server socket
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
    
    // Bind to port
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        CLOSE_SOCKET(server_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Start listening
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen failed");
        CLOSE_SOCKET(server_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    printf("Chat server listening on port %d\n", PORT);
    printf("Waiting for clients... (Ctrl+C to stop)\n\n");
    
    // Array to track client sockets
    int client_sockets[MAX_CLIENTS] = {0};
    
    char buffer[BUFFER_SIZE];
    
    // Main server loop
    while (1) {
        // Set up file descriptor set for select()
        fd_set readfds;
        FD_ZERO(&readfds);
        
        // Add server socket to set
        FD_SET(server_fd, &readfds);
        int max_fd = server_fd;
        
        // Add all active client sockets to set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            
            if (sd > max_fd) {
                max_fd = sd;
            }
        }
        
        // Wait for activity on any socket (blocking call)
        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        
        if (activity < 0) {
            perror("select error");
            break;
        }
        
        // Check if there's a new connection
        if (FD_ISSET(server_fd, &readfds)) {
            struct sockaddr_in client_addr;
            #ifdef _WIN32
            int addrlen = sizeof(client_addr);
            #else
            socklen_t addrlen = sizeof(client_addr);
            #endif
            
            int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
            if (new_socket < 0) {
                perror("accept failed");
                continue;
            }
            
            // Find empty slot for new client
            int slot_found = 0;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("[Server] New client connected (slot %d) from %s:%d\n",
                           i, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    
                    // Send welcome message
                    char *welcome = "Welcome to the chat! Type your messages.\n";
                    send(new_socket, welcome, strlen(welcome), 0);
                    
                    slot_found = 1;
                    break;
                }
            }
            
            if (!slot_found) {
                char *msg = "Server full. Try again later.\n";
                send(new_socket, msg, strlen(msg), 0);
                CLOSE_SOCKET(new_socket);
                printf("[Server] Connection rejected (server full)\n");
            }
        }
        
        // Check all clients for incoming data
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            
            if (sd == 0) continue;
            
            if (FD_ISSET(sd, &readfds)) {
                // Receive data from client
                memset(buffer, 0, BUFFER_SIZE);
                int bytes = recv(sd, buffer, BUFFER_SIZE - 1, 0);
                
                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    printf("[Client %d] %s", i, buffer);
                    
                    // Broadcast message to all OTHER clients
                    char broadcast[BUFFER_SIZE + 50];
                    snprintf(broadcast, sizeof(broadcast), "[Client %d]: %s", i, buffer);
                    
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_sockets[j] != 0 && j != i) {
                            send(client_sockets[j], broadcast, strlen(broadcast), 0);
                        }
                    }
                } else {
                    // Client disconnected
                    struct sockaddr_in addr;
                    #ifdef _WIN32
                    int addr_len = sizeof(addr);
                    #else
                    socklen_t addr_len = sizeof(addr);
                    #endif
                    getpeername(sd, (struct sockaddr*)&addr, &addr_len);
                    
                    printf("[Server] Client %d disconnected (%s:%d)\n",
                           i, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                    
                    CLOSE_SOCKET(sd);
                    client_sockets[i] = 0;
                }
            }
        }
    }
    
    // Cleanup
    CLOSE_SOCKET(server_fd);
    
    #ifdef _WIN32
    WSACleanup();
    #endif
    
    return 0;
}

/*
 * How select() works:
 * 
 * select() monitors multiple file descriptors (sockets) at once.
 * It blocks until at least one has activity (data to read, connection, etc.)
 * 
 * This lets us handle many clients with a single thread!
 * 
 * Steps:
 * 1. Create fd_set with FD_ZERO()
 * 2. Add sockets with FD_SET()
 * 3. Call select() - blocks until activity
 * 4. Check which sockets are ready with FD_ISSET()
 * 5. Handle each ready socket
 * 6. Repeat
 * 
 * Test:
 * 1. Run this server
 * 2. Connect with multiple clients (02_simple_client or telnet)
 * 3. Messages from one client appear on all others
 * 
 * Try:
 * - Add usernames (client sends name on connect)
 * - Add private messages (/msg <user> <text>)
 * - Add commands (/list to see users, /quit to disconnect)
 * - Save chat history to file
 * - Add timestamps to messages
 */
