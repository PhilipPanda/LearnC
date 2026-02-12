/*
 * 06_udp_receiver.c
 * 
 * UDP receiver - listens for datagrams from any sender.
 * No connection needed - just bind to port and receive.
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
    
    printf("=== UDP Receiver ===\n");
    
    // Create UDP socket
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket failed");
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Allow address reuse
    int opt = 1;
    #ifdef _WIN32
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    #else
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    #endif
    
    // Bind to port (receive datagrams sent to this port)
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;  // Accept from any interface
    local_addr.sin_port = htons(PORT);
    
    if (bind(sock_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind failed");
        CLOSE_SOCKET(sock_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    printf("Listening for UDP datagrams on port %d...\n", PORT);
    printf("Press Ctrl+C to stop\n\n");
    
    // Receive loop
    char buffer[BUFFER_SIZE];
    int datagram_count = 0;
    
    while (1) {
        struct sockaddr_in sender_addr;
        #ifdef _WIN32
        int addr_len = sizeof(sender_addr);
        #else
        socklen_t addr_len = sizeof(sender_addr);
        #endif
        
        // Receive datagram
        // recvfrom() tells us who sent it (fills sender_addr)
        int bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE - 1, 0,
                            (struct sockaddr*)&sender_addr, &addr_len);
        
        if (bytes < 0) {
            perror("recvfrom failed");
            break;
        }
        
        // Null-terminate and print
        buffer[bytes] = '\0';
        datagram_count++;
        
        char *sender_ip = inet_ntoa(sender_addr.sin_addr);
        int sender_port = ntohs(sender_addr.sin_port);
        
        printf("[Datagram #%d from %s:%d] %s\n",
               datagram_count, sender_ip, sender_port, buffer);
    }
    
    // Cleanup
    CLOSE_SOCKET(sock_fd);
    
    #ifdef _WIN32
    WSACleanup();
    #endif
    
    return 0;
}

/*
 * UDP characteristics:
 * 
 * Connectionless:
 * - No accept() or connect()
 * - Just bind to port and receive
 * - Any sender can send to this port
 * 
 * Unreliable:
 * - Packets can be lost (network congestion, errors)
 * - Packets can arrive out of order
 * - No automatic retransmission
 * - Application must handle reliability if needed
 * 
 * Fast:
 * - No connection overhead
 * - No acknowledgment waiting
 * - Minimal protocol overhead (8 byte header)
 * 
 * Test:
 * 1. Run this receiver
 * 2. Run 05_udp_sender (can run multiple!)
 * 3. All senders can send to this receiver simultaneously
 * 4. Try running multiple receivers on different ports
 * 
 * Experiment:
 * - Send rapid messages and see if any drop
 * - Monitor with Wireshark to see raw UDP packets
 * - Compare packet loss with different message sizes
 * - Send from different machines on network
 * 
 * Try:
 * - Echo datagrams back to sender (sendto with sender_addr)
 * - Track which senders are active
 * - Add sequence numbers to detect lost packets
 * - Implement request-response pattern
 * - Broadcast to multiple receivers (multicast)
 */
