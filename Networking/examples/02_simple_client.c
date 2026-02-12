/*
 * 02_simple_client.c
 * 
 * Basic TCP client that connects to a server and sends messages.
 */

#include <stdio.h>
#include <string.h>

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>  // For inet_pton
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define CLOSE_SOCKET close
#endif

#define SERVER_IP "127.0.0.1"  // localhost
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
    
    printf("=== Simple TCP Client ===\n");
    
    // 1. Create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket failed");
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // 2. Set up server address
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;           // IPv4
    server_addr.sin_port = htons(PORT);         // Port (convert to network byte order)
    
    // Convert IP string to binary form
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        printf("Invalid address: %s\n", SERVER_IP);
        CLOSE_SOCKET(sock_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // 3. Connect to server
    printf("Connecting to %s:%d...\n", SERVER_IP, PORT);
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        printf("Make sure the server is running (01_simple_server)\n");
        CLOSE_SOCKET(sock_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    printf("Connected to server!\n");
    printf("Type messages (or 'quit' to exit):\n\n");
    
    // 4. Communication loop
    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];
    
    while (1) {
        // Get input from user
        printf("> ");
        if (fgets(send_buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        
        // Remove newline if present
        size_t len = strlen(send_buffer);
        if (len > 0 && send_buffer[len - 1] == '\n') {
            send_buffer[len - 1] = '\0';
            len--;
        }
        
        // Skip empty lines
        if (len == 0) continue;
        
        // Send message to server
        if (send(sock_fd, send_buffer, len, 0) < 0) {
            perror("send failed");
            break;
        }
        
        // Receive echo from server
        memset(recv_buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock_fd, recv_buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received > 0) {
            recv_buffer[bytes_received] = '\0';
            printf("Server echo: %s\n\n", recv_buffer);
            
            // Check if we sent quit
            if (strcmp(send_buffer, "quit") == 0) {
                printf("Exiting...\n");
                break;
            }
        } else if (bytes_received == 0) {
            printf("Server closed connection\n");
            break;
        } else {
            perror("recv failed");
            break;
        }
    }
    
    // 5. Cleanup
    printf("Closing connection...\n");
    CLOSE_SOCKET(sock_fd);
    
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
 * 1. First run 01_simple_server
 * 2. Then run this client
 * 3. Type messages and see them echoed back
 * 4. Type "quit" to disconnect
 * 
 * What's happening:
 * - Client creates socket
 * - Connects to server at 127.0.0.1:8080
 * - Sends user input to server
 * - Receives and prints server's response
 * - Closes when user types "quit"
 * 
 * Try:
 * - Change SERVER_IP to another machine's IP
 * - Run multiple clients connecting to same server
 * - Add timestamps to messages
 * - Send binary data instead of text
 * - Add automatic reconnection if server disconnects
 */
