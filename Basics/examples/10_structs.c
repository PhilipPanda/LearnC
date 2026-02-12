/*
 * 10_structs.c
 * 
 * Structs let you group related data together.
 */

#include <stdio.h>
#include <string.h>

// Define a struct type
struct Person {
    char name[50];
    int age;
    float height;
};

// Typedef makes it cleaner
typedef struct {
    float x, y;
} Point;

typedef struct {
    Point position;
    Point velocity;
    float radius;
} Circle;

int main(void) {
    // Create and initialize struct
    printf("=== Basic Struct ===\n");
    struct Person alice;
    strcpy(alice.name, "Alice");
    alice.age = 28;
    alice.height = 5.6f;
    
    printf("Name: %s\n", alice.name);
    printf("Age: %d\n", alice.age);
    printf("Height: %.1f\n", alice.height);
    
    // Initialize at declaration
    struct Person bob = {"Bob", 35, 6.1f};
    printf("\n%s is %d years old\n", bob.name, bob.age);
    
    // Using typedef - cleaner syntax
    printf("\n=== With Typedef ===\n");
    Point p1 = {10.0f, 20.0f};
    Point p2 = {5.0f, 15.0f};
    
    printf("p1: (%.1f, %.1f)\n", p1.x, p1.y);
    printf("p2: (%.1f, %.1f)\n", p2.x, p2.y);
    
    // Nested structs
    printf("\n=== Nested Structs ===\n");
    Circle ball = {
        .position = {100.0f, 200.0f},
        .velocity = {5.0f, -3.0f},
        .radius = 15.0f
    };
    
    printf("Ball at (%.1f, %.1f), radius %.1f\n", 
           ball.position.x, ball.position.y, ball.radius);
    
    // Move the ball
    ball.position.x += ball.velocity.x;
    ball.position.y += ball.velocity.y;
    printf("After move: (%.1f, %.1f)\n", ball.position.x, ball.position.y);
    
    // Pointers to structs
    printf("\n=== Struct Pointers ===\n");
    Point *ptr = &p1;
    printf("Via pointer: (%.1f, %.1f)\n", ptr->x, ptr->y);
    // ptr->x is shorthand for (*ptr).x
    
    return 0;
}

/*
 * Struct syntax:
 * 
 * Definition:
 *   struct TypeName {
 *       type field1;
 *       type field2;
 *   };
 * 
 * Usage:
 *   struct TypeName var;
 *   var.field1 = value;
 * 
 * With typedef:
 *   typedef struct { ... } TypeName;
 *   TypeName var;  // No need for 'struct' keyword
 * 
 * Accessing members:
 *   var.field         - Direct access
 *   ptr->field        - Through pointer
 *   (*ptr).field      - Same as above
 * 
 * Initialization:
 *   Point p = {1.0f, 2.0f};            - Positional
 *   Point p = {.x = 1.0f, .y = 2.0f};  - Named (C99)
 * 
 * Try:
 *   - Create a Rectangle struct with width/height
 *   - Make an array of structs
 *   - Write a function that takes struct as parameter
 *   - Pass struct by pointer vs by value (what's the difference?)
 */
