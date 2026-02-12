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
