# Secure Coding Practices

## The Big Picture

Knowing crypto algorithms isn't enough - you must use them correctly and avoid common vulnerabilities. This guide covers practical security principles for C programming.

## Common Vulnerabilities

### 1. Buffer Overflows

```c
// BAD:
char password[10];
gets(password);  // ❌ No bounds checking!

// GOOD:
char password[10];
fgets(password, sizeof(password), stdin);  // ✅ Bounded
```

### 2. Using rand() for Crypto

```c
// BAD:
srand(time(NULL));
int key = rand();  // ❌ Predictable!

// GOOD (Windows):
#include <windows.h>
#include <bcrypt.h>
uint8_t key[32];
BCryptGenRandom(NULL, key, 32, BCRYPT_USE_SYSTEM_PREFERRED_RNG);

// GOOD (Linux):
#include <sys/random.h>
uint8_t key[32];
getrandom(key, 32, 0);
```

### 3. Hardcoded Keys/Passwords

```c
// BAD:
const char* password = "admin123";  // ❌ In source code!

// GOOD:
// Read from environment variable or secure config
const char* password = getenv("DB_PASSWORD");
```

### 4. Not Clearing Sensitive Data

```c
// BAD:
char password[100];
get_password(password);
authenticate(password);
// password still in memory! ❌

// GOOD:
char password[100];
get_password(password);
authenticate(password);
memset(password, 0, sizeof(password));  // ✅ Clear it
// Even better: use volatile or explicit_bzero
```

### 5. Time-of-Check to Time-of-Use (TOCTOU)

```c
// BAD:
if (access("file.txt", R_OK) == 0) {
    // Race condition here! File could change
    FILE* f = fopen("file.txt", "r");  // ❌
}

// GOOD:
FILE* f = fopen("file.txt", "r");
if (f != NULL) {
    // Check and use atomically ✅
}
```

## Password Security

### Storing Passwords

```c
// NEVER:
store_password("alice", "password123");  // ❌❌❌

// NEVER:
char hash[33];
md5("password123", hash);
store_password("alice", hash);  // ❌ Rainbow tables!

// BETTER:
char salt[17];
generate_salt(salt);
char hash[65];
sha256_with_salt("password123", salt, hash);
store_password("alice", salt, hash);  // ✅ But still not ideal

// BEST:
char hash[61];
bcrypt("password123", 12, hash);  // Work factor = 12
store_password("alice", hash);  // ✅✅✅
// Salt included in hash, slow by design
```

### Password Requirements

```c
int is_strong_password(const char* password) {
    int len = strlen(password);
    if (len < 12) return 0;  // Minimum 12 characters
    
    int has_upper = 0, has_lower = 0, has_digit = 0, has_special = 0;
    
    for (int i = 0; i < len; i++) {
        if (isupper(password[i])) has_upper = 1;
        if (islower(password[i])) has_lower = 1;
        if (isdigit(password[i])) has_digit = 1;
        if (ispunct(password[i])) has_special = 1;
    }
    
    return has_upper && has_lower && has_digit && has_special;
}
```

## Input Validation

```c
// Always validate user input!

int validate_username(const char* username) {
    int len = strlen(username);
    
    // Length check
    if (len < 3 || len > 20) return 0;
    
    // Character check (alphanumeric only)
    for (int i = 0; i < len; i++) {
        if (!isalnum(username[i]) && username[i] != '_') {
            return 0;
        }
    }
    
    return 1;
}
```

## Timing Attacks

Comparing secrets byte-by-byte leaks information through timing.

```c
// BAD: Early exit leaks position of first mismatch
int compare_bad(const char* a, const char* b, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (a[i] != b[i]) {
            return 0;  // ❌ Early exit = timing leak
        }
    }
    return 1;
}

// GOOD: Constant-time comparison
int compare_constant_time(const uint8_t* a, const uint8_t* b, size_t len) {
    volatile uint8_t result = 0;
    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];
    }
    return result == 0;  // ✅ Always checks all bytes
}

// Use for:
// - Password verification
// - MAC verification
// - Any secret comparison
```

## Integer Overflow

```c
// BAD:
size_t size1 = user_input1;
size_t size2 = user_input2;
char* buffer = malloc(size1 + size2);  // ❌ Overflow!

// GOOD:
if (size1 > SIZE_MAX - size2) {
    return -1;  // Overflow would occur
}
size_t total = size1 + size2;
char* buffer = malloc(total);  // ✅
```

## Format String Vulnerabilities

```c
// BAD:
char user_input[100];
gets(user_input);
printf(user_input);  // ❌ User controls format string!

// GOOD:
char user_input[100];
fgets(user_input, sizeof(user_input), stdin);
printf("%s", user_input);  // ✅ Safe
```

## Memory Safety

### Use-After-Free

```c
// BAD:
char* data = malloc(100);
free(data);
strcpy(data, "hello");  // ❌ Use after free!

// GOOD:
char* data = malloc(100);
strcpy(data, "hello");
free(data);
data = NULL;  // ✅ Prevent accidental reuse
```

### Double Free

```c
// BAD:
free(ptr);
free(ptr);  // ❌ Double free!

// GOOD:
free(ptr);
ptr = NULL;  // ✅ Prevent double free
```

## Secure Random Numbers

```c
// NEVER use rand() for security!

// Windows:
int secure_random_windows(uint8_t* buffer, size_t len) {
    HCRYPTPROV prov;
    if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, 0)) {
        return -1;
    }
    
    int result = CryptGenRandom(prov, len, buffer) ? 0 : -1;
    CryptReleaseContext(prov, 0);
    return result;
}

// Linux/Unix:
int secure_random_linux(uint8_t* buffer, size_t len) {
    FILE* urandom = fopen("/dev/urandom", "rb");
    if (!urandom) return -1;
    
    size_t read = fread(buffer, 1, len, urandom);
    fclose(urandom);
    
    return (read == len) ? 0 : -1;
}
```

## Key Storage

### Environment Variables
```c
const char* api_key = getenv("API_KEY");
if (!api_key) {
    fprintf(stderr, "API_KEY not set\n");
    return 1;
}
```

### Configuration Files
```c
// Store encrypted, or with restricted permissions
// chmod 600 config.key  (owner read/write only)
```

### Never in Source Code
```c
// ❌ NEVER DO THIS:
#define API_KEY "sk_live_abc123xyz"
const char* password = "admin123";
```

## Secure String Operations

```c
// Use bounded functions:
strncpy(dest, src, sizeof(dest) - 1);
dest[sizeof(dest) - 1] = '\0';

snprintf(buffer, sizeof(buffer), "User: %s", username);

// Instead of:
strcpy(dest, src);      // ❌ Unbounded
sprintf(buffer, "...");  // ❌ Unbounded
```

## Principle of Least Privilege

```c
// Drop privileges after initialization
if (getuid() == 0) {
    // Running as root, drop to normal user
    setuid(normal_user_id);
}
```

## Input Sanitization

```c
void sanitize_filename(char* filename) {
    // Remove dangerous characters
    for (int i = 0; filename[i]; i++) {
        if (filename[i] == '/' || filename[i] == '\\' ||
            filename[i] == '.' || filename[i] == ':') {
            filename[i] = '_';
        }
    }
}
```

## Side-Channel Resistance

### Cache Timing
```c
// Access patterns can leak secrets
// BAD: Branching based on secret
if (secret_key[i] == guess) {
    // Different execution time! ❌
}

// GOOD: Constant time lookup
result = lookup_table[secret_key[i]];  // ✅
```

## Checklist for Secure Code

✅ **Use strong crypto:** AES-256, SHA-256, RSA-2048+  
✅ **Random IVs/salts:** Never reuse, always random  
✅ **Authenticate:** Use HMAC or authenticated encryption  
✅ **Validate input:** Check all user input  
✅ **Bounds checking:** Use safe string functions  
✅ **Clear secrets:** Zero memory after use  
✅ **Constant-time:** Compare secrets in constant time  
✅ **Strong RNG:** Use OS crypto RNG  
✅ **Update libraries:** Keep crypto libraries current  
✅ **Don't roll your own:** Use established implementations  

## Summary

**Golden Rules:**
1. Never implement crypto yourself for production
2. Use established libraries (OpenSSL, libsodium, Crypto++)
3. Keep secrets out of source code
4. Validate and sanitize all input
5. Clear sensitive data from memory
6. Use constant-time comparisons
7. Keep software updated
8. Assume attacker has source code

**Common Mistakes:**
- Using `rand()` for crypto
- Reusing IVs
- ECB mode
- No authentication
- Storing plaintext passwords
- Timing attacks
- Buffer overflows

Security is hard - one mistake can compromise everything. Learn these principles and apply them rigorously!
