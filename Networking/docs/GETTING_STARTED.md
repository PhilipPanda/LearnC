# Getting Started with Network Programming

## The Big Picture

Network programming lets programs on different computers talk to each other. You create **sockets** - endpoints for sending and receiving data over a network.

Think of a socket like a phone line. One program "calls" another, they connect, and then they can talk. When done, they hang up.

## What is a Socket?

A socket is a file descriptor (just a number) that represents a network connection. You read and write to it like a file, but the data goes over the network.

```
[Your Program] <--socket--> [Network] <--socket--> [Other Program]
```

## TCP vs UDP

Two main protocols:

### TCP (Transmission Control Protocol)
- **Reliable**: Guarantees data arrives in order
- **Connection-based**: Must connect before sending
- **Slower**: Extra overhead for reliability
- **Use for**: HTTP, file transfers, chat, anything that can't lose data

Think: Phone call - you dial, connect, talk, hang up.

### UDP (User Datagram Protocol)  
- **Unreliable**: Packets might get lost or arrive out of order
- **Connectionless**: Just send data to an address
- **Faster**: No connection overhead
- **Use for**: Gaming, video streaming, DNS, anything where speed > reliability

Think: Shouting across a room - might not hear every word, but it's immediate.

## Client-Server Model

Most networked apps use this pattern:

**Server:**
1. Create socket
2. Bind to a port (like a phone number)
3. Listen for connections
4. Accept clients
5. Receive/send data
6. Close connection

**Client:**
1. Create socket
2. Connect to server (IP + port)
3. Send/receive data
4. Close connection

```
SERVER                          CLIENT
------                          ------
socket()                        socket()
bind(port 8080)
listen()
                                connect(IP:8080)
accept() ------------->
         <------------- connection established
recv() <---------------         send("hello")
send("echo: hello") --->        recv()
close()                         close()
```

## IP Addresses and Ports

Every computer on a network has an **IP address** (like 192.168.1.100 or google.com).

Each program uses a **port** (0-65535) to distinguish different services on the same computer:
- Port 80: HTTP (web)
- Port 443: HTTPS (secure web)
- Port 22: SSH
- Port 8080: Common development port

Think of IP as a street address, port as an apartment number.

## Socket API Functions

### Core Functions

**socket()** - Create a socket
```c
int socket(int domain, int type, int protocol);
// domain: AF_INET (IPv4)
// type: SOCK_STREAM (TCP) or SOCK_DGRAM (UDP)
// protocol: 0 (auto)
```

**bind()** - Assign address/port to socket (server only)
```c
int bind(int sockfd, struct sockaddr *addr, socklen_t len);
```

**listen()** - Mark socket as passive (server only)
```c
int listen(int sockfd, int backlog);
// backlog: max queued connections
```

**accept()** - Accept incoming connection (server only)
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *len);
// Returns new socket for this client
```

**connect()** - Connect to server (client only)
```c
int connect(int sockfd, struct sockaddr *addr, socklen_t len);
```

**send() / recv()** - Send/receive data
```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

**close()** - Close socket (closesocket() on Windows)
```c
int close(int sockfd);
```

## Platform Differences

### Windows (Winsock)
- Must call `WSAStartup()` before using sockets
- Use `closesocket()` instead of `close()`
- Link with `-lws2_32`
- Include `<winsock2.h>`

### Linux
- Sockets work out of the box
- Use standard `close()`
- Include `<sys/socket.h>`, `<netinet/in.h>`, etc.

## Common Structures

**sockaddr_in** - IPv4 address structure
```c
struct sockaddr_in {
    short sin_family;        // AF_INET
    unsigned short sin_port; // Port (use htons())
    struct in_addr sin_addr; // IP address
    char sin_zero[8];        // Padding
};
```

**Byte order functions:**
- `htons()` - Host to network short (port numbers)
- `htonl()` - Host to network long (IP addresses)
- `ntohs()` - Network to host short
- `ntohl()` - Network to host long

Networks use **big-endian** (most significant byte first). Your CPU might use little-endian. These functions convert.

## Your First Server

```c
// 1. Create socket
int server_fd = socket(AF_INET, SOCK_STREAM, 0);

// 2. Set up address
struct sockaddr_in addr = {0};
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = INADDR_ANY;  // Any interface
addr.sin_port = htons(8080);         // Port 8080

// 3. Bind socket to port
bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));

// 4. Start listening
listen(server_fd, 5);  // Queue up to 5 connections

// 5. Accept client
int client_fd = accept(server_fd, NULL, NULL);

// 6. Communicate
char buf[1024];
int bytes = recv(client_fd, buf, sizeof(buf), 0);
send(client_fd, "OK", 2, 0);

// 7. Clean up
close(client_fd);
close(server_fd);
```

## Your First Client

```c
// 1. Create socket
int sock = socket(AF_INET, SOCK_STREAM, 0);

// 2. Set up server address
struct sockaddr_in addr = {0};
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // localhost
addr.sin_port = htons(8080);

// 3. Connect to server
connect(sock, (struct sockaddr*)&addr, sizeof(addr));

// 4. Communicate
send(sock, "Hello", 5, 0);
char buf[1024];
recv(sock, buf, sizeof(buf), 0);

// 5. Clean up
close(sock);
```

## Testing

**Run server:**
```bash
./01_simple_server
# Server listening on port 8080...
```

**In another terminal, run client:**
```bash
./02_simple_client
# Connected to server!
# Received: echo message
```

You can also use `telnet` or `nc` (netcat) to connect manually:
```bash
telnet localhost 8080
nc localhost 8080
```

## Common Mistakes

**1. Forgetting WSAStartup() on Windows**
```c
// WRONG on Windows
int sock = socket(...);

// RIGHT on Windows
WSADATA wsa;
WSAStartup(MAKEWORD(2,2), &wsa);
int sock = socket(...);
```

**2. Not checking return values**
```c
// WRONG
int sock = socket(AF_INET, SOCK_STREAM, 0);
bind(sock, ...);  // What if socket() failed?

// RIGHT
int sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock < 0) {
    perror("socket failed");
    return 1;
}
```

**3. Forgetting byte order conversion**
```c
// WRONG
addr.sin_port = 8080;  // Will connect to port 36895!

// RIGHT
addr.sin_port = htons(8080);  // Convert to network byte order
```

**4. Not handling partial sends/receives**
```c
// TCP might not send all data at once
send(sock, big_buffer, 10000, 0);  // Might only send 5000 bytes!

// Need to loop:
int sent = 0;
while (sent < total) {
    int n = send(sock, buffer + sent, total - sent, 0);
    if (n < 0) break;  // Error
    sent += n;
}
```

## Next Steps

1. Read the [Socket API](SOCKETS.md) documentation
2. Work through examples in order
3. Experiment - change ports, messages, add features
4. Use Wireshark to see actual network packets
5. Build something real - chat app, file server, game

Network programming is powerful once you understand the basics. Start simple and build up!
