# Asymmetric Encryption

## The Big Picture

Asymmetric encryption uses **two different keys**: a public key (can share freely) and a private key (keep secret). This solves the key distribution problem of symmetric encryption.

Also called **public-key cryptography**.

## Key Pairs

```
┌─────────────┐         ┌─────────────┐
│ Public Key  │←────────│  Key Pair   │
│ (share it!) │         │  Generator  │
└─────────────┘         └─────────────┘
                              │
┌─────────────┐               │
│ Private Key │←──────────────┘
│ (keep secret!)
└─────────────┘

Public + Private = Mathematically linked
Knowing public key doesn't reveal private key
```

## Two Uses

### 1. Encryption (Confidentiality)

```
Encrypt with PUBLIC key  → Only PRIVATE key can decrypt

Alice wants to send Bob a secret:
1. Alice gets Bob's public key
2. Alice encrypts: ciphertext = encrypt(message, bob_public_key)
3. Alice sends ciphertext
4. Bob decrypts: message = decrypt(ciphertext, bob_private_key)

Only Bob can read it!
```

### 2. Digital Signatures (Authentication)

```
Sign with PRIVATE key → Anyone with PUBLIC key can verify

Alice wants to prove she wrote a message:
1. Alice signs: signature = sign(message, alice_private_key)
2. Alice sends message + signature
3. Bob verifies: valid = verify(message, signature, alice_public_key)

Only Alice could have created this signature!
```

## RSA Algorithm

Most famous asymmetric algorithm. Based on factoring large numbers.

### Math Foundation

```
Given:
- n = p × q  (p, q are large primes)
- e = 65537  (public exponent, usually)
- d = modular inverse of e

Public key:  (n, e)
Private key: (n, d)

Encryption:  ciphertext = plaintext^e mod n
Decryption:  plaintext = ciphertext^d mod n
```

### Key Generation (Simplified)

```c
typedef struct {
    uint64_t n;  // Modulus
    uint64_t e;  // Public exponent
} RSAPublicKey;

typedef struct {
    uint64_t n;  // Modulus
    uint64_t d;  // Private exponent
} RSAPrivateKey;

void rsa_generate_keys(RSAPublicKey* pub, RSAPrivateKey* priv) {
    // 1. Choose two large primes
    uint64_t p = 61;  // In reality: 1024+ bit primes
    uint64_t q = 53;
    
    // 2. n = p * q
    uint64_t n = p * q;  // 3233
    
    // 3. φ(n) = (p-1) * (q-1)
    uint64_t phi = (p - 1) * (q - 1);  // 3120
    
    // 4. Choose e (commonly 65537)
    uint64_t e = 17;  // Small example
    
    // 5. Calculate d (modular inverse of e mod φ(n))
    uint64_t d = mod_inverse(e, phi);  // 2753
    
    // Public key
    pub->n = n;
    pub->e = e;
    
    // Private key
    priv->n = n;
    priv->d = d;
}
```

### Modular Exponentiation

```c
// Calculate (base^exp) mod m efficiently
uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    
    return result;
}
```

### RSA Encryption/Decryption

```c
uint64_t rsa_encrypt(uint64_t plaintext, RSAPublicKey* pub) {
    return mod_pow(plaintext, pub->e, pub->n);
}

uint64_t rsa_decrypt(uint64_t ciphertext, RSAPrivateKey* priv) {
    return mod_pow(ciphertext, priv->d, priv->n);
}

// Example:
RSAPublicKey pub;
RSAPrivateKey priv;
rsa_generate_keys(&pub, &priv);

uint64_t message = 42;
uint64_t encrypted = rsa_encrypt(message, &pub);     // Encrypt
uint64_t decrypted = rsa_decrypt(encrypted, &priv);  // Decrypt
// decrypted == 42
```

## Digital Signatures

Sign message to prove authenticity.

```c
uint64_t rsa_sign(uint64_t message_hash, RSAPrivateKey* priv) {
    // Sign with PRIVATE key
    return mod_pow(message_hash, priv->d, priv->n);
}

int rsa_verify(uint64_t message_hash, uint64_t signature, RSAPublicKey* pub) {
    // Verify with PUBLIC key
    uint64_t decrypted = mod_pow(signature, pub->e, pub->n);
    return (decrypted == message_hash);
}

// Usage:
char message[] = "I agree to the terms";
uint64_t hash = simple_hash(message);  // Hash message first
uint64_t signature = rsa_sign(hash, &alice_private);

// Anyone can verify
if (rsa_verify(hash, signature, &alice_public)) {
    printf("Signature valid! Alice signed this.\n");
}
```

## Hybrid Encryption

Combine asymmetric + symmetric (best of both worlds).

```c
// Encrypt large data:

// 1. Generate random symmetric key
uint8_t session_key[32];
generate_random_bytes(session_key, 32);

// 2. Encrypt data with symmetric key (fast!)
aes_encrypt(large_data, session_key, encrypted_data);

// 3. Encrypt session key with RSA (small, so RSA is OK)
uint64_t encrypted_key = rsa_encrypt(session_key, &recipient_public);

// 4. Send both
send(encrypted_key);      // RSA-encrypted session key
send(encrypted_data);     // AES-encrypted data

// Receiver:
session_key = rsa_decrypt(encrypted_key, &recipient_private);
data = aes_decrypt(encrypted_data, session_key);
```

**This is how HTTPS works!**

## Diffie-Hellman Key Exchange

Two parties agree on shared secret over insecure channel.

```c
// Public parameters (known to everyone)
uint64_t p = 23;  // Prime
uint64_t g = 5;   // Generator

// Alice
uint64_t alice_private = 6;  // Secret
uint64_t alice_public = mod_pow(g, alice_private, p);  // 8

// Bob
uint64_t bob_private = 15;  // Secret
uint64_t bob_public = mod_pow(g, bob_private, p);  // 19

// Exchange public values (can be intercepted, doesn't matter!)
// Alice receives bob_public
// Bob receives alice_public

// Both compute shared secret
uint64_t alice_shared = mod_pow(bob_public, alice_private, p);    // 2
uint64_t bob_shared = mod_pow(alice_public, bob_private, p);      // 2

// alice_shared == bob_shared!
// Use as symmetric key
```

## RSA Padding

Raw RSA is vulnerable. Always use padding.

### PKCS#1 v1.5 Padding
```c
// For encryption:
// 00 02 [random padding] 00 [message]

void rsa_pkcs1_pad(const uint8_t* message, size_t msg_len,
                  uint8_t* padded, size_t padded_len) {
    padded[0] = 0x00;
    padded[1] = 0x02;
    
    // Random padding
    for (size_t i = 2; i < padded_len - msg_len - 1; i++) {
        padded[i] = (rand() % 255) + 1;  // Non-zero
    }
    
    padded[padded_len - msg_len - 1] = 0x00;
    memcpy(padded + padded_len - msg_len, message, msg_len);
}
```

### OAEP (Optimal Asymmetric Encryption Padding)
More secure, but complex.

## Elliptic Curve Cryptography (ECC)

Alternative to RSA. Smaller keys, same security.

```
RSA 2048-bit ≈ ECC 224-bit (same security level)

Advantages:
- Smaller keys
- Faster operations
- Less bandwidth

Disadvantage:
- More complex math
```

## Key Management

### Storing Keys

```c
// Never store private keys in plaintext!

// Option 1: Encrypt private key with password
encrypted_key = aes_encrypt(private_key, password_derived_key);

// Option 2: Use OS keychain/keystore

// Option 3: Hardware security module (HSM)
```

### Key File Format (PEM-style)

```
-----BEGIN RSA PRIVATE KEY-----
Base64EncodedKeyData...
-----END RSA PRIVATE KEY-----
```

## Attacks on RSA

### 1. Factoring n
If you can factor n = p × q, you can recover private key.

**Defense:** Use large keys (2048+ bits)

### 2. Small Exponent Attack
If e is small and message is small, can break.

**Defense:** Use padding (OAEP)

### 3. Timing Attacks
Measure decryption time to leak key bits.

**Defense:** Constant-time operations

## Summary

| Feature | Symmetric | Asymmetric |
|---------|-----------|------------|
| Keys | One shared key | Key pair (public + private) |
| Speed | Fast (⚡⚡⚡) | Slow (⚡) |
| Key Size | 128-256 bits | 2048-4096 bits |
| Use Case | Bulk encryption | Key exchange, signatures |
| Examples | AES, ChaCha20 | RSA, ECC |

**Key Concepts:**
- Public key = encrypt or verify
- Private key = decrypt or sign
- RSA based on factoring problem
- Hybrid encryption = RSA + AES
- Digital signatures prove authenticity
- Diffie-Hellman for key agreement

Asymmetric crypto enables secure communication without pre-shared secrets - it's what makes the internet secure!
