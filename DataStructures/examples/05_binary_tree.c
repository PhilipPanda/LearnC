/*
 * 05_binary_tree.c
 * 
 * Binary Search Tree (BST) implementation.
 * Demonstrates tree operations, traversals, and recursive algorithms.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Create new node
TreeNode* create_node(int data) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Insert into BST (recursive)
TreeNode* insert(TreeNode* root, int data) {
    if (root == NULL) {
        return create_node(data);
    }
    
    if (data < root->data) {
        root->left = insert(root->left, data);
    } else if (data > root->data) {
        root->right = insert(root->right, data);
    }
    // If equal, don't insert duplicate
    
    return root;
}

// Search in BST
TreeNode* search(TreeNode* root, int target) {
    if (root == NULL || root->data == target) {
        return root;
    }
    
    if (target < root->data) {
        return search(root->left, target);
    } else {
        return search(root->right, target);
    }
}

// Find minimum node
TreeNode* find_min(TreeNode* root) {
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

// Delete node from BST
TreeNode* delete_node(TreeNode* root, int data) {
    if (root == NULL) return NULL;
    
    if (data < root->data) {
        root->left = delete_node(root->left, data);
    } else if (data > root->data) {
        root->right = delete_node(root->right, data);
    } else {
        // Found node to delete
        
        // Case 1: Leaf or Case 2: One child
        if (root->left == NULL) {
            TreeNode* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            TreeNode* temp = root->left;
            free(root);
            return temp;
        }
        
        // Case 3: Two children
        // Get inorder successor (smallest in right subtree)
        TreeNode* temp = find_min(root->right);
        root->data = temp->data;
        root->right = delete_node(root->right, temp->data);
    }
    
    return root;
}

// Inorder traversal (Left, Root, Right) - gives sorted order!
void inorder(TreeNode* root) {
    if (root == NULL) return;
    
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

// Preorder traversal (Root, Left, Right)
void preorder(TreeNode* root) {
    if (root == NULL) return;
    
    printf("%d ", root->data);
    preorder(root->left);
    preorder(root->right);
}

// Postorder traversal (Left, Right, Root)
void postorder(TreeNode* root) {
    if (root == NULL) return;
    
    postorder(root->left);
    postorder(root->right);
    printf("%d ", root->data);
}

// Tree height
int height(TreeNode* root) {
    if (root == NULL) return -1;
    
    int left_height = height(root->left);
    int right_height = height(root->right);
    
    return 1 + (left_height > right_height ? left_height : right_height);
}

// Count nodes
int size(TreeNode* root) {
    if (root == NULL) return 0;
    return 1 + size(root->left) + size(root->right);
}

// Print tree (simple visualization)
void print_tree(TreeNode* root, int space) {
    if (root == NULL) return;
    
    space += 5;
    
    print_tree(root->right, space);
    
    printf("\n");
    for (int i = 5; i < space; i++) {
        printf(" ");
    }
    printf("%d\n", root->data);
    
    print_tree(root->left, space);
}

// Free tree
void free_tree(TreeNode* root) {
    if (root == NULL) return;
    
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main(void) {
    printf("=== Binary Search Tree ===\n\n");
    
    TreeNode* root = NULL;
    
    // Example 1: Building a BST
    printf("Example 1: Building a BST\n");
    printf("-------------------------\n");
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    
    printf("Inserting: ");
    for (int i = 0; i < 7; i++) {
        printf("%d ", values[i]);
        root = insert(root, values[i]);
    }
    printf("\n\n");
    
    printf("Tree structure:\n");
    print_tree(root, 0);
    
    // Example 2: Traversals
    printf("\n\nExample 2: Tree traversals\n");
    printf("--------------------------\n");
    printf("Inorder (sorted):   ");
    inorder(root);
    printf("\nPreorder (copy):    ");
    preorder(root);
    printf("\nPostorder (delete): ");
    postorder(root);
    printf("\n");
    
    // Example 3: Searching
    printf("\n\nExample 3: Searching\n");
    printf("--------------------\n");
    int search_values[] = {40, 25, 70};
    for (int i = 0; i < 3; i++) {
        TreeNode* result = search(root, search_values[i]);
        if (result != NULL) {
            printf("  Found %d in tree\n", search_values[i]);
        } else {
            printf("  %d not found\n", search_values[i]);
        }
    }
    
    // Example 4: Tree properties
    printf("\n\nExample 4: Tree properties\n");
    printf("--------------------------\n");
    printf("Height: %d\n", height(root));
    printf("Size (nodes): %d\n", size(root));
    printf("Root: %d\n", root->data);
    printf("Min: %d\n", find_min(root)->data);
    
    // Example 5: Deletion
    printf("\n\nExample 5: Deletion\n");
    printf("-------------------\n");
    printf("Current tree:\n");
    print_tree(root, 0);
    
    printf("\n\nDeleting 20 (leaf node):\n");
    root = delete_node(root, 20);
    print_tree(root, 0);
    
    printf("\n\nDeleting 30 (node with two children):\n");
    root = delete_node(root, 30);
    print_tree(root, 0);
    
    printf("\n\nInorder after deletions: ");
    inorder(root);
    printf("\n");
    
    // Example 6: Building skewed tree (worst case)
    printf("\n\nExample 6: Skewed tree (worst case)\n");
    printf("------------------------------------\n");
    TreeNode* skewed = NULL;
    
    printf("Inserting in sorted order: 10, 20, 30, 40, 50\n");
    skewed = insert(skewed, 10);
    skewed = insert(skewed, 20);
    skewed = insert(skewed, 30);
    skewed = insert(skewed, 40);
    skewed = insert(skewed, 50);
    
    printf("\nSkewed tree structure (like linked list!):\n");
    print_tree(skewed, 0);
    printf("\nHeight: %d (O(n) - BAD!)\n", height(skewed));
    printf("Compare to balanced height: O(log n) = ~2-3\n");
    
    // Cleanup
    printf("\n\nFreeing trees...\n");
    free_tree(root);
    free_tree(skewed);
    printf("Done!\n");
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Binary Search Tree Properties:
 * 
 * BST Property:
 * - Left subtree < Root < Right subtree
 * - Holds for every node in tree
 * - Enables binary search!
 * 
 * Example:
 *         50
 *        /  \
 *       30   70
 *      / \   / \
 *     20 40 60 80
 * 
 * All nodes in left subtree < 50 < all in right subtree
 * 
 * Operations Complexity:
 * 
 * Balanced tree (height = log n):
 * - Search: O(log n)
 * - Insert: O(log n)
 * - Delete: O(log n)
 * 
 * Skewed tree (height = n):
 * - Search: O(n)
 * - Insert: O(n)
 * - Delete: O(n)
 * 
 * Traversals:
 * 
 * 1. Inorder (Left, Root, Right):
 *    - Gives sorted order for BST
 *    - Use: Print sorted, validate BST
 * 
 * 2. Preorder (Root, Left, Right):
 *    - Root first
 *    - Use: Copy tree, serialize
 * 
 * 3. Postorder (Left, Right, Root):
 *    - Root last
 *    - Use: Delete tree, expression evaluation
 * 
 * Why Trees?
 * 
 * Arrays:
 * - Fast access O(1)
 * - Slow insert/delete O(n)
 * - Fixed size
 * 
 * Linked Lists:
 * - Fast insert at head O(1)
 * - Slow search O(n)
 * - No random access
 * 
 * BST:
 * - Fast search O(log n)
 * - Fast insert O(log n)
 * - Fast delete O(log n)
 * - Ordered iteration
 * 
 * Real Applications:
 * - Databases (B-trees)
 * - File systems
 * - Compilers (syntax trees)
 * - Autocomplete
 * - Range queries
 * 
 * Balancing:
 * 
 * Problem: Sorted insertion creates skewed tree (O(n) operations)
 * Solution: Self-balancing trees
 * 
 * - AVL tree: Strict balancing, height diff ≤ 1
 * - Red-Black tree: Looser, faster inserts
 * - B-tree: Multiple children, for databases
 * 
 * These maintain O(log n) height automatically!
 * 
 * Try:
 * - Implement iterative search
 * - Add level-order traversal (use queue)
 * - Check if tree is BST
 * - Find kth smallest element
 * - Implement AVL rotations
 */
