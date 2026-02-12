/*
 * 07_http_client.c
 * 
 * Simple HTTP client that sends GET requests and prints responses.
 * Demonstrates HTTP protocol basics.
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
    #include <netdb.h>
    #define CLOSE_SOCKET close
#endif

#define BUFFER_SIZE 4096

// Parse URL into host and path
void parse_url(const char *url, char *host, char *path) {
    // Skip http:// if present
    const char *start = url;
    if (strncmp(url, "http://", 7) == 0) {
        start = url + 7;
    }
    
    // Find first slash (path separator)
    const char *slash = strchr(start, '/');
    
    if (slash) {
        // Copy host
        size_t host_len = slash - start;
        strncpy(host, start, host_len);
        host[host_len] = '\0';
        
        // Copy path
        strcpy(path, slash);
    } else {
        // No path, just host
        strcpy(host, start);
        strcpy(path, "/");
    }
}

int main(int argc, char *argv[]) {
    // Initialize Winsock (Windows)
    #ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
    #endif
    
    printf("=== Simple HTTP Client ===\n");
    
    // Default URL if none provided
    const char *url = (argc > 1) ? argv[1] : "http://example.com/";
    
    // Parse URL
    char host[256] = {0};
    char path[256] = {0};
    parse_url(url, host, path);
    
    printf("Fetching: %s\n", url);
    printf("Host: %s\n", host);
    printf("Path: %s\n\n", path);
    
    // Create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket failed");
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Resolve hostname to IP address
    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr, "DNS lookup failed for %s\n", host);
        CLOSE_SOCKET(sock_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Set up server address
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);  // HTTP port
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    char *ip = inet_ntoa(server_addr.sin_addr);
    printf("Connecting to %s (%s)...\n", host, ip);
    
    // Connect to server
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        CLOSE_SOCKET(sock_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    printf("Connected!\n\n");
    
    // Build HTTP GET request
    char request[1024];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: SimpleHTTPClient/1.0\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, host);
    
    printf("Sending request:\n");
    printf("----------------------------------------\n");
    printf("%s", request);
    printf("----------------------------------------\n\n");
    
    // Send request
    if (send(sock_fd, request, strlen(request), 0) < 0) {
        perror("send failed");
        CLOSE_SOCKET(sock_fd);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    // Receive response
    printf("Response:\n");
    printf("========================================\n");
    
    char buffer[BUFFER_SIZE];
    int total_received = 0;
    int bytes;
    
    while ((bytes = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
        total_received += bytes;
    }
    
    printf("\n========================================\n");
    printf("Total received: %d bytes\n", total_received);
    
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
 * HTTP Request Format:
 * 
 * GET /path HTTP/1.1\r\n
 * Host: example.com\r\n
 * Header: Value\r\n
 * \r\n
 * 
 * Key points:
 * - Lines end with \r\n (CRLF), not just \n
 * - Blank line (\r\n\r\n) separates headers from body
 * - Host header is required in HTTP/1.1
 * - Connection: close tells server to close after response
 * 
 * HTTP Response Format:
 * 
 * HTTP/1.1 200 OK\r\n
 * Content-Type: text/html\r\n
 * Content-Length: 1234\r\n
 * \r\n
 * <body content>
 * 
 * Status codes:
 * - 200 OK - Success
 * - 301/302 - Redirect (check Location header)
 * - 404 - Not Found
 * - 500 - Server Error
 * 
 * Test:
 * 1. Run: 07_http_client http://example.com
 * 2. Run: 07_http_client http://example.com/page.html
 * 3. Try different websites
 * 
 * Try:
 * - Parse response headers separately from body
 * - Extract status code (200, 404, etc.)
 * - Follow redirects (301/302 with Location header)
 * - Save response to file
 * - Add more headers (Accept, User-Agent, etc.)
 * - Implement POST requests with body data
 * - Add timeout handling
 * 
 * Limitations:
 * - No HTTPS support (would need SSL/TLS library)
 * - Doesn't follow redirects
 * - Basic error handling
 * - No chunked transfer encoding support
 */
