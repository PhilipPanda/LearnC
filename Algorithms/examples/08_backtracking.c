/*
 * 08_backtracking.c
 * 
 * N-Queens problem using backtracking.
 * Place N queens on N×N chessboard so none attack each other.
 * Classic example of exhaustive search with pruning.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 20

// Print the chessboard
void print_board(int board[][MAX_N], int n) {
    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (board[i][j]) {
                printf(" Q ");
            } else {
                printf(" . ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Check if queen can be placed at board[row][col]
int is_safe(int board[][MAX_N], int n, int row, int col) {
    // Check this column (rows above)
    for (int i = 0; i < row; i++) {
        if (board[i][col]) return 0;
    }
    
    // Check upper left diagonal
    for (int i = row, j = col; i >= 0 && j >= 0; i--, j--) {
        if (board[i][j]) return 0;
    }
    
    // Check upper right diagonal
    for (int i = row, j = col; i >= 0 && j < n; i--, j++) {
        if (board[i][j]) return 0;
    }
    
    return 1;
}

// Solve N-Queens using backtracking
int solve_n_queens(int board[][MAX_N], int n, int row, int show_steps, int *solutions_found) {
    // Base case: all queens placed
    if (row >= n) {
        (*solutions_found)++;
        printf("Solution #%d:", *solutions_found);
        print_board(board, n);
        return 1;  // Found a solution
    }
    
    // Try placing queen in each column
    for (int col = 0; col < n; col++) {
        if (is_safe(board, n, row, col)) {
            // Place queen
            board[row][col] = 1;
            
            if (show_steps) {
                printf("Trying queen at row %d, col %d\n", row, col);
            }
            
            // Recur to place rest
            if (solve_n_queens(board, n, row + 1, show_steps, solutions_found)) {
                // Continue to find all solutions (remove this if to stop at first)
            }
            
            // Backtrack: remove queen and try next position
            board[row][col] = 0;
        }
    }
    
    return 0;
}

// Find all solutions for N-Queens
void find_all_solutions(int n, int show_steps) {
    int board[MAX_N][MAX_N] = {0};
    int solutions_found = 0;
    
    printf("Solving %d-Queens problem...\n", n);
    if (show_steps) {
        printf("(Showing search process)\n");
    }
    
    solve_n_queens(board, n, 0, show_steps, &solutions_found);
    
    if (solutions_found == 0) {
        printf("No solutions found.\n");
    } else {
        printf("Total solutions found: %d\n", solutions_found);
    }
}

int main(void) {
    printf("=== N-Queens Problem (Backtracking) ===\n\n");
    
    // Example 1: 4-Queens with visualization
    printf("Example 1: 4-Queens problem\n");
    printf("---------------------------\n");
    find_all_solutions(4, 0);
    
    // Example 2: 8-Queens (classic chess problem)
    printf("\n\nExample 2: 8-Queens problem (classic)\n");
    printf("--------------------------------------\n");
    find_all_solutions(8, 0);
    
    // Example 3: Smaller examples
    printf("\n\nExample 3: Trying different board sizes\n");
    printf("----------------------------------------\n");
    
    printf("\n5-Queens:\n");
    find_all_solutions(5, 0);
    
    printf("\n6-Queens:\n");
    find_all_solutions(6, 0);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * What is Backtracking?
 * 
 * Backtracking is like exploring a maze:
 * 1. Try a path
 * 2. If it leads to solution: great!
 * 3. If it hits dead end: go back and try different path
 * 4. Continue until all paths explored
 * 
 * N-Queens Problem:
 * - Place N queens on N×N chessboard
 * - No two queens can attack each other
 * - Queens attack: same row, column, or diagonal
 * 
 * Algorithm:
 * 1. Place queen in first row
 * 2. Try to place queen in next row
 * 3. Check if safe (no conflicts)
 * 4. If safe: continue to next row
 * 5. If not safe: try next column
 * 6. If no columns work: backtrack to previous row
 * 7. Continue until all queens placed or all options tried
 * 
 * Example: 4-Queens
 * 
 * Try (0,0): Q . . .
 *            . . Q .
 *            X X X X  <- No safe spot! Backtrack.
 * 
 * Try (0,1): . Q . .
 *            . . . Q
 *            Q . . .  <- Can't place row 2! Backtrack.
 * 
 * Try (0,2): . . Q .
 *            Q . . .
 *            . . . Q
 *            . Q . .  <- Success!
 * 
 * Complexity:
 * - Time: O(n!) - tries many permutations
 * - Space: O(n) - recursion depth
 * 
 * Number of solutions:
 * - 4-Queens: 2 solutions
 * - 8-Queens: 92 solutions
 * - 10-Queens: 724 solutions
 * 
 * Backtracking pattern:
 * 
 * function solve(state):
 *     if is_solution(state):
 *         record solution
 *         return
 *     
 *     for each choice in choices(state):
 *         if is_valid(choice):
 *             make_choice(choice)
 *             solve(new_state)
 *             undo_choice(choice)  // Backtrack!
 * 
 * Other backtracking problems:
 * - Sudoku solver
 * - Maze solving
 * - Subset sum
 * - Graph coloring
 * - Traveling salesman (with pruning)
 * 
 * Try:
 * - Modify to stop at first solution
 * - Add counter for states explored
 * - Solve Sudoku using same pattern
 * - Implement maze solver
 */
