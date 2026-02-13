# Cryptography in C

Learn cryptography fundamentals in C. Hash functions, encryption algorithms, digital signatures, and secure coding - everything you need to understand security and build secure applications.

## What you get

- Hash functions (MD5, SHA-256)
- Symmetric encryption (AES, XOR cipher)
- Asymmetric encryption (RSA basics)
- Password hashing (bcrypt-style)
- Base64 encoding/decoding
- Random number generation
- Message authentication codes (HMAC)

All examples implement algorithms from scratch. No external crypto libraries - pure C implementation for learning.

## Building

```bash
# Windows
cd examples
build_all.bat

# Linux / MSYS2
cd examples
./build_all.sh
```

## Documentation

- **[Crypto Basics](docs/CRYPTO_BASICS.md)** - Encryption, hashing, keys, security concepts
- **[Hash Functions](docs/HASH_FUNCTIONS.md)** - MD5, SHA-256, collision resistance
- **[Symmetric Encryption](docs/SYMMETRIC_CRYPTO.md)** - AES, block ciphers, modes of operation
- **[Asymmetric Encryption](docs/ASYMMETRIC_CRYPTO.md)** - RSA, public/private keys, digital signatures
- **[Secure Coding](docs/SECURE_CODING.md)** - Best practices, common vulnerabilities

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_hash_functions | MD5, SHA-256, file hashing, integrity verification |
| 02_symmetric_crypto | XOR cipher, simple encryption/decryption |
| 03_password_hashing | Secure password storage, salting, key derivation |
| 04_base64 | Base64 encoding/decoding for data transmission |
| 05_rsa_basics | RSA encryption, public/private keys, signatures |
| 06_file_encryption | Encrypt/decrypt files securely |

Start with 01 and 02 to understand the fundamentals, then progress through the more advanced topics.

## What this teaches

- Cryptographic primitives (hashing, encryption)
- How security algorithms work internally
- Secure random number generation
- Key management and storage
- Common attack vectors and defenses
- Practical security implementation
- Bit manipulation and byte operations

After this, you'll understand how security works at a low level and can implement secure systems, understand vulnerabilities, and build cryptographic tools.

## Quick Start

```bash
cd examples
build_all.bat

# Hash a file
bin\01_hash_functions.exe yourfile.txt

# Encrypt a message
bin\02_symmetric_crypto.exe

# Test password hashing
bin\03_password_hashing.exe
```

## ⚠️ Important Security Notice

**These implementations are for EDUCATIONAL PURPOSES ONLY!**

- Do NOT use these in production systems
- Use established libraries (OpenSSL, libsodium) for real applications
- These examples prioritize clarity over security
- Real crypto requires constant-time operations, side-channel protection, etc.

This module teaches HOW crypto works, not production-ready crypto implementation!

---

Cryptography is fundamental to modern computing - HTTPS, passwords, VPNs, digital signatures. Master these fundamentals and you'll understand the security that powers the internet!
