/*
 * 05_functions.c
 * 
 * Functions let you organize code into reusable pieces.
 */

#include <stdio.h>

// Function declaration (tells compiler function exists)
int add(int a, int b);
void greet(const char* name);
int square(int n);
void print_separator(void);

int main(void) {
    // Call functions
    printf("=== Basic Functions ===\n");
    int result = add(5, 3);
    printf("5 + 3 = %d\n", result);
    
    greet("Alice");
    greet("Bob");
    
    printf("\n=== Return Values ===\n");
    printf("Square of 7: %d\n", square(7));
    printf("Square of 12: %d\n", square(12));
    
    print_separator();
    printf("Done!\n");
    
    return 0;
}

// Function definitions (actual code)

// Returns sum of two numbers
int add(int a, int b) {
    return a + b;
}

// Returns nothing (void), just prints
void greet(const char* name) {
    printf("Hello, %s!\n", name);
}

// Takes one parameter, returns its square
int square(int n) {
    return n * n;
}

// No parameters, no return value
void print_separator(void) {
    printf("------------------------\n");
}

/*
 * Function anatomy:
 * 
 * return_type function_name(parameter_type parameter_name) {
 *     // function body
 *     return value;  // if return_type isn't void
 * }
 * 
 * Parts:
 *   return_type    - What the function gives back (int, void, etc.)
 *   function_name  - What you call it
 *   parameters     - Inputs to the function
 *   return         - Exit function and give back a value
 * 
 * void means "nothing":
 *   void func()           - returns nothing
 *   func(void)            - takes no parameters
 * 
 * Declaration vs Definition:
 *   Declaration - Says function exists (at top of file)
 *   Definition  - Actual implementation
 * 
 * Try:
 *   - Write a function that multiplies two numbers
 *   - Write a function that prints numbers 1 to n
 *   - Call functions from within other functions
 */
