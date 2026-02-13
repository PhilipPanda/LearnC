/*
 * Base64 Encoding/Decoding
 * 
 * Learn Base64:
 * - Why Base64 is needed
 * - Encoding binary to ASCII
 * - Decoding ASCII back to binary
 * - Practical applications
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Base64 alphabet
static const char base64_chars[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Base64 encode
size_t base64_encode(const uint8_t* input, size_t input_len, char* output) {
    size_t output_len = 0;
    uint32_t buffer;
    int bits_collected = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        buffer = (buffer << 8) | input[i];
        bits_collected += 8;
        
        while (bits_collected >= 6) {
            bits_collected -= 6;
            output[output_len++] = base64_chars[(buffer >> bits_collected) & 0x3F];
        }
    }
    
    // Handle remaining bits
    if (bits_collected > 0) {
        buffer <<= (6 - bits_collected);
        output[output_len++] = base64_chars[buffer & 0x3F];
    }
    
    // Add padding
    while (output_len % 4 != 0) {
        output[output_len++] = '=';
    }
    
    output[output_len] = '\0';
    return output_len;
}

// Base64 decode
int base64_decode_char(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;  // Invalid character
}

size_t base64_decode(const char* input, uint8_t* output) {
    size_t input_len = strlen(input);
    size_t output_len = 0;
    uint32_t buffer = 0;
    int bits_collected = 0;
    
    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == '=') break;  // Padding
        
        int value = base64_decode_char(input[i]);
        if (value < 0) continue;  // Skip invalid characters
        
        buffer = (buffer << 6) | value;
        bits_collected += 6;
        
        if (bits_collected >= 8) {
            bits_collected -= 8;
            output[output_len++] = (buffer >> bits_collected) & 0xFF;
        }
    }
    
    return output_len;
}

// Print bytes
void print_bytes(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", data[i]);
    }
}

void demo_basic_encoding() {
    printf("=== Basic Base64 Encoding ===\n\n");
    
    const char* test_strings[] = {
        "A",
        "AB",
        "ABC",
        "Hello",
        "Hello World!",
        "The quick brown fox"
    };
    
    for (int i = 0; i < 6; i++) {
        char encoded[256];
        base64_encode((uint8_t*)test_strings[i], strlen(test_strings[i]), encoded);
        
        printf("Input:    \"%s\"\n", test_strings[i]);
        printf("Encoded:  %s\n", encoded);
        printf("Length:   %zu → %zu characters\n\n", 
               strlen(test_strings[i]), strlen(encoded));
    }
}

void demo_binary_data() {
    printf("=== Binary Data Encoding ===\n\n");
    
    // Binary data (not printable)
    uint8_t binary[] = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD, 0xFC};
    size_t binary_len = sizeof(binary);
    
    printf("Binary data (hex): ");
    print_bytes(binary, binary_len);
    printf("\n");
    
    // Encode
    char encoded[256];
    base64_encode(binary, binary_len, encoded);
    printf("Base64:            %s\n\n", encoded);
    
    printf("✓ Binary data → ASCII text\n");
    printf("✓ Can be transmitted over text-only channels\n");
    printf("✓ Can be included in JSON, XML, email\n");
}

void demo_roundtrip() {
    printf("\n=== Encode/Decode Round-trip ===\n\n");
    
    const char* original = "Secret Message 123!@#";
    printf("Original: \"%s\"\n", original);
    
    // Encode
    char encoded[256];
    base64_encode((uint8_t*)original, strlen(original), encoded);
    printf("Encoded:  %s\n", encoded);
    
    // Decode
    uint8_t decoded[256];
    size_t decoded_len = base64_decode(encoded, decoded);
    decoded[decoded_len] = '\0';
    
    printf("Decoded:  \"%s\"\n\n", (char*)decoded);
    
    if (strcmp(original, (char*)decoded) == 0) {
        printf("✓ Perfect round-trip! Original == Decoded\n");
    } else {
        printf("✗ Error: Mismatch!\n");
    }
}

void demo_padding() {
    printf("\n=== Padding Examples ===\n\n");
    
    printf("Base64 outputs must be multiple of 4 characters.\n");
    printf("Padding '=' is added if needed.\n\n");
    
    const char* inputs[] = {"A", "AB", "ABC", "ABCD"};
    
    for (int i = 0; i < 4; i++) {
        char encoded[256];
        base64_encode((uint8_t*)inputs[i], strlen(inputs[i]), encoded);
        
        int padding = 0;
        for (size_t j = 0; encoded[j]; j++) {
            if (encoded[j] == '=') padding++;
        }
        
        printf("Input: \"%s\" (%zu bytes)\n", inputs[i], strlen(inputs[i]));
        printf("  Encoded: %s\n", encoded);
        printf("  Padding: %d\n\n", padding);
    }
}

void demo_efficiency() {
    printf("=== Efficiency Analysis ===\n\n");
    
    uint8_t data[100];
    for (int i = 0; i < 100; i++) {
        data[i] = i;
    }
    
    char encoded[256];
    size_t encoded_len = base64_encode(data, 100, encoded);
    
    printf("Binary data:  100 bytes\n");
    printf("Base64:       %zu characters\n", encoded_len);
    printf("Overhead:     +%.1f%%\n\n", ((float)encoded_len / 100 - 1) * 100);
    
    printf("Base64 expands data by ~33%%\n");
    printf("3 bytes → 4 characters\n");
    printf("This is the cost of binary → ASCII conversion\n");
}

void demo_use_cases() {
    printf("\n=== Practical Use Cases ===\n\n");
    
    printf("Base64 is used in:\n\n");
    
    printf("1. Email Attachments (MIME)\n");
    printf("   Content-Transfer-Encoding: base64\n");
    printf("   SGVsbG8gV29ybGQh\n\n");
    
    printf("2. Data URLs (HTML)\n");
    printf("   <img src=\"data:image/png;base64,iVBORw0KG...\">\n\n");
    
    printf("3. JWT Tokens (Authentication)\n");
    printf("   eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWI...\n\n");
    
    printf("4. Basic HTTP Authentication\n");
    printf("   Authorization: Basic dXNlcjpwYXNzd29yZA==\n\n");
    
    printf("5. Binary Config Files\n");
    printf("   api_key = \"YWJjMTIz...\"\n\n");
    
    // Demo: Basic Auth
    printf("--- Example: HTTP Basic Auth ---\n");
    const char* username = "admin";
    const char* password = "secret123";
    char credentials[100];
    snprintf(credentials, sizeof(credentials), "%s:%s", username, password);
    
    char encoded_creds[256];
    base64_encode((uint8_t*)credentials, strlen(credentials), encoded_creds);
    
    printf("Credentials: %s:%s\n", username, password);
    printf("Base64:      %s\n", encoded_creds);
    printf("Header:      Authorization: Basic %s\n", encoded_creds);
}

void demo_not_encryption() {
    printf("\n=== ⚠️ Base64 is NOT Encryption! ===\n\n");
    
    const char* secret = "MyPassword123";
    char encoded[256];
    base64_encode((uint8_t*)secret, strlen(secret), encoded);
    
    printf("\"Secret\":  %s\n", secret);
    printf("Base64:     %s\n\n", encoded);
    
    printf("❌ Base64 is ENCODING, not ENCRYPTION!\n");
    printf("❌ Anyone can decode Base64\n");
    printf("❌ Provides NO security!\n");
    printf("❌ Just makes data text-safe\n\n");
    
    printf("For security, use:\n");
    printf("  ✓ Encryption (AES, RSA)\n");
    printf("  ✓ Then Base64 to make it text-safe (optional)\n");
}

int main(void) {
    printf("=== Base64 Encoding/Decoding ===\n\n");
    
    demo_basic_encoding();
    demo_binary_data();
    demo_roundtrip();
    demo_padding();
    demo_efficiency();
    demo_use_cases();
    demo_not_encryption();
    
    printf("\n=== Summary ===\n");
    printf("Base64 converts binary ↔ ASCII text\n");
    printf("✓ 6 bits → 1 character (A-Z, a-z, 0-9, +, /)\n");
    printf("✓ Always produces printable ASCII\n");
    printf("✓ 33%% size increase\n");
    printf("✓ Reversible (not encryption!)\n");
    printf("✓ Used for data URLs, email, JWT, HTTP auth\n");
    printf("❌ NOT for security (anyone can decode)\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
