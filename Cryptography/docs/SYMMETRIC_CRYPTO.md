# Symmetric Encryption

## The Big Picture

Symmetric encryption uses the **same key** for encryption and decryption. It's fast and forms the backbone of secure communications (HTTPS, VPNs, file encryption).

## Simple Example: XOR Cipher

The simplest encryption - XOR each byte with key byte.

```c
void xor_encrypt(uint8_t* data, size_t len, const uint8_t* key, size_t key_len) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % key_len];  // XOR with key (repeating)
    }
}

// Decrypt is identical (XOR twice = original)
void xor_decrypt(uint8_t* data, size_t len, const uint8_t* key, size_t key_len) {
    xor_encrypt(data, len, key, key_len);  // Same function!
}

// Example:
char message[] = "Hello";
char key[] = "KEY";
xor_encrypt((uint8_t*)message, 5, (uint8_t*)key, 3);
// message is now encrypted
xor_decrypt((uint8_t*)message, 5, (uint8_t*)key, 3);
// message is back to "Hello"
```

**Why XOR?**
- `A XOR B XOR B = A` (self-inverse)
- Fast (single CPU instruction)
- Simple to implement

**Weakness:**
- If key reused, attackable
- Not secure for serious use (educational only!)

## Block Ciphers

Encrypt data in fixed-size blocks (AES = 128-bit blocks).

```
Plaintext blocks: [Block1] [Block2] [Block3]
                     ↓         ↓         ↓
                  Encrypt   Encrypt   Encrypt
                     ↓         ↓         ↓
Ciphertext blocks: [Block1] [Block2] [Block3]
```

### Padding

If data doesn't fit evenly into blocks, add padding.

```c
// PKCS#7 Padding
// If 3 bytes short, add: 03 03 03
// If 1 byte short, add: 01
// If perfect fit, add full block: 10 10 10 ... (16 times)

void pkcs7_pad(uint8_t* data, size_t data_len, size_t block_size) {
    size_t padding_len = block_size - (data_len % block_size);
    for (size_t i = 0; i < padding_len; i++) {
        data[data_len + i] = (uint8_t)padding_len;
    }
}

// Example: 13 bytes with 16-byte blocks
// Original: "Hello World!!"
// Padded:   "Hello World!!" + [03 03 03]
```

## Modes of Operation

How to encrypt multiple blocks.

### ECB (Electronic Codebook) - ⚠️ INSECURE

Encrypt each block independently.

```c
for (int i = 0; i < num_blocks; i++) {
    encrypt_block(plaintext_blocks[i], key, ciphertext_blocks[i]);
}
```

**Problem:** Identical plaintext blocks = identical ciphertext blocks.
- Patterns visible in encrypted image
- **Never use ECB!**

### CBC (Cipher Block Chaining) - ✅ Good

XOR each plaintext block with previous ciphertext block.

```c
// First block uses IV (Initialization Vector)
xor_block(plaintext[0], iv, temp);
encrypt_block(temp, key, ciphertext[0]);

// Subsequent blocks
for (int i = 1; i < num_blocks; i++) {
    xor_block(plaintext[i], ciphertext[i-1], temp);
    encrypt_block(temp, key, ciphertext[i]);
}
```

**IV must be random and unique per message!**

```c
// Encryption
uint8_t iv[16];
generate_random_bytes(iv, 16);
cbc_encrypt(plaintext, key, iv, ciphertext);
// Store/send: iv + ciphertext

// Decryption
cbc_decrypt(ciphertext, key, iv, plaintext);
```

### CTR (Counter Mode) - ✅ Good

Encrypts counter values, then XORs with plaintext (stream cipher).

```c
for (int i = 0; i < num_blocks; i++) {
    encrypt_block(counter + i, key, keystream[i]);
    xor_block(plaintext[i], keystream[i], ciphertext[i]);
}
```

**Advantages:**
- Parallelizable
- Random access
- No padding needed

## AES Basics

**AES (Advanced Encryption Standard)** is the current standard.

**Key sizes:** 128, 192, or 256 bits  
**Block size:** 128 bits (16 bytes)  
**Structure:** Substitution-Permutation Network

### Simplified AES Structure

```
Round (repeated 10-14 times):
1. SubBytes    - Substitute bytes using S-box
2. ShiftRows   - Shift rows cyclically
3. MixColumns  - Mix columns (diffusion)
4. AddRoundKey - XOR with round key

Final round skips MixColumns.
```

### AES State

```c
typedef uint8_t AESState[4][4];  // 4×4 matrix (16 bytes)

// Convert 16 bytes to state
void bytes_to_state(const uint8_t* in, AESState state) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[j][i] = in[i*4 + j];
        }
    }
}
```

### S-Box (Substitution)

Lookup table for byte substitution.

```c
static const uint8_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, // ...
    // Full table: 256 values
};

void sub_bytes(AESState state) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = sbox[state[i][j]];
        }
    }
}
```

## Key Schedule

AES expands the key into multiple round keys.

```c
// AES-128: 128-bit key → 11 round keys (176 bytes)
void aes_key_expansion(const uint8_t* key, uint8_t* round_keys);
```

## Stream Ciphers vs Block Ciphers

### Block Cipher (AES)
- Fixed block size (16 bytes)
- Needs padding
- Modes of operation

### Stream Cipher (ChaCha20)
- Encrypts byte-by-byte (or bit-by-bit)
- No padding needed
- Generates keystream

```c
// Stream cipher (simplified)
void stream_cipher(uint8_t* data, size_t len, const uint8_t* key) {
    uint8_t keystream[len];
    generate_keystream(key, keystream, len);
    
    for (size_t i = 0; i < len; i++) {
        data[i] ^= keystream[i];
    }
}
```

## Key Derivation Functions (KDF)

Derive encryption key from password.

```c
// PBKDF2 (Password-Based Key Derivation Function 2)
void pbkdf2(const char* password, const uint8_t* salt, int iterations,
           uint8_t* key, size_t key_len) {
    // Repeatedly hash password + salt
    // iterations = 100,000+ (slow by design)
    
    // Simplified version:
    uint8_t temp[32];
    sha256_string(password, (char*)temp);
    
    for (int i = 0; i < iterations; i++) {
        sha256_update_salt(temp, salt, temp);
    }
    
    memcpy(key, temp, key_len);
}
```

## Practical Example: File Encryption

```c
int encrypt_file(const char* input_file, const char* output_file, const char* password) {
    // 1. Read file
    FILE* in = fopen(input_file, "rb");
    // ... read data ...
    
    // 2. Generate salt
    uint8_t salt[16];
    generate_random_bytes(salt, 16);
    
    // 3. Derive key from password
    uint8_t key[32];
    pbkdf2(password, salt, 100000, key, 32);
    
    // 4. Generate IV
    uint8_t iv[16];
    generate_random_bytes(iv, 16);
    
    // 5. Encrypt
    aes_cbc_encrypt(data, data_len, key, iv, encrypted);
    
    // 6. Write: salt + iv + ciphertext
    FILE* out = fopen(output_file, "wb");
    fwrite(salt, 16, 1, out);
    fwrite(iv, 16, 1, out);
    fwrite(encrypted, encrypted_len, 1, out);
    fclose(out);
    
    return 0;
}
```

## Security Considerations

### 1. Never Reuse Keys+IV
```c
// BAD:
encrypt(message1, key, iv);
encrypt(message2, key, iv);  // ❌ Same IV = broken!

// GOOD:
generate_random_bytes(iv1, 16);
encrypt(message1, key, iv1);
generate_random_bytes(iv2, 16);  // New IV
encrypt(message2, key, iv2);     // ✅
```

### 2. Authenticate Then Encrypt
```c
// Encrypt-then-MAC (best)
ciphertext = encrypt(plaintext, key_enc);
mac = hmac(ciphertext, key_mac);
send(ciphertext + mac);
```

### 3. Use Different Keys
```c
// Different keys for different purposes
key_encrypt = derive_key(master_key, "encryption");
key_mac = derive_key(master_key, "authentication");
```

## Summary

| Algorithm | Type | Key Size | Block Size | Speed | Security |
|-----------|------|----------|------------|-------|----------|
| XOR | Stream | Any | 1 byte | ⚡⚡⚡ | ⚠️ Weak |
| DES | Block | 56 bits | 8 bytes | ⚡⚡ | ❌ Broken |
| 3DES | Block | 168 bits | 8 bytes | ⚡ | ⚠️ Deprecated |
| AES-128 | Block | 128 bits | 16 bytes | ⚡⚡ | ✅ Secure |
| AES-256 | Block | 256 bits | 16 bytes | ⚡⚡ | ✅ Very Secure |
| ChaCha20 | Stream | 256 bits | N/A | ⚡⚡ | ✅ Secure |

**Key Takeaways:**
- Symmetric = same key for encrypt/decrypt
- Block ciphers need padding and modes
- CBC requires random IV
- Never reuse key+IV pairs
- Always authenticate (HMAC)
- Derive keys from passwords (PBKDF2)

Symmetric encryption is fast and secure when used correctly - it's what protects your HTTPS connections and encrypted files!
