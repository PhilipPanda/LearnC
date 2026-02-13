/*
 * Symmetric Encryption
 * 
 * Learn symmetric cryptography:
 * - XOR cipher (simple encryption)
 * - Key-based encryption/decryption
 * - Demonstrating encryption concepts
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// XOR cipher (educational - not secure for production!)
void xor_encrypt_decrypt(uint8_t* data, size_t len, const uint8_t* key, size_t key_len) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key[i % key_len];
    }
}

// Simple substitution cipher
void substitution_encrypt(char* text, int shift) {
    for (int i = 0; text[i]; i++) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] = 'a' + (text[i] - 'a' + shift) % 26;
        } else if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i] = 'A' + (text[i] - 'A' + shift) % 26;
        }
    }
}

void substitution_decrypt(char* text, int shift) {
    substitution_encrypt(text, 26 - shift);  // Reverse shift
}

// Print bytes as hex
void print_hex(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
}

// Print bytes as hex with spaces
void print_hex_spaced(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", data[i]);
    }
}

// Demonstrate encryption/decryption
void demo_xor_cipher() {
    printf("=== XOR Cipher Demo ===\n\n");
    
    char plaintext[] = "Secret Message!";
    char key[] = "MyKey";
    
    printf("Plaintext: \"%s\"\n", plaintext);
    printf("Key:       \"%s\"\n\n", key);
    
    // Encrypt
    printf("Encrypting...\n");
    xor_encrypt_decrypt((uint8_t*)plaintext, strlen(plaintext), (uint8_t*)key, strlen(key));
    
    printf("Encrypted (hex): ");
    print_hex((uint8_t*)plaintext, strlen(plaintext));
    printf("\n");
    printf("Encrypted (raw): \"");
    for (int i = 0; i < strlen(plaintext); i++) {
        printf("%c", plaintext[i] >= 32 && plaintext[i] < 127 ? plaintext[i] : '.');
    }
    printf("\"\n\n");
    
    // Decrypt
    printf("Decrypting...\n");
    xor_encrypt_decrypt((uint8_t*)plaintext, strlen(plaintext), (uint8_t*)key, strlen(key));
    
    printf("Decrypted: \"%s\"\n", plaintext);
}

void demo_caesar_cipher() {
    printf("\n=== Caesar Cipher Demo ===\n\n");
    
    char message[] = "HELLO WORLD";
    int shift = 3;
    
    printf("Plaintext: \"%s\"\n", message);
    printf("Shift:     %d\n\n", shift);
    
    printf("Encrypting...\n");
    substitution_encrypt(message, shift);
    printf("Encrypted: \"%s\"\n\n", message);
    
    printf("Decrypting...\n");
    substitution_decrypt(message, shift);
    printf("Decrypted: \"%s\"\n", message);
}

void demo_key_sensitivity() {
    printf("\n=== Key Sensitivity Demo ===\n\n");
    printf("Wrong key = wrong decryption!\n\n");
    
    char message[] = "Secret!";
    char correct_key[] = "KEY123";
    char wrong_key[] = "KEY124";  // Only last char different
    
    char encrypted[100];
    strcpy(encrypted, message);
    
    printf("Plaintext:     \"%s\"\n", message);
    printf("Correct key:   \"%s\"\n", correct_key);
    printf("Wrong key:     \"%s\"\n\n", wrong_key);
    
    // Encrypt with correct key
    xor_encrypt_decrypt((uint8_t*)encrypted, strlen(encrypted), 
                       (uint8_t*)correct_key, strlen(correct_key));
    printf("Encrypted: ");
    print_hex((uint8_t*)encrypted, strlen(message));
    printf("\n\n");
    
    // Try to decrypt with wrong key
    char wrong_decrypt[100];
    strcpy(wrong_decrypt, encrypted);
    xor_encrypt_decrypt((uint8_t*)wrong_decrypt, strlen(message),
                       (uint8_t*)wrong_key, strlen(wrong_key));
    printf("Decrypt with WRONG key: \"%s\" (garbage!)\n", wrong_decrypt);
    
    // Decrypt with correct key
    char correct_decrypt[100];
    strcpy(correct_decrypt, encrypted);
    xor_encrypt_decrypt((uint8_t*)correct_decrypt, strlen(message),
                       (uint8_t*)correct_key, strlen(correct_key));
    printf("Decrypt with RIGHT key: \"%s\" (success!)\n", correct_decrypt);
}

void demo_file_encryption() {
    printf("\n=== File Encryption Demo ===\n\n");
    
    // Create test file
    const char* filename = "secret.txt";
    const char* enc_filename = "secret.txt.enc";
    const char* dec_filename = "secret_decrypted.txt";
    
    FILE* f = fopen(filename, "w");
    fprintf(f, "This is a secret message!\nLine 2\nLine 3");
    fclose(f);
    
    printf("Created: %s\n", filename);
    
    // Read file
    f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* file_data = malloc(file_size);
    fread(file_data, 1, file_size, f);
    fclose(f);
    
    // Encrypt
    const char* key = "MyEncryptionKey123";
    printf("Encrypting with key: \"%s\"\n", key);
    xor_encrypt_decrypt(file_data, file_size, (uint8_t*)key, strlen(key));
    
    // Write encrypted file
    f = fopen(enc_filename, "wb");
    fwrite(file_data, 1, file_size, f);
    fclose(f);
    printf("Created: %s (encrypted)\n\n", enc_filename);
    
    // Decrypt
    printf("Decrypting...\n");
    xor_encrypt_decrypt(file_data, file_size, (uint8_t*)key, strlen(key));
    
    // Write decrypted file
    f = fopen(dec_filename, "wb");
    fwrite(file_data, 1, file_size, f);
    fclose(f);
    printf("Created: %s (decrypted)\n", dec_filename);
    
    free(file_data);
    
    printf("\nCompare files to verify:\n");
    printf("  - %s (original)\n", filename);
    printf("  - %s (encrypted - unreadable)\n", enc_filename);
    printf("  - %s (decrypted - should match original)\n", dec_filename);
}

int main(void) {
    printf("=== Symmetric Encryption ===\n\n");
    printf("This demonstrates basic encryption concepts.\n");
    printf("Note: These are educational examples, not production-ready!\n\n");
    
    // Run demos
    demo_xor_cipher();
    demo_caesar_cipher();
    demo_key_sensitivity();
    demo_file_encryption();
    
    printf("\n=== Summary ===\n");
    printf("Key concepts demonstrated:\n");
    printf("  1. Symmetric encryption uses same key for encrypt/decrypt\n");
    printf("  2. XOR cipher is simplest (but not secure!)\n");
    printf("  3. Wrong key = garbage output\n");
    printf("  4. Can encrypt files, text, any data\n");
    printf("  5. Key must be kept secret\n");
    printf("\nReal-world symmetric algorithms:\n");
    printf("  - AES (Advanced Encryption Standard)\n");
    printf("  - ChaCha20\n");
    printf("  - Blowfish\n");
    printf("\nThese educational examples show HOW encryption works,\n");
    printf("but use established libraries (OpenSSL) for production!\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
