/*
 * 02_doubly_linked_list.c
 * 
 * Doubly linked list - each node has prev and next pointers.
 * Enables bidirectional traversal and easier deletions.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct DNode {
    int data;
    struct DNode* prev;
    struct DNode* next;
} DNode;

// Create new node
DNode* create_node(int data) {
    DNode* new_node = (DNode*)malloc(sizeof(DNode));
    if (new_node == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    new_node->data = data;
    new_node->prev = NULL;
    new_node->next = NULL;
    return new_node;
}

// Print forward
void print_forward(DNode* head) {
    if (head == NULL) {
        printf("NULL\n");
        return;
    }
    
    printf("NULL <- ");
    DNode* current = head;
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" <-> ");
        }
        current = current->next;
    }
    printf(" -> NULL\n");
}

// Print backward
void print_backward(DNode* tail) {
    if (tail == NULL) {
        printf("NULL\n");
        return;
    }
    
    printf("NULL <- ");
    DNode* current = tail;
    while (current != NULL) {
        printf("%d", current->data);
        if (current->prev != NULL) {
            printf(" <-> ");
        }
        current = current->prev;
    }
    printf(" -> NULL\n");
}

// Get tail
DNode* get_tail(DNode* head) {
    if (head == NULL) return NULL;
    
    DNode* current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

// Insert at head
void insert_at_head(DNode** head, int data) {
    DNode* new_node = create_node(data);
    
    if (*head != NULL) {
        (*head)->prev = new_node;
    }
    new_node->next = *head;
    *head = new_node;
}

// Insert at tail
void insert_at_tail(DNode** head, int data) {
    DNode* new_node = create_node(data);
    
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    
    DNode* tail = get_tail(*head);
    tail->next = new_node;
    new_node->prev = tail;
}

// Insert after node with value
void insert_after(DNode* head, int after_value, int data) {
    DNode* current = head;
    
    // Find node with after_value
    while (current != NULL && current->data != after_value) {
        current = current->next;
    }
    
    if (current == NULL) {
        printf("  Value %d not found\n", after_value);
        return;
    }
    
    DNode* new_node = create_node(data);
    new_node->next = current->next;
    new_node->prev = current;
    
    if (current->next != NULL) {
        current->next->prev = new_node;
    }
    current->next = new_node;
    
    printf("  Inserted %d after %d\n", data, after_value);
}

// Delete node (no need to traverse from head!)
void delete_node(DNode** head, DNode* node) {
    if (node == NULL) return;
    
    // Update prev node's next
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        // Deleting head
        *head = node->next;
    }
    
    // Update next node's prev
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    
    free(node);
}

// Delete by value
void delete_value(DNode** head, int data) {
    DNode* current = *head;
    
    while (current != NULL && current->data != data) {
        current = current->next;
    }
    
    if (current != NULL) {
        delete_node(head, current);
        printf("  Deleted %d\n", data);
    } else {
        printf("  Value %d not found\n", data);
    }
}

// Search
DNode* search(DNode* head, int data) {
    DNode* current = head;
    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Free list
void free_list(DNode* head) {
    DNode* current = head;
    while (current != NULL) {
        DNode* temp = current;
        current = current->next;
        free(temp);
    }
}

int main(void) {
    printf("=== Doubly Linked List ===\n\n");
    
    DNode* list = NULL;
    
    // Example 1: Building the list
    printf("Example 1: Building a doubly linked list\n");
    printf("----------------------------------------\n");
    insert_at_tail(&list, 10);
    insert_at_tail(&list, 20);
    insert_at_tail(&list, 30);
    printf("Forward:  ");
    print_forward(list);
    printf("Backward: ");
    print_backward(get_tail(list));
    
    // Example 2: Insert at head
    printf("\n\nExample 2: Inserting at head\n");
    printf("----------------------------\n");
    insert_at_head(&list, 5);
    printf("After inserting 5 at head:\n");
    printf("Forward:  ");
    print_forward(list);
    
    // Example 3: Insert after specific node
    printf("\n\nExample 3: Inserting after node\n");
    printf("-------------------------------\n");
    printf("Current: ");
    print_forward(list);
    insert_after(list, 20, 25);
    printf("After:   ");
    print_forward(list);
    
    // Example 4: Bidirectional traversal
    printf("\n\nExample 4: Bidirectional traversal\n");
    printf("-----------------------------------\n");
    printf("Forward from head: ");
    print_forward(list);
    printf("Backward from tail: ");
    print_backward(get_tail(list));
    
    // Example 5: Deletion advantages
    printf("\n\nExample 5: Easy deletion with node pointer\n");
    printf("------------------------------------------\n");
    printf("Current: ");
    print_forward(list);
    
    // Find and delete directly
    DNode* node_to_delete = search(list, 25);
    if (node_to_delete != NULL) {
        printf("Found node with value 25, deleting it...\n");
        delete_node(&list, node_to_delete);
    }
    
    printf("After:   ");
    print_forward(list);
    
    // Example 6: Delete by value
    printf("\n\nExample 6: Delete by value\n");
    printf("--------------------------\n");
    printf("Current: ");
    print_forward(list);
    delete_value(&list, 20);
    printf("After:   ");
    print_forward(list);
    
    // Cleanup
    printf("\n\nFreeing list...\n");
    free_list(list);
    printf("Done!\n");
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Doubly Linked List Advantages:
 * 
 * 1. Bidirectional Traversal:
 *    - Can traverse forward and backward
 *    - Useful for undo/redo, browser history
 * 
 * 2. Easier Deletion:
 *    - Can delete node given only pointer to it
 *    - Don't need to traverse from head
 *    - In singly linked list, need previous node
 * 
 * 3. Easier Insertion:
 *    - Can insert before a node easily
 *    - Singly linked list only allows insert after
 * 
 * Disadvantages:
 * 
 * 1. More Memory:
 *    - Extra pointer per node
 *    - For small data, pointer overhead is significant
 * 
 * 2. More Pointer Updates:
 *    - Must update 4 pointers on insert (vs 2)
 *    - More complex code
 *    - Slightly slower operations
 * 
 * When to Use:
 * - Need backward traversal
 * - Have pointers to nodes and need to delete them
 * - Implementing undo/redo
 * - LRU cache
 * - Browser history
 * - Music playlist (next/previous)
 * 
 * Complexity (same as singly linked):
 * - Insert at head: O(1)
 * - Insert at tail: O(1) with tail pointer, O(n) without
 * - Delete with pointer: O(1) - BIG WIN!
 * - Delete by value: O(n)
 * - Search: O(n)
 */
