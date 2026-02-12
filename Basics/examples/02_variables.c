/*
 * 02_variables.c
 * 
 * Variables store data. You need to declare the type before using them.
 */

#include <stdio.h>

int main(void) {
    // Integer types
    int age = 25;
    long long population = 7900000000LL;  // Use long long for big numbers
    short temperature = -5;
    
    // Floating point
    float price = 19.99f;
    double pi = 3.14159265359;
    
    // Character (single letter)
    char grade = 'A';
    
    // Boolean (C uses integers: 0 = false, non-zero = true)
    int is_raining = 1;  // true
    
    // Print them
    printf("Age: %d\n", age);
    printf("Population: %lld\n", population);
    printf("Temperature: %d\n", temperature);
    printf("Price: $%.2f\n", price);
    printf("Pi: %f\n", pi);
    printf("Grade: %c\n", grade);
    printf("Is raining? %d\n", is_raining);
    
    // You can change values
    age = 26;
    printf("New age: %d\n", age);
    
    // Math with variables
    int x = 10;
    int y = 3;
    printf("x + y = %d\n", x + y);
    printf("x - y = %d\n", x - y);
    printf("x * y = %d\n", x * y);
    printf("x / y = %d\n", x / y);  // Integer division!
    printf("x %% y = %d\n", x % y);  // Remainder (modulo)
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Common types:
 *   int     - Whole numbers (-2147483648 to 2147483647)
 *   long    - Bigger whole numbers
 *   short   - Smaller whole numbers
 *   float   - Decimal numbers (6-7 digits precision)
 *   double  - Decimal numbers (15-16 digits precision)
 *   char    - Single character ('a', 'Z', '3', etc.)
 * 
 * Format specifiers for printf:
 *   %d or %i  - int
 *   %ld       - long
 *   %f        - float/double
 *   %c        - char
 *   %.2f      - float with 2 decimal places
 * 
 * Try:
 *   - Create more variables
 *   - Mix different types in calculations (what happens?)
 *   - Try dividing floats vs ints
 */
