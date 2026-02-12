/*
 * 01_simple_server.c
 * 
 * Basic TCP server that accepts one client and echoes back what it receives.
 */

#include <stdio.h>
#include <string.h>

// Platform-specific includes
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

int main(void) {
    // Initialize Winsock (Windows only)
    #ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
    #endif
    
    printf("=== Simple TCP Echo Server ===\n");
    
    // 1. Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Set socket option to reuse address (avoid "Address already in use")
    int opt = 1;
    #ifdef _WIN32
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    #else
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    #endif
    
    // 2. Set up address structure
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces (0.0.0.0)
    address.sin_port = htons(PORT);        // Port 8080 (convert to network byte order)
    
    // 3. Bind socket to address/port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        CLOSE_SOCKET(server_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // 4. Start listening for connections (queue up to 3)
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        CLOSE_SOCKET(server_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    printf("Server listening on port %d...\n", PORT);
    printf("Waiting for client connection...\n");
    
    // 5. Accept incoming connection
    struct sockaddr_in client_addr;
    #ifdef _WIN32
    int addrlen = sizeof(client_addr);
    #else
    socklen_t addrlen = sizeof(client_addr);
    #endif
    
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
    if (client_fd < 0) {
        perror("accept failed");
        CLOSE_SOCKET(server_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Print client info
    char *client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);
    printf("Client connected from %s:%d\n", client_ip, client_port);
    
    // 6. Echo loop - receive and send back
    char buffer[BUFFER_SIZE];
    while (1) {
        // Clear buffer
        memset(buffer, 0, BUFFER_SIZE);
        
        // Receive data from client
        int bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received > 0) {
            // Null-terminate and print
            buffer[bytes_received] = '\0';
            printf("Received: %s", buffer);
            
            // Echo back to client
            send(client_fd, buffer, bytes_received, 0);
            
            // Exit if client sends "quit"
            if (strncmp(buffer, "quit", 4) == 0) {
                printf("Client requested quit\n");
                break;
            }
        } else if (bytes_received == 0) {
            // Connection closed by client
            printf("Client disconnected\n");
            break;
        } else {
            // Error
            perror("recv failed");
            break;
        }
    }
    
    // 7. Cleanup
    printf("Closing connection...\n");
    CLOSE_SOCKET(client_fd);
    CLOSE_SOCKET(server_fd);
    
    #ifdef _WIN32
    WSACleanup();
    #endif
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * How to test:
 * 
 * 1. Run this server
 * 2. In another terminal, use telnet or nc:
 *    - Windows: telnet localhost 8080
 *    - Linux:   nc localhost 8080
 * 3. Type messages and see them echoed back
 * 4. Type "quit" to disconnect
 * 
 * Or compile and run 02_simple_client.c
 * 
 * What's happening:
 * - Server creates socket and binds to port 8080
 * - Waits for client to connect (accept blocks)
 * - Once connected, reads data and echoes it back
 * - Closes when client sends "quit" or disconnects
 * 
 * Try:
 * - Change PORT to different number
 * - Connect with a web browser (type http://localhost:8080)
 * - Run multiple clients (only one will connect, others queue)
 * - Add message logging to file
 */
