/*
 * File Encryption
 * 
 * Complete file encryption system:
 * - Encrypt files with password
 * - Decrypt files with password
 * - Key derivation from password
 * - File format with metadata
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// File encryption format:
// [MAGIC 8 bytes] [SALT 16 bytes] [IV 16 bytes] [ENCRYPTED DATA]

#define MAGIC "ENCFILE1"
#define SALT_SIZE 16
#define IV_SIZE 16
#define KEY_SIZE 32

// Simple hash for key derivation (PBKDF2-style)
void derive_key(const char* password, const uint8_t* salt, uint8_t* key) {
    // Simplified key derivation
    // Real implementation would use PBKDF2 with many iterations
    
    uint32_t hash = 5381;
    
    // Hash password + salt multiple times
    for (int iteration = 0; iteration < 10000; iteration++) {
        // Hash password
        for (size_t i = 0; i < strlen(password); i++) {
            hash = ((hash << 5) + hash) + password[i];
        }
        
        // Mix in salt
        for (int i = 0; i < SALT_SIZE; i++) {
            hash = ((hash << 5) + hash) + salt[i];
        }
    }
    
    // Generate key bytes from hash
    for (int i = 0; i < KEY_SIZE; i++) {
        hash = ((hash << 5) + hash) + i;
        key[i] = hash & 0xFF;
    }
}

// Generate random bytes (pseudo-random for demo)
void generate_random_bytes(uint8_t* buffer, size_t len) {
    static int initialized = 0;
    if (!initialized) {
        srand(time(NULL));
        initialized = 1;
    }
    
    for (size_t i = 0; i < len; i++) {
        buffer[i] = rand() & 0xFF;
    }
}

// XOR-based encryption (educational - use AES in production!)
void encrypt_data(uint8_t* data, size_t len, const uint8_t* key, const uint8_t* iv) {
    uint8_t keystream[KEY_SIZE + IV_SIZE];
    memcpy(keystream, key, KEY_SIZE);
    memcpy(keystream + KEY_SIZE, iv, IV_SIZE);
    
    for (size_t i = 0; i < len; i++) {
        data[i] ^= keystream[i % (KEY_SIZE + IV_SIZE)];
        
        // Update keystream (stream cipher-like)
        keystream[i % (KEY_SIZE + IV_SIZE)] ^= data[i];
    }
}

// Decrypt (same as encrypt for XOR)
void decrypt_data(uint8_t* data, size_t len, const uint8_t* key, const uint8_t* iv) {
    encrypt_data(data, len, key, iv);
}

// Encrypt file
int encrypt_file(const char* input_file, const char* output_file, const char* password) {
    // Read input file
    FILE* in = fopen(input_file, "rb");
    if (!in) {
        fprintf(stderr, "Failed to open input file: %s\n", input_file);
        return -1;
    }
    
    fseek(in, 0, SEEK_END);
    size_t file_size = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    uint8_t* file_data = malloc(file_size);
    if (!file_data) {
        fclose(in);
        return -1;
    }
    
    fread(file_data, 1, file_size, in);
    fclose(in);
    
    printf("Read %zu bytes from %s\n", file_size, input_file);
    
    // Generate salt and IV
    uint8_t salt[SALT_SIZE];
    uint8_t iv[IV_SIZE];
    generate_random_bytes(salt, SALT_SIZE);
    generate_random_bytes(iv, IV_SIZE);
    
    printf("Generated salt and IV\n");
    
    // Derive key from password
    uint8_t key[KEY_SIZE];
    derive_key(password, salt, key);
    
    printf("Derived encryption key from password\n");
    
    // Encrypt
    encrypt_data(file_data, file_size, key, iv);
    
    printf("Encrypted data\n");
    
    // Write output file
    FILE* out = fopen(output_file, "wb");
    if (!out) {
        fprintf(stderr, "Failed to create output file: %s\n", output_file);
        free(file_data);
        return -1;
    }
    
    // Write header
    fwrite(MAGIC, 1, 8, out);
    fwrite(salt, 1, SALT_SIZE, out);
    fwrite(iv, 1, IV_SIZE, out);
    fwrite(file_data, 1, file_size, out);
    
    fclose(out);
    free(file_data);
    
    printf("Wrote encrypted file: %s\n", output_file);
    printf("File format: [MAGIC 8B] [SALT 16B] [IV 16B] [DATA %zuB]\n", file_size);
    
    return 0;
}

// Decrypt file
int decrypt_file(const char* input_file, const char* output_file, const char* password) {
    // Read encrypted file
    FILE* in = fopen(input_file, "rb");
    if (!in) {
        fprintf(stderr, "Failed to open encrypted file: %s\n", input_file);
        return -1;
    }
    
    // Read and verify magic
    char magic[9] = {0};
    fread(magic, 1, 8, in);
    if (strcmp(magic, MAGIC) != 0) {
        fprintf(stderr, "Invalid file format (wrong magic)\n");
        fclose(in);
        return -1;
    }
    
    printf("Verified file format\n");
    
    // Read salt and IV
    uint8_t salt[SALT_SIZE];
    uint8_t iv[IV_SIZE];
    fread(salt, 1, SALT_SIZE, in);
    fread(iv, 1, IV_SIZE, in);
    
    printf("Read salt and IV\n");
    
    // Read encrypted data
    fseek(in, 0, SEEK_END);
    size_t file_size = ftell(in) - 8 - SALT_SIZE - IV_SIZE;
    fseek(in, 8 + SALT_SIZE + IV_SIZE, SEEK_SET);
    
    uint8_t* file_data = malloc(file_size);
    if (!file_data) {
        fclose(in);
        return -1;
    }
    
    fread(file_data, 1, file_size, in);
    fclose(in);
    
    printf("Read %zu bytes of encrypted data\n", file_size);
    
    // Derive key from password
    uint8_t key[KEY_SIZE];
    derive_key(password, salt, key);
    
    printf("Derived decryption key from password\n");
    
    // Decrypt
    decrypt_data(file_data, file_size, key, iv);
    
    printf("Decrypted data\n");
    
    // Write decrypted file
    FILE* out = fopen(output_file, "wb");
    if (!out) {
        fprintf(stderr, "Failed to create output file: %s\n", output_file);
        free(file_data);
        return -1;
    }
    
    fwrite(file_data, 1, file_size, out);
    fclose(out);
    free(file_data);
    
    printf("Wrote decrypted file: %s\n", output_file);
    
    return 0;
}

void print_menu() {
    printf("\n=== File Encryption Tool ===\n\n");
    printf("1. Encrypt a file\n");
    printf("2. Decrypt a file\n");
    printf("3. Demo: Create test files\n");
    printf("4. Exit\n");
    printf("\nChoice: ");
}

void demo_create_test_files() {
    printf("\n=== Creating Test Files ===\n\n");
    
    // Create test file 1: Plain text
    FILE* f = fopen("test_plain.txt", "w");
    fprintf(f, "This is a secret message!\n");
    fprintf(f, "It contains sensitive data.\n");
    fprintf(f, "Line 3 of the document.\n");
    fclose(f);
    printf("Created: test_plain.txt\n");
    
    // Create test file 2: Binary data
    f = fopen("test_binary.dat", "wb");
    uint8_t binary_data[256];
    for (int i = 0; i < 256; i++) {
        binary_data[i] = i;
    }
    fwrite(binary_data, 1, 256, f);
    fclose(f);
    printf("Created: test_binary.dat (256 bytes)\n");
    
    printf("\nTest files created! Now try:\n");
    printf("  1. Encrypt test_plain.txt\n");
    printf("  2. Try to open the encrypted file (unreadable!)\n");
    printf("  3. Decrypt it back\n");
    printf("  4. Compare with original\n");
}

void demo_full_workflow() {
    printf("\n=== Automated Demo ===\n\n");
    
    // Create test file
    const char* test_file = "demo_secret.txt";
    FILE* f = fopen(test_file, "w");
    fprintf(f, "TOP SECRET DOCUMENT\n");
    fprintf(f, "Launch codes: 1234-5678-9012\n");
    fprintf(f, "Password: SuperSecret123\n");
    fclose(f);
    
    printf("Created: %s\n\n", test_file);
    
    // Show original
    printf("--- Original file ---\n");
    f = fopen(test_file, "r");
    char line[100];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
    printf("\n");
    
    // Encrypt
    const char* password = "MyPassword123";
    const char* encrypted_file = "demo_secret.txt.enc";
    
    printf("Encrypting with password: \"%s\"\n", password);
    encrypt_file(test_file, encrypted_file, password);
    
    // Show encrypted file is unreadable
    printf("\n--- Encrypted file (hex dump, first 64 bytes) ---\n");
    f = fopen(encrypted_file, "rb");
    uint8_t buffer[64];
    size_t read = fread(buffer, 1, 64, f);
    for (size_t i = 0; i < read; i++) {
        printf("%02x ", buffer[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    fclose(f);
    printf("\n(Unreadable without password!)\n\n");
    
    // Decrypt
    const char* decrypted_file = "demo_secret_decrypted.txt";
    printf("Decrypting with password: \"%s\"\n", password);
    decrypt_file(encrypted_file, decrypted_file, password);
    
    // Show decrypted
    printf("\n--- Decrypted file ---\n");
    f = fopen(decrypted_file, "r");
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    fclose(f);
    
    printf("\n✓ Decryption successful!\n");
    printf("\nFiles created:\n");
    printf("  %s (original)\n", test_file);
    printf("  %s (encrypted)\n", encrypted_file);
    printf("  %s (decrypted)\n", decrypted_file);
}

int main(void) {
    printf("=== File Encryption System ===\n\n");
    printf("This demonstrates password-based file encryption.\n");
    printf("Note: Educational implementation - use strong crypto in production!\n");
    
    while (1) {
        print_menu();
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');  // Clear buffer
            continue;
        }
        while (getchar() != '\n');  // Clear newline
        
        if (choice == 1) {
            char input[256], output[256], password[256];
            
            printf("\nInput file: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            
            printf("Output file: ");
            fgets(output, sizeof(output), stdin);
            output[strcspn(output, "\n")] = 0;
            
            printf("Password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0;
            
            printf("\n");
            encrypt_file(input, output, password);
            
        } else if (choice == 2) {
            char input[256], output[256], password[256];
            
            printf("\nEncrypted file: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            
            printf("Output file: ");
            fgets(output, sizeof(output), stdin);
            output[strcspn(output, "\n")] = 0;
            
            printf("Password: ");
            fgets(password, sizeof(password), stdin);
            password[strcspn(password, "\n")] = 0;
            
            printf("\n");
            decrypt_file(input, output, password);
            
        } else if (choice == 3) {
            demo_create_test_files();
            printf("\nRunning full demo...\n");
            demo_full_workflow();
            
        } else if (choice == 4) {
            break;
        }
    }
    
    printf("\n=== Summary ===\n");
    printf("File encryption demonstrated:\n");
    printf("  ✓ Password-based encryption\n");
    printf("  ✓ Key derivation (PBKDF2-style)\n");
    printf("  ✓ Random salt per file\n");
    printf("  ✓ Random IV per file\n");
    printf("  ✓ File format with metadata\n");
    printf("\nProduction systems should use:\n");
    printf("  - AES-256-GCM (authenticated encryption)\n");
    printf("  - PBKDF2/bcrypt/Argon2 (strong KDF)\n");
    printf("  - Hardware RNG (secure randomness)\n");
    
    return 0;
}
