/*
 * 08_file_transfer.c
 * 
 * File transfer server and client in one program.
 * Run as server to receive files, or as client to send files.
 * Demonstrates binary data transfer over TCP.
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
#define BUFFER_SIZE 4096

// File transfer protocol:
// [8 bytes: filename length][N bytes: filename][8 bytes: file size][file data]

void run_server(void) {
    printf("=== File Transfer Server ===\n");
    
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return;
    }
    
    // Allow address reuse
    int opt = 1;
    #ifdef _WIN32
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    #else
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    #endif
    
    // Bind
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        CLOSE_SOCKET(server_fd);
        return;
    }
    
    // Listen
    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        CLOSE_SOCKET(server_fd);
        return;
    }
    
    printf("Server listening on port %d\n", PORT);
    printf("Waiting for file transfer...\n\n");
    
    // Accept client
    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept failed");
        CLOSE_SOCKET(server_fd);
        return;
    }
    
    printf("Client connected!\n");
    
    // Receive filename length
    long long filename_len;
    recv(client_fd, (char*)&filename_len, sizeof(filename_len), 0);
    
    if (filename_len <= 0 || filename_len > 255) {
        printf("Invalid filename length\n");
        CLOSE_SOCKET(client_fd);
        CLOSE_SOCKET(server_fd);
        return;
    }
    
    // Receive filename
    char filename[256] = {0};
    recv(client_fd, filename, filename_len, 0);
    printf("Receiving file: %s\n", filename);
    
    // Receive file size
    long long file_size;
    recv(client_fd, (char*)&file_size, sizeof(file_size), 0);
    printf("File size: %lld bytes\n", file_size);
    
    // Open file for writing
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to create file");
        CLOSE_SOCKET(client_fd);
        CLOSE_SOCKET(server_fd);
        return;
    }
    
    // Receive file data
    char buffer[BUFFER_SIZE];
    long long received = 0;
    
    while (received < file_size) {
        int to_receive = (file_size - received < BUFFER_SIZE) ? 
                        (file_size - received) : BUFFER_SIZE;
        
        int bytes = recv(client_fd, buffer, to_receive, 0);
        if (bytes <= 0) break;
        
        fwrite(buffer, 1, bytes, file);
        received += bytes;
        
        // Progress indicator
        int percent = (int)((received * 100) / file_size);
        printf("\rProgress: %d%% (%lld / %lld bytes)", percent, received, file_size);
        fflush(stdout);
    }
    
    printf("\n\nFile received successfully!\n");
    
    fclose(file);
    CLOSE_SOCKET(client_fd);
    CLOSE_SOCKET(server_fd);
}

void run_client(const char *server_ip, const char *filepath) {
    printf("=== File Transfer Client ===\n");
    
    // Open file
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("Failed to open file");
        return;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Extract filename from path
    const char *filename = strrchr(filepath, '/');
    if (!filename) filename = strrchr(filepath, '\\');
    filename = filename ? filename + 1 : filepath;
    
    printf("Sending file: %s\n", filename);
    printf("File size: %lld bytes\n", file_size);
    
    // Create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket failed");
        fclose(file);
        return;
    }
    
    // Connect to server
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(server_ip);
    
    printf("Connecting to %s:%d...\n", server_ip, PORT);
    if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Connection failed");
        CLOSE_SOCKET(sock_fd);
        fclose(file);
        return;
    }
    
    printf("Connected!\n\n");
    
    // Send filename length
    long long filename_len = strlen(filename);
    send(sock_fd, (char*)&filename_len, sizeof(filename_len), 0);
    
    // Send filename
    send(sock_fd, filename, filename_len, 0);
    
    // Send file size
    send(sock_fd, (char*)&file_size, sizeof(file_size), 0);
    
    // Send file data
    char buffer[BUFFER_SIZE];
    long long sent = 0;
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(sock_fd, buffer, bytes_read, 0);
        sent += bytes_read;
        
        // Progress indicator
        int percent = (int)((sent * 100) / file_size);
        printf("\rProgress: %d%% (%lld / %lld bytes)", percent, sent, file_size);
        fflush(stdout);
    }
    
    printf("\n\nFile sent successfully!\n");
    
    fclose(file);
    CLOSE_SOCKET(sock_fd);
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
    
    if (argc < 2) {
        printf("Usage:\n");
        printf("  Server mode: %s server\n", argv[0]);
        printf("  Client mode: %s client <server_ip> <filepath>\n", argv[0]);
        printf("\nExample:\n");
        printf("  Server: %s server\n", argv[0]);
        printf("  Client: %s client 127.0.0.1 test.txt\n", argv[0]);
        #ifdef _WIN32
        WSACleanup();
        #endif
        return 1;
    }
    
    if (strcmp(argv[1], "server") == 0) {
        run_server();
    } else if (strcmp(argv[1], "client") == 0 && argc >= 4) {
        run_client(argv[2], argv[3]);
    } else {
        printf("Invalid arguments. See usage above.\n");
    }
    
    #ifdef _WIN32
    WSACleanup();
    #endif
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Binary Protocol Design:
 * 
 * Sending arbitrary data over network requires a protocol.
 * We use length-prefixed fields:
 * 
 * 1. Filename length (8 bytes)
 * 2. Filename (N bytes)
 * 3. File size (8 bytes)
 * 4. File data (size bytes)
 * 
 * This way receiver knows exactly how much data to expect.
 * 
 * Test:
 * 1. Create a test file: echo "Hello from file!" > test.txt
 * 2. Terminal 1: 08_file_transfer server
 * 3. Terminal 2: 08_file_transfer client 127.0.0.1 test.txt
 * 4. Server receives file with same name
 * 
 * Try:
 * - Transfer large files (images, videos)
 * - Add checksum/hash to verify integrity
 * - Implement resume capability (transfer partial files)
 * - Add compression (zlib)
 * - Multiple files in sequence
 * - Bidirectional transfer
 * - Add encryption
 * 
 * Important:
 * - Always use binary mode ("rb"/"wb") for files
 * - Handle partial sends/receives (loop until complete)
 * - Consider byte order for multi-byte integers
 * - Add error checking and recovery
 */
