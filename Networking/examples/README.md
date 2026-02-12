# Networking Examples

Progressive examples teaching network programming in C.

## Building

Windows:
```bash
build_all.bat
```

Linux:
```bash
./build_all.sh
```

## Examples

### 01_simple_server.c
**Basic TCP echo server**

What it teaches:
- Creating a socket
- Binding to a port
- Listening for connections
- Accepting a client
- Receiving and sending data
- Closing connections

Run this first, then connect with `02_simple_client` or telnet.

### 02_simple_client.c
**Basic TCP client**

What it teaches:
- Creating a client socket
- Connecting to a server
- Sending user input
- Receiving responses
- Interactive communication

Run `01_simple_server` first, then run this.

### 03_echo_server.c
**Enhanced echo server**

What it teaches:
- Handling multiple clients (sequentially)
- Logging client connections
- Better error handling
- Server that runs continuously

Accepts clients one at a time. Each client must disconnect before the next can connect.

### 04_chat_server.c
**Multi-client chat server**

What it teaches:
- Handling multiple clients simultaneously
- Using select() to multiplex connections
- Broadcasting messages to all clients
- Managing client list

Run this and connect with multiple `02_simple_client` instances. Messages from one appear on all.

### 05_udp_sender.c
**UDP datagram sender**

What it teaches:
- Creating UDP sockets (SOCK_DGRAM)
- Sending datagrams with sendto()
- Connectionless communication
- No handshake required

Run `06_udp_receiver` first, then use this to send messages.

### 06_udp_receiver.c
**UDP datagram receiver**

What it teaches:
- Receiving UDP datagrams
- Getting sender information with recvfrom()
- No accept() needed for UDP
- Handling connectionless protocol

Run this first, then connect with `05_udp_sender`.

### 07_http_client.c
**Simple HTTP client**

What it teaches:
- HTTP protocol basics
- DNS lookup with gethostbyname()
- Formatting HTTP requests
- Parsing responses

Usage: `07_http_client http://example.com`

### 08_file_transfer.c
**File transfer (server and client)**

What it teaches:
- Binary file transfer
- Custom protocol design (length-prefixed)
- Progress indicators
- Large data handling

Server mode: `08_file_transfer server`  
Client mode: `08_file_transfer client 127.0.0.1 file.txt`

## Testing

**Test server with telnet:**
```bash
telnet localhost 8080
```

**Test server with netcat:**
```bash
nc localhost 8080
```

**Run server and client:**
```bash
# Terminal 1
bin\01_simple_server.exe

# Terminal 2
bin\02_simple_client.exe
```

## Learning Path

1. **01 → 02**: Basic client-server communication
2. **03**: Improved server that handles multiple clients
3. Read docs to understand TCP vs UDP
4. Experiment - change ports, add features, break things

## Common Issues

**"Address already in use"**
- Previous server still running
- Wait 30 seconds for OS to release port
- Or use SO_REUSEADDR (already done in examples)

**"Connection refused"**
- Server not running
- Wrong port number
- Firewall blocking connection

**Client hangs on connect**
- Server not listening yet
- Wrong IP address
- Network issue

## Next Steps

After mastering these basics:
- Multi-client servers with select() or threads
- UDP for fast, connectionless communication
- HTTP client to fetch web pages
- File transfer protocol
- Chat application
- Your own protocol design

The fundamentals here apply to all network programming!
