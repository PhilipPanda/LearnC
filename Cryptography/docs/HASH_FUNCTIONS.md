# Hash Functions

## The Big Picture

Hash functions are one-way transformations that convert data of any size into a fixed-size output (digest/hash). They're fundamental to password storage, data integrity, digital signatures, and blockchain.

## Properties of Cryptographic Hashes

### 1. Deterministic
Same input always produces same output.

```c
md5("hello") → always "5d41402abc4b2a76b9719d911017c592"
md5("hello") → always "5d41402abc4b2a76b9719d911017c592"
```

### 2. One-Way (Preimage Resistance)
Cannot reverse hash to get original input.

```c
// Easy:
hash = md5("password123")

// Impossible:
original = reverse_md5(hash)  // No such function exists!
```

### 3. Avalanche Effect
Tiny input change = completely different hash.

```c
md5("hello")  → "5d41402abc4b2a76b9719d911017c592"
md5("Hello")  → "8b1a9953c4611296a827abf8c47804d7"
                 ↑ Only 1 character different, hash completely different!
```

### 4. Collision Resistance
Hard to find two different inputs with same hash.

```c
// Finding this is VERY hard:
md5(input1) == md5(input2)  // where input1 != input2
```

## MD5 Algorithm

**MD5** produces 128-bit (16-byte) hashes. Output: 32 hex characters.

### Structure

```c
typedef struct {
    uint32_t state[4];     // Hash state (A, B, C, D)
    uint32_t count[2];     // Bit count
    uint8_t buffer[64];    // Input buffer
} MD5Context;

// Output
typedef struct {
    uint8_t bytes[16];     // 128 bits
} MD5Hash;
```

### Basic Implementation

```c
void md5_init(MD5Context* ctx);
void md5_update(MD5Context* ctx, const uint8_t* data, size_t len);
void md5_final(MD5Context* ctx, MD5Hash* hash);

// Usage:
MD5Context ctx;
MD5Hash hash;

md5_init(&ctx);
md5_update(&ctx, (uint8_t*)"Hello", 5);
md5_update(&ctx, (uint8_t*)" World", 6);
md5_final(&ctx, &hash);

// hash.bytes contains the 16-byte MD5 hash
```

### MD5 in Practice

```c
void md5_string(const char* input, char* output_hex) {
    MD5Context ctx;
    MD5Hash hash;
    
    md5_init(&ctx);
    md5_update(&ctx, (uint8_t*)input, strlen(input));
    md5_final(&ctx, &hash);
    
    // Convert to hex string
    for (int i = 0; i < 16; i++) {
        sprintf(output_hex + i*2, "%02x", hash.bytes[i]);
    }
    output_hex[32] = '\0';
}

// Example:
char hash[33];
md5_string("password", hash);
printf("%s\n", hash);  // "5f4dcc3b5aa765d61d8327deb882cf99"
```

### ⚠️ MD5 is BROKEN

MD5 is **cryptographically broken** (collisions can be found).

**Don't use for:**
- Password hashing
- Digital signatures
- Security-critical applications

**Still OK for:**
- Checksums (non-security)
- Cache keys
- Hash tables

## SHA-256 Algorithm

**SHA-256** produces 256-bit (32-byte) hashes. Output: 64 hex characters.

Part of SHA-2 family. Currently considered secure.

### Structure

```c
typedef struct {
    uint32_t state[8];     // Hash state
    uint64_t count;        // Bit count
    uint8_t buffer[64];    // Input buffer
} SHA256Context;

typedef struct {
    uint8_t bytes[32];     // 256 bits
} SHA256Hash;
```

### Usage

```c
void sha256_string(const char* input, char* output_hex) {
    SHA256Context ctx;
    SHA256Hash hash;
    
    sha256_init(&ctx);
    sha256_update(&ctx, (uint8_t*)input, strlen(input));
    sha256_final(&ctx, &hash);
    
    // Convert to hex
    for (int i = 0; i < 32; i++) {
        sprintf(output_hex + i*2, "%02x", hash.bytes[i]);
    }
    output_hex[64] = '\0';
}

// Example:
char hash[65];
sha256_string("hello", hash);
// "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824"
```

## File Hashing

Hash entire files to verify integrity.

```c
int hash_file_md5(const char* filename, char* output_hex) {
    FILE* file = fopen(filename, "rb");
    if (!file) return -1;
    
    MD5Context ctx;
    MD5Hash hash;
    md5_init(&ctx);
    
    uint8_t buffer[4096];
    size_t bytes_read;
    
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        md5_update(&ctx, buffer, bytes_read);
    }
    
    md5_final(&ctx, &hash);
    fclose(file);
    
    for (int i = 0; i < 16; i++) {
        sprintf(output_hex + i*2, "%02x", hash.bytes[i]);
    }
    output_hex[32] = '\0';
    
    return 0;
}
```

## Password Hashing

**NEVER store passwords in plaintext!**

### Wrong Way (Plain)
```c
// Database:
username: "alice"
password: "password123"  // ❌ TERRIBLE!
```

### Wrong Way (MD5)
```c
// Database:
username: "alice"
password_hash: "5f4dcc3b5aa765d61d8327deb882cf99"  // ❌ Still bad!
// Vulnerable to rainbow tables
```

### Better Way (Salt + SHA-256)
```c
char password[] = "password123";
char salt[17];
generate_random_salt(salt, 16);  // Random salt
char combined[100];
sprintf(combined, "%s%s", salt, password);

char hash[65];
sha256_string(combined, hash);

// Store in database:
username: "alice"
salt: "a3f8e9d1c4b2a7e6"
password_hash: "..."  // ✅ Better!
```

### Best Way (Key Derivation Functions)
Use purpose-built algorithms: bcrypt, scrypt, Argon2.

```c
// These are intentionally SLOW to prevent brute force
char password[] = "password123";
char hash[61];
bcrypt_hash(password, 12, hash);  // 12 = work factor (iterations)

// Verify password
int matches = bcrypt_verify(password, hash);
```

## HMAC (Hash-based MAC)

Authenticate messages using a key.

```c
char message[] = "Transfer $100 to Alice";
char key[] = "shared_secret";
char mac[65];

hmac_sha256(message, key, mac);
// Send: message + mac

// Receiver:
char computed_mac[65];
hmac_sha256(received_message, key, computed_mac);

if (strcmp(mac, computed_mac) == 0) {
    // Message authentic!
} else {
    // Message tampered!
}
```

### HMAC Implementation (Simplified)

```c
void hmac_sha256(const char* message, const char* key, char* output) {
    // HMAC = H((K ⊕ opad) || H((K ⊕ ipad) || message))
    
    uint8_t ipad[64], opad[64];
    memset(ipad, 0x36, 64);
    memset(opad, 0x5c, 64);
    
    // XOR key with pads
    for (int i = 0; i < strlen(key); i++) {
        ipad[i] ^= key[i];
        opad[i] ^= key[i];
    }
    
    // Inner hash: H((K ⊕ ipad) || message)
    SHA256Context ctx;
    SHA256Hash inner_hash;
    sha256_init(&ctx);
    sha256_update(&ctx, ipad, 64);
    sha256_update(&ctx, (uint8_t*)message, strlen(message));
    sha256_final(&ctx, &inner_hash);
    
    // Outer hash: H((K ⊕ opad) || inner_hash)
    SHA256Hash final_hash;
    sha256_init(&ctx);
    sha256_update(&ctx, opad, 64);
    sha256_update(&ctx, inner_hash.bytes, 32);
    sha256_final(&ctx, &final_hash);
    
    // Convert to hex
    for (int i = 0; i < 32; i++) {
        sprintf(output + i*2, "%02x", final_hash.bytes[i]);
    }
}
```

## Practical Uses

### 1. Password Storage
```c
// Store this in database
store_password(username, bcrypt_hash(password + salt));
```

### 2. File Integrity
```c
// Before:
char hash1[65];
sha256_file("important.doc", hash1);

// After download/transfer:
char hash2[65];
sha256_file("important.doc", hash2);

if (strcmp(hash1, hash2) == 0) {
    // File unchanged!
}
```

### 3. Digital Signatures
```c
// Hash message first, then sign the hash
char hash[65];
sha256_string(message, hash);
rsa_sign(hash, private_key, signature);
```

### 4. Proof of Work (Blockchain)
```c
// Find input where hash starts with N zeros
int nonce = 0;
char hash[65];
char data[100];

while (1) {
    sprintf(data, "%s%d", block_data, nonce);
    sha256_string(data, hash);
    
    if (strncmp(hash, "0000", 4) == 0) {
        // Found valid hash!
        break;
    }
    nonce++;
}
```

## Summary

- **Hash:** One-way, fixed size, deterministic
- **MD5:** 128-bit, broken (don't use for security)
- **SHA-256:** 256-bit, secure, current standard
- **Salt:** Random data added to prevent rainbow tables
- **HMAC:** Authenticate messages with shared key
- **bcrypt/scrypt:** Slow hashes for passwords

Hash functions are everywhere in security - master them and you understand the foundation of modern cryptography!
