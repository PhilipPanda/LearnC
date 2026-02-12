# Network Protocols

Understanding the protocols that make the internet work.

## The Big Picture

Protocols are rules for how programs communicate over a network. Like a language - both sides need to speak the same one.

```
[Application Layer]  HTTP, FTP, DNS, etc.
       |
[Transport Layer]    TCP, UDP
       |
[Network Layer]      IP (routing packets)
       |
[Link Layer]         Ethernet, WiFi (physical transmission)
```

We work at the **Application Layer** using **TCP** or **UDP** at the Transport Layer.

## TCP (Transmission Control Protocol)

**The reliable choice.** Guarantees delivery and order.

### How It Works

TCP establishes a connection before sending data (three-way handshake):

```
CLIENT                  SERVER
  |                        |
  |----SYN--------------->|  "I want to connect"
  |<---SYN-ACK-----------|  "OK, let's connect"
  |----ACK--------------->|  "Connected!"
  |                        |
  |====DATA TRANSFER======|
  |                        |
  |----FIN--------------->|  "I'm done"
  |<---ACK----------------|  "OK"
  |<---FIN----------------|  "I'm done too"
  |----ACK--------------->|  "Bye"
```

### Features

**Reliability:**
- Every packet is acknowledged
- Lost packets are retransmitted
- Checksums detect corruption

**Ordering:**
- Packets arrive in the order sent
- Out-of-order packets are reordered

**Flow Control:**
- Prevents sender from overwhelming receiver
- Receiver tells sender "slow down" if needed

**Congestion Control:**
- Detects network congestion
- Reduces transmission rate automatically

### When to Use TCP

- **Web (HTTP/HTTPS)** - Pages must load completely
- **Email (SMTP, IMAP)** - Can't lose messages
- **File transfer (FTP)** - Files must be intact
- **Chat applications** - Messages in order
- **SSH/Telnet** - Every keystroke matters
- **Database connections** - Queries can't be lost

### The Cost

All that reliability comes with overhead:
- Connection setup (handshake)
- Acknowledgment packets
- Retransmissions
- More CPU/memory on both ends

Typical overhead: 20-60 bytes per packet, plus RTT latency.

## UDP (User Datagram Protocol)

**The fast choice.** Fire and forget.

### How It Works

No connection, no guarantees. Just send packets (datagrams) to an address:

```
CLIENT                  SERVER
  |                        |
  |----DATAGRAM---------->|  Sent
  |----DATAGRAM---------->|  Sent
  |----DATAGRAM---X        Lost (no one knows!)
  |----DATAGRAM---------->|  Sent
```

### Features

**Speed:**
- No connection setup
- No waiting for acknowledgments
- Minimal protocol overhead

**Simplicity:**
- Just send data to an address
- No state to maintain

**Multicast/Broadcast:**
- Can send to multiple recipients at once
- TCP can only do one-to-one

### When to Use UDP

- **Gaming** - Fast position updates, lag is worse than packet loss
- **Video streaming** - Missing a frame is OK, buffering is annoying
- **Voice over IP** - Brief audio drops OK, delay is not
- **DNS** - Simple query/response, just retry if no answer
- **IoT sensors** - Send periodic readings, missing one is fine
- **Real-time dashboards** - Latest value matters, not every value

### The Tradeoff

You handle reliability yourself if you need it:
- Sequence numbers (detect missing packets)
- Acknowledgments (custom protocol)
- Timeouts and retransmissions
- Ordering logic

Most UDP apps accept some loss as acceptable.

## TCP vs UDP Comparison

| Feature | TCP | UDP |
|---------|-----|-----|
| Connection | Yes (three-way handshake) | No (connectionless) |
| Reliability | Guaranteed delivery | No guarantee |
| Ordering | Always in order | May arrive out of order |
| Speed | Slower (overhead) | Faster (minimal overhead) |
| Header size | 20 bytes minimum | 8 bytes |
| Use case | Accuracy critical | Speed critical |
| Example | HTTP, FTP, email | DNS, gaming, streaming |

## HTTP (Hypertext Transfer Protocol)

The protocol of the web. Built on top of TCP.

### Request Format

```
GET /index.html HTTP/1.1
Host: www.example.com
User-Agent: MyApp/1.0
Connection: close

```

Parts:
- **Method**: GET, POST, PUT, DELETE, etc.
- **Path**: Resource you want
- **HTTP version**: Usually HTTP/1.1
- **Headers**: Key-value pairs (Host, User-Agent, etc.)
- **Blank line**: Separates headers from body
- **Body**: Optional (for POST/PUT)

### Response Format

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 1234
Connection: close

<html>
...page content...
</html>
```

Parts:
- **Status line**: HTTP version, status code, reason
- **Headers**: Content-Type, Length, etc.
- **Blank line**: Separates headers from body
- **Body**: The actual content

### Status Codes

- **1xx** - Informational
- **2xx** - Success
  - `200 OK` - Request succeeded
  - `201 Created` - Resource created
- **3xx** - Redirection
  - `301 Moved Permanently`
  - `302 Found` (temporary redirect)
- **4xx** - Client error
  - `400 Bad Request`
  - `404 Not Found`
  - `403 Forbidden`
- **5xx** - Server error
  - `500 Internal Server Error`
  - `503 Service Unavailable`

### Common Headers

**Request headers:**
- `Host:` - Server domain (required in HTTP/1.1)
- `User-Agent:` - Client software
- `Accept:` - Content types client understands
- `Content-Type:` - Type of body data
- `Content-Length:` - Size of body
- `Connection:` - `keep-alive` or `close`

**Response headers:**
- `Content-Type:` - MIME type of body (text/html, application/json, etc.)
- `Content-Length:` - Size of body in bytes
- `Server:` - Server software
- `Set-Cookie:` - Set a cookie

### Example HTTP GET in C

```c
// Connect to server (port 80 for HTTP)
// ...

// Send request
char *request = 
    "GET / HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Connection: close\r\n"
    "\r\n";
send(sockfd, request, strlen(request), 0);

// Receive response
char buffer[4096];
int bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
buffer[bytes] = '\0';
printf("%s\n", buffer);
```

**Important:** HTTP uses `\r\n` (CRLF) for line endings, not just `\n`.

### HTTP Methods

**GET** - Retrieve a resource
```
GET /users/123 HTTP/1.1
Host: api.example.com
```

**POST** - Create a resource or submit data
```
POST /users HTTP/1.1
Host: api.example.com
Content-Type: application/json
Content-Length: 27

{"name":"Alice","age":30}
```

**PUT** - Update a resource
```
PUT /users/123 HTTP/1.1
Content-Type: application/json

{"name":"Alice Smith"}
```

**DELETE** - Delete a resource
```
DELETE /users/123 HTTP/1.1
```

## Custom Protocols

You can design your own protocol on top of TCP or UDP.

### Message Format Example

```
[4 bytes: message length][1 byte: message type][N bytes: data]
```

**Benefits:**
- Efficient (binary format)
- Versioned (type field for different messages)
- Easy to parse

**C implementation:**
```c
// Send message
typedef struct {
    uint32_t length;  // Total length (including header)
    uint8_t type;     // Message type
    uint8_t data[];   // Variable-length data
} Message;

void send_message(int sock, uint8_t type, const void *data, uint32_t data_len) {
    uint32_t total_len = 5 + data_len;  // Header + data
    uint32_t net_len = htonl(total_len);
    
    send(sock, &net_len, 4, 0);           // Length
    send(sock, &type, 1, 0);              // Type
    send(sock, data, data_len, 0);        // Data
}

// Receive message
int recv_message(int sock, uint8_t *type, void *buffer, size_t buf_size) {
    // Read length
    uint32_t net_len;
    recv(sock, &net_len, 4, 0);
    uint32_t len = ntohl(net_len) - 5;  // Subtract header
    
    // Read type
    recv(sock, type, 1, 0);
    
    // Read data
    if (len > buf_size) return -1;  // Buffer too small
    return recv(sock, buffer, len, 0);
}
```

### Protocol Design Tips

1. **Version field** - For future compatibility
2. **Length prefix** - Know how much data to expect
3. **Type/command field** - Distinguish message types
4. **Network byte order** - Use htonl/htons for integers
5. **Magic numbers** - Validate you're talking to the right protocol
6. **Checksums** - Detect corruption (especially for UDP)
7. **Keep it simple** - Start simple, add features as needed

### Example: Simple Chat Protocol

```
Message types:
0x01 - LOGIN: [4 bytes: username length][N bytes: username]
0x02 - MESSAGE: [4 bytes: text length][N bytes: text]
0x03 - LOGOUT: [no data]
0x04 - SERVER_MSG: [4 bytes: text length][N bytes: text]
```

Client sends LOGIN, then MESSAGEs. Server broadcasts to all clients.

## Port Numbers

Standard ports for common protocols:

| Port | Protocol | Description |
|------|----------|-------------|
| 20/21 | FTP | File transfer |
| 22 | SSH | Secure shell |
| 23 | Telnet | Remote terminal |
| 25 | SMTP | Email sending |
| 53 | DNS | Domain name lookup |
| 80 | HTTP | Web |
| 110 | POP3 | Email retrieval |
| 143 | IMAP | Email retrieval |
| 443 | HTTPS | Secure web |
| 3306 | MySQL | Database |
| 5432 | PostgreSQL | Database |
| 6379 | Redis | Cache/database |
| 8080 | HTTP-alt | Development web server |

**Port ranges:**
- 0-1023: **Well-known ports** (system/privileged)
- 1024-49151: **Registered ports** (applications)
- 49152-65535: **Dynamic/private ports** (temporary use)

## Testing and Debugging

**netcat (nc)** - Swiss army knife of networking
```bash
# TCP server
nc -l 8080

# TCP client
nc localhost 8080

# UDP server
nc -u -l 8080

# Send HTTP request
echo -e "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n" | nc example.com 80
```

**telnet** - Connect to TCP servers
```bash
telnet localhost 8080
```

**Wireshark** - See actual packets
- Capture network traffic
- Filter by protocol, port, IP
- See exact bytes sent/received
- Essential for debugging

**curl** - Test HTTP
```bash
curl http://example.com
curl -v http://example.com  # Verbose (show headers)
curl -X POST -d "data" http://example.com
```

## Summary

- **TCP**: Reliable, ordered, slower. Use when data can't be lost.
- **UDP**: Fast, unreliable, simpler. Use when speed > accuracy.
- **HTTP**: Web protocol on TCP. Request/response with headers.
- **Custom protocols**: Design your own for specific needs.
- **Ports**: 0-1023 system, 1024+ applications, avoid conflicts.

Choose the right tool for the job. Most apps use TCP for reliability. UDP when you need speed and can tolerate loss. HTTP when you want web compatibility.
