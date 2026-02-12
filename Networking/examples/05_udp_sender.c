/*
 * 05_udp_sender.c
 * 
 * UDP sender - sends datagrams to a receiver.
 * UDP is connectionless - no handshake, just send data to an address.
 */

#include <stdio.h>
#include <string.h>

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

#define SERVER_IP "127.0.0.1"
#define PORT 8080
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
    
    printf("=== UDP Sender ===\n");
    
    // Create UDP socket (SOCK_DGRAM instead of SOCK_STREAM)
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket failed");
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Set up destination address
    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    printf("Sending UDP datagrams to %s:%d\n", SERVER_IP, PORT);
    printf("Type messages (or 'quit' to exit):\n\n");
    
    // Send loop
    char buffer[BUFFER_SIZE];
    int message_count = 0;
    
    while (1) {
        // Get input from user
        printf("> ");
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }
        
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        
        if (len == 0) continue;
        
        // Check for quit
        if (strcmp(buffer, "quit") == 0) {
            printf("Exiting...\n");
            break;
        }
        
        // Send datagram (no connection needed!)
        // sendto() specifies destination for each packet
        int bytes_sent = sendto(sock_fd, buffer, len, 0,
                                (struct sockaddr*)&dest_addr, sizeof(dest_addr));
        
        if (bytes_sent < 0) {
            perror("sendto failed");
            break;
        }
        
        message_count++;
        printf("Sent datagram #%d (%d bytes)\n\n", message_count, bytes_sent);
    }
    
    // Cleanup
    CLOSE_SOCKET(sock_fd);
    
    #ifdef _WIN32
    WSACleanup();
    #endif
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * UDP vs TCP:
 * 
 * TCP:
 * - socket() -> connect() -> send/recv() -> close()
 * - Reliable, ordered, connection-based
 * 
 * UDP:
 * - socket() -> sendto() -> close()
 * - No connection, just send to address
 * - No guarantee of delivery or order
 * - Faster, less overhead
 * 
 * Use UDP when:
 * - Speed matters more than reliability
 * - Small messages (under MTU ~1500 bytes)
 * - OK to lose occasional packets
 * - Examples: gaming, live video, DNS queries
 * 
 * Test:
 * 1. Run 06_udp_receiver first
 * 2. Run this sender
 * 3. Type messages and see them appear on receiver
 * 4. Try running multiple senders - all can send to same receiver
 * 
 * Try:
 * - Send to different IP (change SERVER_IP)
 * - Send binary data instead of text
 * - Add sequence numbers to detect lost packets
 * - Implement simple acknowledgment protocol
 * - Send large data in chunks
 */
