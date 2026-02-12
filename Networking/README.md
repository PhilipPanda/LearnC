# C Networking

Learn network programming in C using sockets. Build servers, clients, and understand how networked applications communicate.

## What you get

- TCP socket programming (reliable connections)
- UDP socket programming (fast datagrams)
- Client-server architecture
- Multi-client handling
- HTTP protocol basics
- File transfer over networks
- Cross-platform sockets (Windows/Linux)

Everything from scratch. No frameworks, just raw sockets and the standard library.

## Building

Each example is standalone:

```bash
# Windows
gcc 01_simple_server.c -o 01_simple_server.exe -lws2_32
.\01_simple_server.exe

# Linux
gcc 01_simple_server.c -o 01_simple_server
./01_simple_server
```

Or build all at once:
```bash
cd examples
./build_all.bat    # Windows
./build_all.sh     # Linux
```

## Using it

Here's a simple TCP echo server:

```c
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

int main(void) {
    // Initialize sockets (Windows only)
    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    #endif
    
    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Bind to port
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    
    // Listen for connections
    listen(server_fd, 3);
    printf("Server listening on port 8080...\n");
    
    // Accept client
    int client_fd = accept(server_fd, NULL, NULL);
    printf("Client connected!\n");
    
    // Echo loop
    char buffer[1024];
    int bytes = recv(client_fd, buffer, sizeof(buffer), 0);
    send(client_fd, buffer, bytes, 0);
    
    // Cleanup
    #ifdef _WIN32
    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    #else
    close(client_fd);
    close(server_fd);
    #endif
    
    return 0;
}
```

Compile:
```bash
# Windows
gcc server.c -o server.exe -lws2_32

# Linux
gcc server.c -o server
```

## Documentation

- **[Getting Started](docs/GETTING_STARTED.md)** - Socket concepts and first steps
- **[Socket API](docs/SOCKETS.md)** - Complete socket function reference
- **[Protocols](docs/PROTOCOLS.md)** - TCP vs UDP, HTTP basics

## Examples

Each example teaches something specific:

| Example | What It Teaches |
|---------|----------------|
| 01_simple_server | Basic TCP server, accept connections |
| 02_simple_client | TCP client, connect to server |
| 03_echo_server | Echo server with proper message handling |
| 04_chat_server | Multi-client chat with select() |
| 05_udp_sender | UDP basics, sending datagrams |
| 06_udp_receiver | UDP receiver, connectionless communication |
| 07_http_client | HTTP GET requests, parse responses |
| 08_file_transfer | Send and receive files over TCP |

Go in order. Each one builds on previous concepts.

## What this teaches

- How the internet actually works
- Client-server architecture
- Network protocols (TCP, UDP, HTTP)
- Concurrent connections (select, non-blocking)
- Binary data over networks
- Error handling in network code
- Cross-platform networking

After working through this, you'll understand how web servers, chat apps, games, and networked tools communicate.
