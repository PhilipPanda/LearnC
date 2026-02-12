/*
 * 07_strings.c
 * 
 * Strings in C are character arrays ending with '\0'.
 */

#include <stdio.h>
#include <string.h>  // String functions

int main(void) {
    // String declaration
    char name[] = "Alice";  // Creates: {'A','l','i','c','e','\0'}
    char greeting[20] = "Hello";
    
    printf("=== String Basics ===\n");
    printf("Name: %s\n", name);
    printf("Greeting: %s\n", greeting);
    
    // String length
    printf("\n=== String Functions ===\n");
    int len = strlen(name);
    printf("Length of '%s': %d\n", name, len);
    
    // Copy string
    char copy[20];
    strcpy(copy, name);
    printf("Copy: %s\n", copy);
    
    // Concatenate (append)
    char message[50] = "Hello, ";
    strcat(message, name);  // Adds name to end
    printf("Message: %s\n", message);
    
    // Compare strings (returns 0 if equal)
    if (strcmp(name, "Alice") == 0) {
        printf("Name is Alice\n");
    }
    
    // Strings are character arrays
    printf("\n=== Character Access ===\n");
    printf("First letter: %c\n", name[0]);
    printf("Last letter: %c\n", name[strlen(name)-1]);
    
    // Loop through string
    printf("Letters: ");
    for (int i = 0; name[i] != '\0'; i++) {
        printf("%c ", name[i]);
    }
    printf("\n");
    
    // Modify characters
    char word[] = "hello";
    word[0] = 'H';  // Capitalize
    printf("Modified: %s\n", word);
    
    // String input
    printf("\n=== Input ===\n");
    char input[50];
    printf("Enter your name: ");
    scanf("%49s", input);  // Read up to 49 chars (leave room for \0)
    printf("You entered: %s\n", input);
    
    // Clear input buffer before getchar
    while (getchar() != '\n');
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * String functions (need #include <string.h>):
 *   strlen(str)         - Get length (not including \0)
 *   strcpy(dest, src)   - Copy string
 *   strcat(dest, src)   - Append to string
 *   strcmp(s1, s2)      - Compare (0 if equal)
 * 
 * Important:
 *   - Strings MUST end with '\0' (null terminator)
 *   - "hello" is 6 bytes: 'h','e','l','l','o','\0'
 *   - Make buffers big enough for \0
 *   - Don't overflow buffers (use strncpy, snprintf)
 * 
 * Common mistakes:
 *   char str[5] = "hello";  // WRONG: no room for \0
 *   char str[6] = "hello";  // RIGHT: 5 chars + \0
 * 
 * String literals vs arrays:
 *   char *str = "hello";    - Pointer to read-only string
 *   char str[] = "hello";   - Modifiable array
 * 
 * Try:
 *   - Write a function to count vowels in a string
 *   - Reverse a string
 *   - Check if two strings are anagrams
 */
