/*
 * Password Hashing
 * 
 * Learn secure password storage:
 * - Why plain passwords are bad
 * - Salt generation and usage
 * - Slow hashing (PBKDF2-style)
 * - Password verification
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// Simple hash function (for demonstration)
uint32_t djb2_hash(const uint8_t* data, size_t len) {
    uint32_t hash = 5381;
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }
    return hash;
}

// Generate random salt
void generate_salt(char* salt, size_t len) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    srand(time(NULL));
    
    for (size_t i = 0; i < len - 1; i++) {
        salt[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    salt[len - 1] = '\0';
}

// Hash password with salt
void hash_password(const char* password, const char* salt, char* hash_output) {
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", salt, password);
    
    uint32_t hash = djb2_hash((uint8_t*)combined, strlen(combined));
    sprintf(hash_output, "%08x", hash);
}

// Slow hash (PBKDF2-style - intentionally slow)
void slow_hash_password(const char* password, const char* salt, int iterations, char* hash_output) {
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", salt, password);
    
    uint32_t hash = djb2_hash((uint8_t*)combined, strlen(combined));
    
    // Iterate many times (makes brute force slower)
    for (int i = 0; i < iterations; i++) {
        uint8_t temp[4];
        temp[0] = (hash >> 24) & 0xff;
        temp[1] = (hash >> 16) & 0xff;
        temp[2] = (hash >> 8) & 0xff;
        temp[3] = hash & 0xff;
        hash = djb2_hash(temp, 4);
    }
    
    sprintf(hash_output, "%08x", hash);
}

// Verify password
int verify_password(const char* password, const char* salt, const char* stored_hash) {
    char computed_hash[9];
    hash_password(password, salt, computed_hash);
    return strcmp(computed_hash, stored_hash) == 0;
}

// Constant-time comparison (prevents timing attacks)
int constant_time_compare(const char* a, const char* b) {
    if (strlen(a) != strlen(b)) return 0;
    
    volatile uint8_t result = 0;
    for (size_t i = 0; i < strlen(a); i++) {
        result |= a[i] ^ b[i];
    }
    return result == 0;
}

void demo_plaintext_bad() {
    printf("=== Demo 1: Plain Password Storage (BAD!) ===\n\n");
    
    // Simulated database
    const char* db_username = "alice";
    const char* db_password = "password123";
    
    printf("Database contents:\n");
    printf("  Username: %s\n", db_username);
    printf("  Password: %s\n\n", db_password);
    
    printf("❌ TERRIBLE! Anyone with database access sees passwords!\n");
    printf("❌ If database is leaked, all passwords compromised!\n");
    printf("❌ NEVER store plaintext passwords!\n");
}

void demo_simple_hash() {
    printf("\n=== Demo 2: Simple Hash (Better, but still bad) ===\n\n");
    
    const char* password = "password123";
    char hash[9];
    hash_password(password, "", hash);  // No salt
    
    printf("Password: %s\n", password);
    printf("Hash:     %s\n\n", hash);
    
    printf("✓ Better: Password not visible\n");
    printf("❌ Still bad: No salt = rainbow table vulnerable!\n");
    printf("❌ Same password always = same hash\n");
}

void demo_with_salt() {
    printf("\n=== Demo 3: Hash with Salt (Good!) ===\n\n");
    
    const char* password = "password123";
    char salt[17];
    generate_salt(salt, 17);
    char hash[9];
    hash_password(password, salt, hash);
    
    printf("Password: %s\n", password);
    printf("Salt:     %s (random, unique per user)\n", salt);
    printf("Hash:     %s\n\n", hash);
    
    printf("Database stores:\n");
    printf("  username: alice\n");
    printf("  salt:     %s\n", salt);
    printf("  hash:     %s\n\n", hash);
    
    printf("✓ Good: Salt prevents rainbow tables\n");
    printf("✓ Same password + different salt = different hash\n");
    
    // Show verification
    printf("\nVerification:\n");
    printf("  User enters: password123\n");
    if (verify_password("password123", salt, hash)) {
        printf("  ✓ Password correct!\n");
    }
    
    printf("  User enters: wrongpass\n");
    if (!verify_password("wrongpass", salt, hash)) {
        printf("  ✗ Password incorrect!\n");
    }
}

void demo_slow_hash() {
    printf("\n=== Demo 4: Slow Hash (Best!) ===\n\n");
    
    const char* password = "password123";
    char salt[17];
    generate_salt(salt, 17);
    
    int iterations = 100000;  // Real systems use 100,000+
    
    printf("Password:   %s\n", password);
    printf("Salt:       %s\n", salt);
    printf("Iterations: %d\n\n", iterations);
    
    printf("Hashing (this will take a moment)...\n");
    clock_t start = clock();
    
    char hash[9];
    slow_hash_password(password, salt, iterations, hash);
    
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("Hash:       %s\n", hash);
    printf("Time taken: %.3f seconds\n\n", time_spent);
    
    printf("✓ Best: Intentionally slow to prevent brute force\n");
    printf("✓ Takes %.0f ms per attempt\n", time_spent * 1000);
    printf("✓ Brute forcing 1 million passwords would take %.0f seconds\n", 
           time_spent * 1000000);
    
    printf("\nReal-world algorithms:\n");
    printf("  - bcrypt (recommended)\n");
    printf("  - scrypt (memory-hard)\n");
    printf("  - Argon2 (modern, winner of PHC)\n");
}

void demo_rainbow_table() {
    printf("\n=== Demo 5: Rainbow Table Attack ===\n\n");
    
    // Simulate rainbow table (precomputed hashes)
    printf("Simulated rainbow table (common passwords):\n");
    
    struct {
        const char* password;
        char hash[9];
    } rainbow_table[5];
    
    const char* common_passwords[] = {"password", "123456", "admin", "qwerty", "password123"};
    
    for (int i = 0; i < 5; i++) {
        rainbow_table[i].password = common_passwords[i];
        hash_password(common_passwords[i], "", rainbow_table[i].hash);
        printf("  %s → %s\n", rainbow_table[i].password, rainbow_table[i].hash);
    }
    
    printf("\n");
    
    // User's password (no salt)
    const char* user_password = "password123";
    char user_hash[9];
    hash_password(user_password, "", user_hash);
    
    printf("User's stored hash (no salt): %s\n\n", user_hash);
    
    // Attack: Check rainbow table
    printf("Attacker checks rainbow table...\n");
    for (int i = 0; i < 5; i++) {
        if (strcmp(user_hash, rainbow_table[i].hash) == 0) {
            printf("✗ MATCH FOUND! Password is: \"%s\"\n", rainbow_table[i].password);
            break;
        }
    }
    
    printf("\n❌ Without salt, common passwords are easily cracked!\n");
}

void demo_constant_time() {
    printf("\n=== Demo 6: Timing Attacks ===\n\n");
    
    const char* correct = "SecretHash123";
    const char* guess1 = "WrongHash000";   // Different from start
    const char* guess2 = "SecretHash000";  // Matches most, different at end
    
    printf("Correct hash:  %s\n", correct);
    printf("Guess 1:       %s (wrong from start)\n", guess1);
    printf("Guess 2:       %s (wrong at end)\n\n", guess2);
    
    // Non-constant time (BAD)
    printf("Non-constant time comparison:\n");
    printf("  Guess 1: ");
    clock_t start = clock();
    int match = (strcmp(correct, guess1) == 0);
    double time1 = (double)(clock() - start) / CLOCKS_PER_SEC * 1000000;
    printf("%s (%.0f µs)\n", match ? "Match" : "No match", time1);
    
    printf("  Guess 2: ");
    start = clock();
    match = (strcmp(correct, guess2) == 0);
    double time2 = (double)(clock() - start) / CLOCKS_PER_SEC * 1000000;
    printf("%s (%.0f µs)\n\n", match ? "Match" : "No match", time2);
    
    printf("⚠️  Timing difference can leak information!\n");
    printf("    Attacker can guess hash character-by-character\n\n");
    
    // Constant time (GOOD)
    printf("Constant-time comparison:\n");
    printf("  Always checks all characters, regardless of match\n");
    printf("  ✓ Prevents timing attacks\n");
}

int main(void) {
    printf("=== Password Hashing & Security ===\n\n");
    
    demo_plaintext_bad();
    demo_simple_hash();
    demo_with_salt();
    demo_slow_hash();
    demo_rainbow_table();
    demo_constant_time();
    
    printf("\n=== Key Takeaways ===\n");
    printf("✗ Never store plaintext passwords\n");
    printf("✗ Never use fast hashes (MD5, SHA-256) for passwords\n");
    printf("✓ Always use salt (random, unique per user)\n");
    printf("✓ Use slow hash functions (bcrypt, scrypt, Argon2)\n");
    printf("✓ Use constant-time comparison for secrets\n");
    printf("✓ Higher iteration count = slower brute force\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
