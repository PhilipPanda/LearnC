/*
 * 03_operators.c
 * 
 * Operators let you manipulate data.
 */

#include <stdio.h>

int main(void) {
    // Arithmetic operators
    int a = 10;
    int b = 3;
    
    printf("=== Arithmetic ===\n");
    printf("%d + %d = %d\n", a, b, a + b);
    printf("%d - %d = %d\n", a, b, a - b);
    printf("%d * %d = %d\n", a, b, a * b);
    printf("%d / %d = %d\n", a, b, a / b);  // 3 (not 3.333!)
    printf("%d %% %d = %d\n", a, b, a % b);  // Remainder
    
    // Increment/decrement
    printf("\n=== Increment/Decrement ===\n");
    int x = 5;
    printf("x = %d\n", x);
    x++;  // x = x + 1
    printf("After x++: %d\n", x);
    x--;  // x = x - 1
    printf("After x--: %d\n", x);
    
    // Compound assignment
    printf("\n=== Compound Assignment ===\n");
    int n = 10;
    n += 5;  // Same as: n = n + 5
    printf("After n += 5: %d\n", n);
    n *= 2;  // Same as: n = n * 2
    printf("After n *= 2: %d\n", n);
    
    // Comparison operators (return 1 for true, 0 for false)
    printf("\n=== Comparison ===\n");
    printf("10 == 10: %d\n", 10 == 10);  // Equal
    printf("10 != 5: %d\n", 10 != 5);    // Not equal
    printf("10 > 5: %d\n", 10 > 5);      // Greater than
    printf("10 < 5: %d\n", 10 < 5);      // Less than
    printf("10 >= 10: %d\n", 10 >= 10);  // Greater or equal
    printf("5 <= 10: %d\n", 5 <= 10);    // Less or equal
    
    // Logical operators
    printf("\n=== Logical ===\n");
    int is_hot = 1;
    int is_humid = 1;
    int is_cold = 0;
    
    printf("Hot AND humid: %d\n", is_hot && is_humid);     // Both true?
    printf("Hot OR cold: %d\n", is_hot || is_cold);        // At least one true?
    printf("NOT cold: %d\n", !is_cold);                    // Flip the value
    
    return 0;
}

/*
 * Arithmetic: +, -, *, /, %
 * Comparison: ==, !=, >, <, >=, <=
 * Logical: && (AND), || (OR), ! (NOT)
 * Assignment: =, +=, -=, *=, /=, %=
 * Increment/Decrement: ++, --
 * 
 * Common mistake:
 *   if (x = 5)  // WRONG - assigns 5 to x
 *   if (x == 5) // RIGHT - checks if x equals 5
 * 
 * Try:
 *   - Combine multiple operations
 *   - Try (10 > 5) && (3 < 7)
 *   - What happens with !(1 && 0)?
 */
