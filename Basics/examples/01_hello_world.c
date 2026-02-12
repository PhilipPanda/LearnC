/*
 * 01_hello_world.c
 * 
 * The classic first program. Shows the basic structure of a C program.
 */

#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * What's happening:
 * 
 * #include <stdio.h>
 *   - Includes standard input/output library
 *   - Gives us printf() function
 * 
 * int main(void)
 *   - Entry point of every C program
 *   - Returns an integer (0 = success)
 *   - void = takes no arguments
 * 
 * printf("Hello, World!\n");
 *   - Prints text to console
 *   - \n = newline character
 * 
 * return 0;
 *   - Exit code (0 = success, non-zero = error)
 * 
 * Try:
 *   - Change the message
 *   - Add more printf statements
 *   - Try \n in different places
 */
