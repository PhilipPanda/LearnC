# Socket API Reference

Complete reference for socket programming functions.

## Creating Sockets

### socket()

Create a new socket.

```c
int socket(int domain, int type, int protocol);
```

**Parameters:**
- `domain` - Address family:
  - `AF_INET` - IPv4
  - `AF_INET6` - IPv6
- `type` - Socket type:
  - `SOCK_STREAM` - TCP (reliable, ordered, connection-based)
  - `SOCK_DGRAM` - UDP (unreliable, connectionless, fast)
- `protocol` - Usually 0 (auto-select based on type)

**Returns:** Socket file descriptor, or -1 on error

**Example:**
```c
int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);  // TCP
int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);   // UDP
```

## Server Functions

### bind()

Bind socket to a local address and port.

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Parameters:**
- `sockfd` - Socket file descriptor
- `addr` - Address structure (cast from `sockaddr_in*`)
- `addrlen` - Size of address structure

**Returns:** 0 on success, -1 on error

**Example:**
```c
struct sockaddr_in addr = {0};
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = INADDR_ANY;  // Bind to all interfaces
addr.sin_port = htons(8080);         // Port 8080

bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
```

**Common errors:**
- `EADDRINUSE` - Port already in use
- `EACCES` - Permission denied (ports < 1024 need root)

### listen()

Mark socket as passive (ready to accept connections).

```c
int listen(int sockfd, int backlog);
```

**Parameters:**
- `sockfd` - Socket file descriptor
- `backlog` - Maximum length of pending connections queue

**Returns:** 0 on success, -1 on error

**Example:**
```c
listen(sockfd, 10);  // Queue up to 10 pending connections
```

The backlog is how many clients can be waiting while you're busy with another. Set it based on expected traffic.

### accept()

Accept an incoming connection. Blocks until a client connects.

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**Parameters:**
- `sockfd` - Listening socket
- `addr` - Output: client's address (can be NULL)
- `addrlen` - Input/Output: size of address structure

**Returns:** New socket for the client, or -1 on error

**Example:**
```c
struct sockaddr_in client_addr;
socklen_t addr_len = sizeof(client_addr);
int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);

// Get client IP
char *client_ip = inet_ntoa(client_addr.sin_addr);
printf("Client connected from %s\n", client_ip);
```

**Important:** The original socket (`sockfd`) keeps listening. The new socket is for this specific client.

## Client Functions

### connect()

Connect to a remote server.

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Parameters:**
- `sockfd` - Socket file descriptor
- `addr` - Server address
- `addrlen` - Size of address structure

**Returns:** 0 on success, -1 on error

**Example:**
```c
struct sockaddr_in server = {0};
server.sin_family = AF_INET;
server.sin_addr.s_addr = inet_addr("127.0.0.1");
server.sin_port = htons(8080);

if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
    perror("Connection failed");
}
```

## Data Transfer

### send()

Send data over a connected socket.

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

**Parameters:**
- `sockfd` - Socket file descriptor
- `buf` - Data to send
- `len` - Number of bytes to send
- `flags` - Usually 0

**Returns:** Number of bytes sent, or -1 on error

**Example:**
```c
char *msg = "Hello, server!";
int sent = send(sockfd, msg, strlen(msg), 0);
if (sent < 0) {
    perror("send failed");
}
```

**Important:** May send less than requested! Always check return value:
```c
int total_sent = 0;
while (total_sent < len) {
    int sent = send(sockfd, buf + total_sent, len - total_sent, 0);
    if (sent < 0) return -1;  // Error
    total_sent += sent;
}
```

### recv()

Receive data from a connected socket.

```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

**Parameters:**
- `sockfd` - Socket file descriptor
- `buf` - Buffer to store received data
- `len` - Maximum bytes to receive
- `flags` - Usually 0

**Returns:** 
- Number of bytes received
- 0 if connection closed
- -1 on error

**Example:**
```c
char buffer[1024];
int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
if (bytes > 0) {
    buffer[bytes] = '\0';  // Null-terminate
    printf("Received: %s\n", buffer);
} else if (bytes == 0) {
    printf("Connection closed\n");
} else {
    perror("recv failed");
}
```

### sendto() / recvfrom()

For UDP (connectionless) sockets.

```c
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
```

**Example - UDP sender:**
```c
struct sockaddr_in dest = {0};
dest.sin_family = AF_INET;
dest.sin_addr.s_addr = inet_addr("192.168.1.100");
dest.sin_port = htons(8080);

sendto(sockfd, "message", 7, 0, (struct sockaddr*)&dest, sizeof(dest));
```

**Example - UDP receiver:**
```c
struct sockaddr_in sender;
socklen_t sender_len = sizeof(sender);
char buffer[1024];

int bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                     (struct sockaddr*)&sender, &sender_len);
```

## Socket Options

### setsockopt()

Set socket options.

```c
int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
```

**Common options:**

**SO_REUSEADDR** - Allow reuse of local addresses (avoid "Address already in use")
```c
int opt = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```

**SO_RCVTIMEO / SO_SNDTIMEO** - Set receive/send timeouts
```c
struct timeval tv;
tv.tv_sec = 5;   // 5 seconds
tv.tv_usec = 0;
setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
```

**TCP_NODELAY** - Disable Nagle's algorithm (reduce latency)
```c
int flag = 1;
setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
```

## Address Conversion

### inet_addr()

Convert IP string to binary form.

```c
in_addr_t inet_addr(const char *cp);
```

**Example:**
```c
addr.sin_addr.s_addr = inet_addr("192.168.1.100");
addr.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0 (any interface)
addr.sin_addr.s_addr = INADDR_LOOPBACK;  // 127.0.0.1
```

### inet_ntoa()

Convert binary address to string.

```c
char *inet_ntoa(struct in_addr in);
```

**Example:**
```c
struct sockaddr_in addr;
// ... after accept() or recvfrom()
printf("IP: %s\n", inet_ntoa(addr.sin_addr));
```

### inet_pton() / inet_ntop()

Modern alternatives (support IPv6).

```c
// String to binary
inet_pton(AF_INET, "192.168.1.1", &addr.sin_addr);

// Binary to string
char ip_str[INET_ADDRSTRLEN];
inet_ntop(AF_INET, &addr.sin_addr, ip_str, sizeof(ip_str));
```

## Multiplexing

### select()

Monitor multiple sockets for activity.

```c
int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout);
```

**Example - wait for data on multiple clients:**
```c
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(client1, &readfds);
FD_SET(client2, &readfds);
FD_SET(client3, &readfds);

int max_fd = (client1 > client2) ? client1 : client2;
max_fd = (max_fd > client3) ? max_fd : client3;

if (select(max_fd + 1, &readfds, NULL, NULL, NULL) > 0) {
    if (FD_ISSET(client1, &readfds)) {
        // client1 has data
    }
    if (FD_ISSET(client2, &readfds)) {
        // client2 has data
    }
    // ...
}
```

Allows handling multiple clients without threads.

## Non-blocking I/O

### fcntl() / ioctlsocket()

Make socket non-blocking.

**Linux:**
```c
#include <fcntl.h>
int flags = fcntl(sockfd, F_GETFL, 0);
fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
```

**Windows:**
```c
u_long mode = 1;  // 1 = non-blocking, 0 = blocking
ioctlsocket(sockfd, FIONBIO, &mode);
```

With non-blocking sockets, operations return immediately. Check for `EWOULDBLOCK` / `EAGAIN`:
```c
int bytes = recv(sockfd, buffer, sizeof(buffer), 0);
if (bytes < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
        // No data available yet
    }
}
```

## Cleanup

### close() / closesocket()

Close a socket.

**Linux:**
```c
close(sockfd);
```

**Windows:**
```c
closesocket(sockfd);
```

### shutdown()

Shutdown part of a connection.

```c
int shutdown(int sockfd, int how);
```

**Options:**
- `SHUT_RD` - Stop receiving
- `SHUT_WR` - Stop sending (common before close)
- `SHUT_RDWR` - Stop both

**Example:**
```c
shutdown(sockfd, SHUT_WR);  // Signal "I'm done sending"
close(sockfd);
```

## Error Handling

Always check return values:

```c
if (socket_fd < 0) {
    perror("socket");
    exit(1);
}

if (bind(socket_fd, ...) < 0) {
    perror("bind");
    close(socket_fd);
    exit(1);
}
```

**Common errors:**
- `EADDRINUSE` - Port in use (use SO_REUSEADDR)
- `ECONNREFUSED` - Server not running
- `ETIMEDOUT` - Connection timed out
- `EPIPE` - Broken pipe (wrote to closed socket)

**Get error message:**
```c
perror("Operation failed");  // Prints: "Operation failed: <error>"

// Or manually:
fprintf(stderr, "Error: %s\n", strerror(errno));
```

## Platform Abstraction

Common pattern for cross-platform code:

```c
#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define CLOSE_SOCKET close
#endif

// Windows init
#ifdef _WIN32
void init_sockets(void) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
}
void cleanup_sockets(void) {
    WSACleanup();
}
#else
void init_sockets(void) {}
void cleanup_sockets(void) {}
#endif
```

Then use `CLOSE_SOCKET(sockfd)` everywhere.

## Best Practices

1. **Always check return values** - Network operations fail often
2. **Use SO_REUSEADDR** - Avoid "Address in use" during development
3. **Handle partial sends/receives** - TCP doesn't guarantee full sends
4. **Set timeouts** - Prevent hanging forever
5. **Close sockets** - Avoid resource leaks
6. **Validate data** - Never trust network input
7. **Use select() for multiple clients** - Better than threads for many connections
8. **Test with localhost first** - Then test across network
9. **Use Wireshark** - See what's really happening on the wire
10. **Read error messages** - `perror()` is your friend
