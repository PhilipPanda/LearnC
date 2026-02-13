# Cryptography Basics

## The Big Picture

Cryptography is the science of securing information. It uses mathematical algorithms to transform data so only authorized parties can read it. Understanding crypto fundamentals is essential for building secure software.

## Core Concepts

### Plaintext and Ciphertext

- **Plaintext:** Original, readable message
- **Ciphertext:** Encrypted, unreadable message

```
Plaintext:  "Hello World"
      ↓
   Encrypt
      ↓
Ciphertext: "X7#mK9@pL2q"
      ↓
   Decrypt
      ↓
Plaintext:  "Hello World"
```

### Keys

A **key** is secret data used to encrypt/decrypt messages.

```c
char plaintext[] = "Secret message";
char key[] = "MySecretKey123";
char ciphertext[100];

encrypt(plaintext, key, ciphertext);  // Uses key to encrypt
decrypt(ciphertext, key, plaintext);  // Same key to decrypt
```

## Types of Cryptography

### 1. Hashing (One-Way Functions)

Converts data to fixed-size output. **Cannot be reversed.**

```
Input (any size) → Hash Function → Hash (fixed size)

"Hello"           →    MD5        → "8b1a9953c4611296a827abf8c47804d7"
"Hello World"     →    MD5        → "b10a8db164e0754105b7a99be72e3fe5"
```

**Properties:**
- Deterministic: Same input = same output
- One-way: Cannot reverse hash to get input
- Avalanche effect: Tiny change = completely different hash
- Collision resistant: Hard to find two inputs with same hash

**Uses:**
- Password storage
- File integrity checking
- Digital signatures
- Blockchain

```c
char password[] = "MyPassword123";
char hash[33];
md5(password, hash);
// Store hash, not password!
```

### 2. Symmetric Encryption (Same Key)

Uses **same key** for encryption and decryption.

```
Plaintext → Encrypt (Key) → Ciphertext → Decrypt (Key) → Plaintext
```

**Examples:** AES, DES, 3DES, ChaCha20

**Pros:**
- Fast
- Simple key management (if only 2 parties)

**Cons:**
- Key distribution problem
- If key leaked, all messages compromised

```c
char message[] = "Secret data";
char key[] = "shared_secret_key";
char encrypted[100];

aes_encrypt(message, key, encrypted);
// Both parties need the same key
aes_decrypt(encrypted, key, message);
```

### 3. Asymmetric Encryption (Key Pairs)

Uses **different keys**: public key (encrypt) and private key (decrypt).

```
Public Key (share freely)  → Encrypt
Private Key (keep secret)  → Decrypt
```

**Examples:** RSA, ECC, ElGamal

**How it works:**
```
Alice wants to send Bob a secret message:

1. Bob generates key pair:
   - Public key: Share with everyone
   - Private key: Keep secret

2. Alice encrypts with Bob's public key
   Plaintext → Encrypt (Bob's Public Key) → Ciphertext

3. Alice sends ciphertext (anyone can intercept, can't decrypt!)

4. Bob decrypts with his private key
   Ciphertext → Decrypt (Bob's Private Key) → Plaintext
```

**Pros:**
- Solves key distribution
- Digital signatures possible
- No need to share secrets

**Cons:**
- Slow (100-1000x slower than symmetric)
- Complex math (RSA, elliptic curves)

## Common Algorithms

### Hash Functions

| Algorithm | Output Size | Security | Speed |
|-----------|-------------|----------|-------|
| MD5 | 128 bits | ⚠️ BROKEN | Fast |
| SHA-1 | 160 bits | ⚠️ BROKEN | Fast |
| SHA-256 | 256 bits | ✅ Secure | Medium |
| SHA-512 | 512 bits | ✅ Secure | Medium |
| bcrypt | Variable | ✅ Secure | Slow (by design) |

### Symmetric Algorithms

| Algorithm | Key Size | Block Size | Security |
|-----------|----------|------------|----------|
| DES | 56 bits | 64 bits | ⚠️ BROKEN |
| 3DES | 168 bits | 64 bits | ⚠️ Deprecated |
| AES-128 | 128 bits | 128 bits | ✅ Secure |
| AES-256 | 256 bits | 128 bits | ✅ Secure |
| ChaCha20 | 256 bits | Stream | ✅ Secure |

### Asymmetric Algorithms

| Algorithm | Key Size | Security | Use Case |
|-----------|----------|----------|----------|
| RSA-1024 | 1024 bits | ⚠️ Weak | Legacy |
| RSA-2048 | 2048 bits | ✅ Secure | General |
| RSA-4096 | 4096 bits | ✅ Very Secure | High security |
| ECC-256 | 256 bits | ✅ Secure | Mobile/IoT |

## Key Sizes Matter

Longer keys = more security, but slower performance.

```c
// Symmetric encryption key strength
56-bit:   Broken in hours (brute force)
128-bit:  Secure (2^128 combinations)
256-bit:  Overkill for most uses

// Asymmetric (RSA) key strength
1024-bit: Factorable with effort
2048-bit: Current standard
4096-bit: Very secure
```

## Random Numbers

Crypto requires **cryptographically secure random numbers**.

```c
// BAD: Predictable!
srand(time(NULL));
int bad_key = rand();  // Don't use for crypto!

// GOOD: Cryptographically secure
int good_key = secure_random();  // Uses /dev/urandom, CryptGenRandom, etc.
```

**Why it matters:**
- Weak RNG = predictable keys
- Predictable keys = broken encryption
- Use OS-provided crypto RNG!

## Encoding vs Encryption

**Encoding** (Base64, hex) is NOT encryption!

```c
// Encoding: Reversible without key
char data[] = "Secret";
char encoded[100];
base64_encode(data, encoded);  // "U2VjcmV0"
// Anyone can decode this!

// Encryption: Requires key
char data[] = "Secret";
char key[] = "MyKey";
char encrypted[100];
aes_encrypt(data, key, encrypted);  // "X7@k..."
// Need key to decrypt!
```

## Salt and IV

### Salt (for hashing)
Random data added to password before hashing.

```c
char password[] = "password123";
char salt[] = "randomsalt456";
char hash[65];

// Without salt (BAD)
sha256(password, hash);
// Same password always = same hash
// Vulnerable to rainbow tables!

// With salt (GOOD)
sha256_with_salt(password, salt, hash);
// Same password + different salt = different hash
// Rainbow tables useless!
```

### IV (Initialization Vector)
Random data used to start encryption. Prevents identical plaintexts from producing identical ciphertexts.

```c
char plaintext[] = "Hello";
char key[] = "SecretKey";
char iv[16];  // Random IV

generate_random_iv(iv);
aes_cbc_encrypt(plaintext, key, iv, ciphertext);
// Store IV with ciphertext (IV doesn't need to be secret)
```

## Message Authentication

Ensure message hasn't been tampered with.

### HMAC (Hash-based MAC)
```c
char message[] = "Transfer $1000 to Alice";
char key[] = "shared_secret";
char mac[32];

hmac_sha256(message, key, mac);
// Send: message + mac
// Receiver verifies MAC with same key
// If MAC doesn't match = message tampered!
```

## Digital Signatures

Prove who sent a message (asymmetric equivalent of MAC).

```c
// Alice signs message
char message[] = "I agree to terms";
char signature[256];
rsa_sign(message, alice_private_key, signature);
// Send: message + signature

// Anyone can verify with Alice's public key
int valid = rsa_verify(message, signature, alice_public_key);
// valid = true → Alice definitely signed this
```

## Common Attacks

### 1. Brute Force
Try every possible key.

**Defense:** Long keys (128+ bits for symmetric, 2048+ for RSA)

### 2. Dictionary Attack
Try common passwords.

**Defense:** Strong passwords, rate limiting

### 3. Rainbow Tables
Precomputed hash tables.

**Defense:** Salt passwords before hashing

### 4. Man-in-the-Middle
Intercept and modify messages.

**Defense:** Encryption + authentication (TLS, signatures)

### 5. Side-Channel Attacks
Timing, power analysis, etc.

**Defense:** Constant-time operations (hard in C!)

## Security Principles

### 1. Kerckhoffs's Principle
**"System should be secure even if everything except the key is public"**

- Algorithm can be public
- Security depends ONLY on key secrecy
- Don't rely on "security through obscurity"

### 2. Defense in Depth
Multiple layers of security.

### 3. Least Privilege
Only give minimum required access.

### 4. Fail Secure
If something breaks, fail to secure state (not open).

## Summary

| Technique | Purpose | Reversible? | Key Needed? |
|-----------|---------|-------------|-------------|
| Hash | Integrity, passwords | No | No |
| Symmetric | Confidentiality | Yes | Yes (same) |
| Asymmetric | Key exchange, signatures | Yes | Yes (different) |
| MAC/HMAC | Authentication | N/A | Yes |
| Encoding | Transport | Yes | No |

**Golden Rules:**
- Never implement crypto yourself (for production)
- Use established libraries (OpenSSL, libsodium)
- Keys must be secret and random
- Salt passwords, use strong hashes
- Encrypt then MAC (authenticate)

Understanding how crypto works is essential for every programmer. These fundamentals apply everywhere from HTTPS to password storage to blockchain!
