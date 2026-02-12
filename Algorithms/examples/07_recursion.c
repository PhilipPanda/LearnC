/*
 * 07_recursion.c
 * 
 * Exploring recursion through classic examples.
 * Understand how functions can call themselves to solve problems.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Factorial: n! = n × (n-1) × (n-2) × ... × 1
int factorial(int n) {
    // Base case: 0! = 1, 1! = 1
    if (n <= 1) {
        return 1;
    }
    
    // Recursive case: n! = n × (n-1)!
    return n * factorial(n - 1);
}

// Fibonacci: fib(n) = fib(n-1) + fib(n-2)
int fibonacci(int n) {
    // Base cases
    if (n == 0) return 0;
    if (n == 1) return 1;
    
    // Recursive case
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Fibonacci with memoization (much faster!)
int fib_memo_helper(int n, int memo[]) {
    if (n <= 1) return n;
    
    // Check if already calculated
    if (memo[n] != -1) {
        return memo[n];
    }
    
    // Calculate and store
    memo[n] = fib_memo_helper(n - 1, memo) + fib_memo_helper(n - 2, memo);
    return memo[n];
}

int fibonacci_memoized(int n) {
    int *memo = (int*)malloc((n + 1) * sizeof(int));
    for (int i = 0; i <= n; i++) {
        memo[i] = -1;
    }
    int result = fib_memo_helper(n, memo);
    free(memo);
    return result;
}

// Sum of digits: sum_digits(1234) = 1 + 2 + 3 + 4 = 10
int sum_digits(int n) {
    if (n == 0) return 0;
    return (n % 10) + sum_digits(n / 10);
}

// Reverse string recursively
void reverse_string(char str[], int start, int end) {
    if (start >= end) return;
    
    // Swap ends
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    
    // Recurse on middle
    reverse_string(str, start + 1, end - 1);
}

// Check if string is palindrome
int is_palindrome(char str[], int start, int end) {
    if (start >= end) return 1;  // Base case: single char or empty
    
    if (str[start] != str[end]) return 0;
    
    return is_palindrome(str, start + 1, end - 1);
}

// Power function: x^n
int power(int x, int n) {
    if (n == 0) return 1;
    if (n == 1) return x;
    
    // Divide and conquer: x^8 = (x^4)²
    int half = power(x, n / 2);
    
    if (n % 2 == 0) {
        return half * half;
    } else {
        return x * half * half;
    }
}

// Count down
void countdown(int n) {
    if (n == 0) {
        printf("Blastoff!\n");
        return;
    }
    printf("%d...\n", n);
    countdown(n - 1);
}

// Greatest Common Divisor (Euclidean algorithm)
int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

int main(void) {
    printf("=== Recursion Examples ===\n\n");
    
    // Factorial
    printf("Example 1: Factorial\n");
    printf("--------------------\n");
    for (int i = 0; i <= 10; i++) {
        printf("  %d! = %d\n", i, factorial(i));
    }
    
    // Fibonacci (slow version)
    printf("\n\nExample 2: Fibonacci (naive recursive)\n");
    printf("---------------------------------------\n");
    printf("Computing fib(0) to fib(20)...\n");
    for (int i = 0; i <= 20; i++) {
        printf("  fib(%d) = %d\n", i, fibonacci(i));
    }
    printf("\nNote: This gets VERY slow for larger n!\n");
    printf("fib(40) would take several seconds...\n");
    
    // Fibonacci with memoization
    printf("\n\nExample 3: Fibonacci (with memoization)\n");
    printf("----------------------------------------\n");
    printf("Computing fib(40) with memoization...\n");
    int result = fibonacci_memoized(40);
    printf("  fib(40) = %d (computed instantly!)\n", result);
    
    // Sum of digits
    printf("\n\nExample 4: Sum of digits\n");
    printf("------------------------\n");
    int numbers[] = {1234, 9876, 42, 100500};
    for (int i = 0; i < 4; i++) {
        printf("  sum_digits(%d) = %d\n", numbers[i], sum_digits(numbers[i]));
    }
    
    // String reversal
    printf("\n\nExample 5: String reversal\n");
    printf("---------------------------\n");
    char str1[] = "hello";
    printf("  Original: %s\n", str1);
    reverse_string(str1, 0, strlen(str1) - 1);
    printf("  Reversed: %s\n", str1);
    
    char str2[] = "recursion";
    printf("  Original: %s\n", str2);
    reverse_string(str2, 0, strlen(str2) - 1);
    printf("  Reversed: %s\n", str2);
    
    // Palindrome check
    printf("\n\nExample 6: Palindrome check\n");
    printf("----------------------------\n");
    char *test[] = {"racecar", "hello", "madam", "recursion", "noon"};
    for (int i = 0; i < 5; i++) {
        int result = is_palindrome(test[i], 0, strlen(test[i]) - 1);
        printf("  '%s' is %s\n", test[i], result ? "a palindrome" : "not a palindrome");
    }
    
    // Power function
    printf("\n\nExample 7: Power function\n");
    printf("-------------------------\n");
    printf("  2^10 = %d\n", power(2, 10));
    printf("  3^5 = %d\n", power(3, 5));
    printf("  5^3 = %d\n", power(5, 3));
    
    // Countdown
    printf("\n\nExample 8: Countdown\n");
    printf("--------------------\n");
    countdown(5);
    
    // GCD
    printf("\n\nExample 9: Greatest Common Divisor\n");
    printf("-----------------------------------\n");
    printf("  gcd(48, 18) = %d\n", gcd(48, 18));
    printf("  gcd(100, 50) = %d\n", gcd(100, 50));
    printf("  gcd(17, 13) = %d\n", gcd(17, 13));
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Understanding Recursion:
 * 
 * Every recursive function needs:
 * 1. Base case - when to stop
 * 2. Recursive case - break problem into smaller problem
 * 3. Progress - eventually reach base case
 * 
 * Call stack visualization for factorial(5):
 * 
 * factorial(5)
 *   factorial(4)
 *     factorial(3)
 *       factorial(2)
 *         factorial(1) -> returns 1
 *       returns 2 * 1 = 2
 *     returns 3 * 2 = 6
 *   returns 4 * 6 = 24
 * returns 5 * 24 = 120
 * 
 * Each call waits for the recursive call to complete.
 * 
 * Fibonacci problem (naive):
 * - fib(n) calls fib(n-1) AND fib(n-2)
 * - Creates exponential number of calls
 * - Recalculates same values many times
 * - Solution: memoization (cache results)
 * 
 * When to use recursion:
 * - Problem naturally recursive (trees, divide-and-conquer)
 * - Makes code clearer than loops
 * - Depth is limited
 * 
 * When NOT to use:
 * - Simple iteration is clearer
 * - Deep recursion (stack overflow)
 * - Performance critical
 * - Overlapping subproblems without memoization
 * 
 * Try:
 * - Convert factorial to iterative
 * - Time fib(35) with and without memoization
 * - Implement tower of Hanoi
 * - Write recursive sum of array
 * - Implement recursive binary search
 */
