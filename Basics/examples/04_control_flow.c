/*
 * 04_control_flow.c
 * 
 * Control the order your code executes.
 */

#include <stdio.h>

int main(void) {
    // If statement
    printf("=== If/Else ===\n");
    int age = 18;
    
    if (age >= 18) {
        printf("You can vote\n");
    } else {
        printf("Too young to vote\n");
    }
    
    // Multiple conditions
    int score = 85;
    
    if (score >= 90) {
        printf("Grade: A\n");
    } else if (score >= 80) {
        printf("Grade: B\n");
    } else if (score >= 70) {
        printf("Grade: C\n");
    } else {
        printf("Grade: F\n");
    }
    
    // While loop - runs while condition is true
    printf("\n=== While Loop ===\n");
    int count = 0;
    while (count < 5) {
        printf("Count: %d\n", count);
        count++;
    }
    
    // For loop - compact loop syntax
    printf("\n=== For Loop ===\n");
    for (int i = 0; i < 5; i++) {
        printf("i = %d\n", i);
    }
    
    // For loop breakdown:
    // for (initialization; condition; increment)
    //      int i = 0       - runs once at start
    //      i < 5           - checked before each iteration
    //      i++             - runs after each iteration
    
    // Do-while - always runs at least once
    printf("\n=== Do-While ===\n");
    int x = 10;
    do {
        printf("x = %d\n", x);
        x++;
    } while (x < 5);  // Condition is false, but body ran once
    
    // Break - exit loop early
    printf("\n=== Break ===\n");
    for (int i = 0; i < 10; i++) {
        if (i == 3) break;  // Stop when i is 3
        printf("%d ", i);
    }
    printf("\n");
    
    // Continue - skip rest of iteration
    printf("\n=== Continue ===\n");
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;  // Skip when i is 2
        printf("%d ", i);
    }
    printf("\n");
    
    // Switch statement - multiple cases
    printf("\n=== Switch ===\n");
    int day = 3;
    
    switch (day) {
        case 1:
            printf("Monday\n");
            break;
        case 2:
            printf("Tuesday\n");
            break;
        case 3:
            printf("Wednesday\n");
            break;
        default:
            printf("Other day\n");
            break;
    }
    
    return 0;
}

/*
 * Control flow:
 *   if/else if/else - conditional execution
 *   while           - loop while condition is true
 *   for             - compact loop
 *   do-while        - loop at least once
 *   switch          - multiple cases
 *   break           - exit loop/switch
 *   continue        - skip to next iteration
 * 
 * Common patterns:
 *   for (int i = 0; i < n; i++)  - count from 0 to n-1
 *   while (condition)            - loop until condition is false
 *   if (x) vs if (x == 1)        - any non-zero is "true"
 * 
 * Try:
 *   - Nested loops (loop inside loop)
 *   - Change loop conditions
 *   - What happens without break in switch?
 */
