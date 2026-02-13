# Cryptography Examples

Complete implementations of cryptographic algorithms and secure coding practices in C.

## Building

### Windows

```cmd
build_all.bat
```

### Linux / MSYS2

```bash
chmod +x build_all.sh
./build_all.sh
```

Executables will be created in the `bin` directory.

## Running the Examples

```bash
# Hash functions
bin\01_hash_functions.exe
bin\01_hash_functions.exe <filename>  # Hash a file

# Symmetric encryption
bin\02_symmetric_crypto.exe

# Password hashing
bin\03_password_hashing.exe

# Base64 encoding
bin\04_base64.exe

# RSA encryption
bin\05_rsa_basics.exe

# File encryption
bin\06_file_encryption.exe
```

## Examples Overview

### 01_hash_functions.c
**What it teaches:** Cryptographic hashing

- MD5 implementation
- String hashing
- File integrity verification
- Avalanche effect demonstration
- Collision resistance concept
- Simple vs cryptographic hashes

**Try it:**
```bash
bin\01_hash_functions.exe
bin\01_hash_functions.exe myfile.txt
```

**Key concepts:** One-way functions, fixed output size, deterministic hashing

---

### 02_symmetric_crypto.c
**What it teaches:** Symmetric encryption fundamentals

- XOR cipher implementation
- Caesar cipher (substitution)
- Key sensitivity
- File encryption/decryption
- Encryption vs encoding

**Try it:**
```bash
bin\02_symmetric_crypto.exe
```

**Key concepts:** Same key for encrypt/decrypt, key must be secret, wrong key = garbage

---

### 03_password_hashing.c
**What it teaches:** Secure password storage

- Plain passwords are bad
- Salt generation and usage
- Slow hashing (PBKDF2-style)
- Password verification
- Rainbow table attacks
- Constant-time comparison
- Timing attacks

**Try it:**
```bash
bin\03_password_hashing.exe
```

**Key concepts:** Never store plaintext, always salt, use slow hashes (bcrypt)

---

### 04_base64.c
**What it teaches:** Base64 encoding/decoding

- Binary to ASCII conversion
- Encoding algorithm
- Decoding algorithm
- Padding
- Use cases (email, JWT, data URLs)
- Not encryption!

**Try it:**
```bash
bin\04_base64.exe
```

**Key concepts:** Encoding ≠ encryption, 33% size increase, reversible without key

---

### 05_rsa_basics.c
**What it teaches:** Public-key cryptography

- RSA key generation
- Public/private key pairs
- Encryption with public key
- Decryption with private key
- Digital signatures
- Key exchange
- Hybrid encryption

**Try it:**
```bash
bin\05_rsa_basics.exe
```

**Key concepts:** Two keys, encrypt with public, decrypt with private, sign with private

---

### 06_file_encryption.c
**What it teaches:** Complete encryption system

- Password-based encryption
- Key derivation (PBKDF2-style)
- Salt and IV generation
- File format design
- Encrypt/decrypt workflow
- Metadata handling

**Try it:**
```bash
bin\06_file_encryption.exe
# Follow interactive menu
# Option 3 runs automated demo
```

**Key concepts:** KDF, salt, IV, file format, metadata

---

## Learning Path

**Beginner:** Start with 01, 02, 04
- Understand hashing, basic encryption, encoding

**Intermediate:** Continue with 03, 06
- Secure password storage, file encryption

**Advanced:** Finish with 05
- Public-key cryptography, RSA

## Important Security Notes

⚠️ **These implementations are for EDUCATION ONLY!**

- Do NOT use in production systems
- Use established libraries (OpenSSL, libsodium)
- These prioritize clarity over security
- Real crypto requires:
  - Constant-time operations
  - Side-channel protection
  - Secure random number generation
  - Proper key management
  - Regular security audits

## What You'll Learn

After working through these examples, you'll understand:

- How cryptographic algorithms work internally
- Difference between hashing, encoding, and encryption
- Symmetric vs asymmetric encryption
- Why password security is hard
- How digital signatures work
- How HTTPS and secure communications work
- Common security pitfalls and how to avoid them

## Real-World Equivalents

| Example | Production Library |
|---------|-------------------|
| Hash functions | OpenSSL (SHA-256, SHA-512) |
| Symmetric crypto | OpenSSL (AES-256-GCM) |
| Password hashing | bcrypt, Argon2 |
| Base64 | Standard library implementations |
| RSA | OpenSSL (RSA-2048+) |
| File encryption | GPG, age, 7-Zip with AES |

## Troubleshooting

### Compilation Issues

**Math functions not found:**
```bash
# Add -lm flag
gcc -o program program.c -lm
```

**Int128 not supported:**
```c
// Replace __uint128_t with smaller types
// Or compile with 64-bit target
```

### Runtime Issues

**Random numbers always same:**
- `srand()` called with same seed
- Use crypto RNG in production

**Decryption produces garbage:**
- Wrong password
- Wrong key
- File corrupted

## Further Reading

**Documentation:**
- See `../docs/` for detailed explanations
- `CRYPTO_BASICS.md` - Overview
- `HASH_FUNCTIONS.md` - Hashing
- `SYMMETRIC_CRYPTO.md` - Symmetric encryption
- `ASYMMETRIC_CRYPTO.md` - Public-key crypto
- `SECURE_CODING.md` - Security best practices

**Books:**
- "Cryptography Engineering" by Ferguson, Schneier, Kohno
- "Serious Cryptography" by Aumasson
- "The Code Book" by Simon Singh

**Standards:**
- NIST Cryptographic Standards
- RFC 2104 (HMAC)
- RFC 8017 (RSA)

---

Master these fundamentals and you'll understand how security works at every level - from HTTPS to password storage to blockchain!
