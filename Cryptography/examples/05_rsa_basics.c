/*
 * RSA Encryption Basics
 * 
 * Learn public-key cryptography:
 * - Key pair generation
 * - Encryption with public key
 * - Decryption with private key
 * - Digital signatures
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Simple RSA implementation (educational - small numbers only!)

typedef struct {
    uint64_t n;  // Modulus
    uint64_t e;  // Public exponent
} PublicKey;

typedef struct {
    uint64_t n;  // Modulus
    uint64_t d;  // Private exponent
} PrivateKey;

// Greatest Common Divisor
uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Extended Euclidean Algorithm (for modular inverse)
int64_t extended_gcd(int64_t a, int64_t b, int64_t* x, int64_t* y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    
    int64_t x1, y1;
    int64_t gcd_val = extended_gcd(b, a % b, &x1, &y1);
    
    *x = y1;
    *y = x1 - (a / b) * y1;
    
    return gcd_val;
}

// Modular inverse
uint64_t mod_inverse(uint64_t a, uint64_t m) {
    int64_t x, y;
    int64_t g = extended_gcd(a, m, &x, &y);
    
    if (g != 1) {
        return 0;  // No inverse exists
    }
    
    return (x % m + m) % m;
}

// Modular exponentiation (efficient)
uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (__uint128_t)result * base % mod;
        }
        exp = exp >> 1;
        base = (__uint128_t)base * base % mod;
    }
    
    return result;
}

// Check if number is prime (simple trial division)
int is_prime(uint64_t n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    
    for (uint64_t i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// Generate RSA key pair
void generate_keypair(PublicKey* pub, PrivateKey* priv) {
    // Use small primes for demonstration
    // Real RSA uses 1024+ bit primes!
    uint64_t p = 61;
    uint64_t q = 53;
    
    printf("Choosing primes: p = %llu, q = %llu\n", p, q);
    
    // n = p * q
    uint64_t n = p * q;
    printf("n = p * q = %llu\n", n);
    
    // φ(n) = (p-1)(q-1)
    uint64_t phi = (p - 1) * (q - 1);
    printf("φ(n) = (p-1)(q-1) = %llu\n", phi);
    
    // Choose e (commonly 65537, but we'll use 17 for this example)
    uint64_t e = 17;
    printf("Public exponent e = %llu\n", e);
    
    // Calculate d (modular inverse of e mod φ(n))
    uint64_t d = mod_inverse(e, phi);
    printf("Private exponent d = %llu\n\n", d);
    
    // Public key
    pub->n = n;
    pub->e = e;
    
    // Private key
    priv->n = n;
    priv->d = d;
}

// RSA encrypt (use public key)
uint64_t rsa_encrypt(uint64_t plaintext, PublicKey* pub) {
    return mod_pow(plaintext, pub->e, pub->n);
}

// RSA decrypt (use private key)
uint64_t rsa_decrypt(uint64_t ciphertext, PrivateKey* priv) {
    return mod_pow(ciphertext, priv->d, priv->n);
}

// RSA sign (use private key)
uint64_t rsa_sign(uint64_t message, PrivateKey* priv) {
    return mod_pow(message, priv->d, priv->n);
}

// RSA verify (use public key)
int rsa_verify(uint64_t message, uint64_t signature, PublicKey* pub) {
    uint64_t decrypted = mod_pow(signature, pub->e, pub->n);
    return decrypted == message;
}

void demo_key_generation() {
    printf("=== RSA Key Generation ===\n\n");
    
    PublicKey pub;
    PrivateKey priv;
    
    generate_keypair(&pub, &priv);
    
    printf("Generated keys:\n");
    printf("  Public key:  (n=%llu, e=%llu)\n", pub.n, pub.e);
    printf("  Private key: (n=%llu, d=%llu)\n\n", priv.n, priv.d);
    
    printf("Public key can be shared with anyone.\n");
    printf("Private key must be kept secret!\n");
}

void demo_encryption() {
    printf("\n=== RSA Encryption/Decryption ===\n\n");
    
    PublicKey pub;
    PrivateKey priv;
    generate_keypair(&pub, &priv);
    
    uint64_t message = 42;
    printf("Original message: %llu\n\n", message);
    
    // Encrypt with PUBLIC key
    printf("Alice encrypts with Bob's PUBLIC key...\n");
    uint64_t encrypted = rsa_encrypt(message, &pub);
    printf("Encrypted: %llu\n\n", encrypted);
    
    // Decrypt with PRIVATE key
    printf("Bob decrypts with his PRIVATE key...\n");
    uint64_t decrypted = rsa_decrypt(encrypted, &priv);
    printf("Decrypted: %llu\n\n", decrypted);
    
    if (message == decrypted) {
        printf("✓ Success! Original message recovered.\n");
    }
    
    printf("\nKey concept:\n");
    printf("  Encrypt with PUBLIC key  → Only PRIVATE key can decrypt\n");
    printf("  Anyone can send secrets → Only recipient can read\n");
}

void demo_confidentiality() {
    printf("\n=== Confidentiality Demo ===\n\n");
    
    // Bob generates keys
    PublicKey bob_pub;
    PrivateKey bob_priv;
    printf("Bob generates key pair:\n");
    generate_keypair(&bob_pub, &bob_priv);
    printf("Bob shares public key: (n=%llu, e=%llu)\n\n", bob_pub.n, bob_pub.e);
    
    // Alice wants to send Bob a secret
    uint64_t secret = 99;
    printf("Alice has secret message: %llu\n", secret);
    printf("Alice encrypts with Bob's public key...\n");
    uint64_t encrypted = rsa_encrypt(secret, &bob_pub);
    printf("Encrypted message: %llu\n\n", encrypted);
    
    printf("Alice sends encrypted message over insecure channel...\n");
    printf("Eve intercepts: %llu\n", encrypted);
    printf("Eve has public key: (n=%llu, e=%llu)\n", bob_pub.n, bob_pub.e);
    printf("But Eve CANNOT decrypt (needs private key)!\n\n");
    
    printf("Bob receives encrypted message: %llu\n", encrypted);
    printf("Bob decrypts with his private key...\n");
    uint64_t decrypted = rsa_decrypt(encrypted, &bob_priv);
    printf("Decrypted: %llu\n\n", decrypted);
    
    printf("✓ Only Bob can read the message!\n");
}

void demo_digital_signatures() {
    printf("\n=== Digital Signatures ===\n\n");
    
    // Alice generates keys
    PublicKey alice_pub;
    PrivateKey alice_priv;
    printf("Alice generates key pair:\n");
    generate_keypair(&alice_pub, &alice_priv);
    printf("Alice shares public key: (n=%llu, e=%llu)\n\n", alice_pub.n, alice_pub.e);
    
    // Alice wants to sign a message
    uint64_t message = 77;
    printf("Alice's message: %llu\n", message);
    printf("Alice signs with her PRIVATE key...\n");
    uint64_t signature = rsa_sign(message, &alice_priv);
    printf("Signature: %llu\n\n", signature);
    
    printf("Alice sends: message=%llu, signature=%llu\n\n", message, signature);
    
    // Bob verifies signature
    printf("Bob verifies signature with Alice's PUBLIC key...\n");
    if (rsa_verify(message, signature, &alice_pub)) {
        printf("✓ Signature valid! Alice definitely signed this.\n");
    } else {
        printf("✗ Signature invalid! Forgery or tampering detected.\n");
    }
    
    // Try to forge signature
    printf("\nEve tries to forge signature...\n");
    uint64_t fake_signature = signature + 1;
    printf("Fake signature: %llu\n", fake_signature);
    
    if (rsa_verify(message, fake_signature, &alice_pub)) {
        printf("✓ Signature valid!\n");
    } else {
        printf("✗ Forgery detected!\n");
    }
    
    printf("\nKey concept:\n");
    printf("  Sign with PRIVATE key → Anyone with PUBLIC key can verify\n");
    printf("  Proves authenticity and integrity\n");
}

void demo_key_exchange() {
    printf("\n=== Key Exchange Scenario ===\n\n");
    
    printf("Problem: Alice and Bob want to communicate securely.\n");
    printf("They've never met and can only use insecure channel.\n\n");
    
    // Bob generates keys
    PublicKey bob_pub;
    PrivateKey bob_priv;
    printf("Step 1: Bob generates key pair\n");
    generate_keypair(&bob_pub, &bob_priv);
    
    printf("Step 2: Bob sends PUBLIC key to Alice\n");
    printf("        Public key: (n=%llu, e=%llu)\n", bob_pub.n, bob_pub.e);
    printf("        (Anyone can see this, it's OK!)\n\n");
    
    printf("Step 3: Alice generates symmetric key for fast encryption\n");
    uint64_t symmetric_key = 123;  // In reality: random AES-256 key
    printf("        Symmetric key: %llu\n", symmetric_key);
    
    printf("Step 4: Alice encrypts symmetric key with Bob's public key\n");
    uint64_t encrypted_key = rsa_encrypt(symmetric_key, &bob_pub);
    printf("        Encrypted: %llu\n\n", encrypted_key);
    
    printf("Step 5: Alice sends encrypted key to Bob\n");
    printf("        (Eve can intercept, but can't decrypt)\n\n");
    
    printf("Step 6: Bob decrypts with his private key\n");
    uint64_t bob_key = rsa_decrypt(encrypted_key, &bob_priv);
    printf("        Decrypted symmetric key: %llu\n\n", bob_key);
    
    printf("✓ Alice and Bob now share symmetric key: %llu\n", symmetric_key);
    printf("✓ They can use fast symmetric encryption (AES) for messages\n");
    printf("✓ This is how HTTPS works!\n");
}

void demo_limitations() {
    printf("\n=== RSA Limitations ===\n\n");
    
    PublicKey pub;
    PrivateKey priv;
    generate_keypair(&pub, &priv);
    
    printf("Message size limit:\n");
    printf("  RSA can only encrypt numbers smaller than n\n");
    printf("  With n=%llu, max message = %llu\n", pub.n, pub.n - 1);
    printf("  Real RSA (2048-bit) has larger n\n\n");
    
    printf("Speed:\n");
    printf("  RSA is 100-1000x slower than AES\n");
    printf("  Used for key exchange, not bulk encryption\n\n");
    
    printf("Security:\n");
    printf("  This example uses tiny 6-bit primes\n");
    printf("  Real RSA uses 1024-4096 bit primes\n");
    printf("  Security depends on difficulty of factoring n\n\n");
    
    printf("Hybrid encryption:\n");
    printf("  Use RSA to exchange symmetric key\n");
    printf("  Use AES for actual data encryption\n");
    printf("  Best of both worlds!\n");
}

int main(void) {
    printf("=== RSA Public-Key Cryptography ===\n\n");
    printf("Note: This uses small numbers for demonstration.\n");
    printf("Real RSA uses 2048+ bit numbers!\n\n");
    
    demo_key_generation();
    demo_encryption();
    demo_confidentiality();
    demo_digital_signatures();
    demo_key_exchange();
    demo_limitations();
    
    printf("\n=== Summary ===\n");
    printf("RSA enables secure communication without pre-shared secrets!\n\n");
    printf("Key concepts:\n");
    printf("  ✓ Two keys: public (share) and private (secret)\n");
    printf("  ✓ Encrypt with public → decrypt with private\n");
    printf("  ✓ Sign with private → verify with public\n");
    printf("  ✓ Based on difficulty of factoring large numbers\n");
    printf("  ✓ Slow, used for key exchange\n");
    printf("  ✓ Combined with symmetric crypto for performance\n\n");
    
    printf("Real-world uses:\n");
    printf("  - HTTPS/TLS (website encryption)\n");
    printf("  - SSH (secure shell)\n");
    printf("  - PGP/GPG (email encryption)\n");
    printf("  - Digital signatures\n");
    printf("  - Code signing\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
