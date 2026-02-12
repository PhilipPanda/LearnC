/*
 * 01_linked_list.c
 * 
 * Singly linked list implementation.
 * Dynamic memory, pointer manipulation, basic operations.
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Create a new node
Node* create_node(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

// Print list
void print_list(Node* head) {
    if (head == NULL) {
        printf("(empty)\n");
        return;
    }
    
    Node* current = head;
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf(" -> NULL\n");
}

// Insert at head (O(1))
void insert_at_head(Node** head, int data) {
    Node* new_node = create_node(data);
    new_node->next = *head;
    *head = new_node;
}

// Insert at tail (O(n))
void insert_at_tail(Node** head, int data) {
    Node* new_node = create_node(data);
    
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    
    Node* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

// Insert at position
void insert_at_position(Node** head, int data, int position) {
    if (position == 0) {
        insert_at_head(head, data);
        return;
    }
    
    Node* new_node = create_node(data);
    Node* current = *head;
    
    for (int i = 0; i < position - 1 && current != NULL; i++) {
        current = current->next;
    }
    
    if (current == NULL) {
        printf("Position out of bounds!\n");
        free(new_node);
        return;
    }
    
    new_node->next = current->next;
    current->next = new_node;
}

// Delete node with value
void delete_value(Node** head, int data) {
    if (*head == NULL) return;
    
    // Special case: head node
    if ((*head)->data == data) {
        Node* temp = *head;
        *head = (*head)->next;
        free(temp);
        printf("  Deleted %d from head\n", data);
        return;
    }
    
    // Find node before the one to delete
    Node* current = *head;
    while (current->next != NULL && current->next->data != data) {
        current = current->next;
    }
    
    if (current->next != NULL) {
        Node* temp = current->next;
        current->next = current->next->next;
        free(temp);
        printf("  Deleted %d\n", data);
    } else {
        printf("  Value %d not found\n", data);
    }
}

// Search for value
int search(Node* head, int data) {
    Node* current = head;
    int position = 0;
    
    while (current != NULL) {
        if (current->data == data) {
            return position;
        }
        current = current->next;
        position++;
    }
    
    return -1;  // Not found
}

// Get length
int length(Node* head) {
    int count = 0;
    Node* current = head;
    
    while (current != NULL) {
        count++;
        current = current->next;
    }
    
    return count;
}

// Reverse the list
Node* reverse(Node* head) {
    Node* prev = NULL;
    Node* current = head;
    Node* next = NULL;
    
    while (current != NULL) {
        next = current->next;  // Save next
        current->next = prev;  // Reverse link
        prev = current;        // Move prev forward
        current = next;        // Move current forward
    }
    
    return prev;  // New head
}

// Free entire list
void free_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
}

int main(void) {
    printf("=== Singly Linked List ===\n\n");
    
    Node* list = NULL;
    
    // Example 1: Building a list
    printf("Example 1: Building a list\n");
    printf("--------------------------\n");
    insert_at_tail(&list, 10);
    insert_at_tail(&list, 20);
    insert_at_tail(&list, 30);
    printf("After inserting at tail: ");
    print_list(list);
    
    insert_at_head(&list, 5);
    printf("After inserting 5 at head: ");
    print_list(list);
    
    insert_at_position(&list, 15, 2);
    printf("After inserting 15 at position 2: ");
    print_list(list);
    
    // Example 2: Searching
    printf("\n\nExample 2: Searching\n");
    printf("--------------------\n");
    printf("Current list: ");
    print_list(list);
    
    int search_values[] = {15, 30, 100};
    for (int i = 0; i < 3; i++) {
        int pos = search(list, search_values[i]);
        if (pos != -1) {
            printf("  Found %d at position %d\n", search_values[i], pos);
        } else {
            printf("  %d not found\n", search_values[i]);
        }
    }
    
    // Example 3: Deletion
    printf("\n\nExample 3: Deletion\n");
    printf("-------------------\n");
    printf("Current list: ");
    print_list(list);
    printf("Length: %d\n\n", length(list));
    
    delete_value(&list, 15);
    printf("After deletion: ");
    print_list(list);
    printf("Length: %d\n", length(list));
    
    // Example 4: Reversing
    printf("\n\nExample 4: Reversing\n");
    printf("--------------------\n");
    printf("Original: ");
    print_list(list);
    
    list = reverse(list);
    printf("Reversed: ");
    print_list(list);
    
    list = reverse(list);
    printf("Reversed again: ");
    print_list(list);
    
    // Example 5: Memory management
    printf("\n\nExample 5: Cleaning up\n");
    printf("----------------------\n");
    printf("Freeing list with %d nodes...\n", length(list));
    free_list(list);
    printf("Memory freed successfully!\n");
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Key Concepts:
 * 
 * 1. Dynamic Memory:
 *    - malloc() allocates memory at runtime
 *    - free() releases memory back
 *    - Always free what you malloc!
 * 
 * 2. Pointer to Pointer (Node**):
 *    - Used when function needs to modify head pointer
 *    - insert_at_head changes what head points to
 *    - Must pass &head to allow modification
 * 
 * 3. Traversal Pattern:
 *    while (current != NULL) {
 *        // Do something with current
 *        current = current->next;
 *    }
 * 
 * 4. Edge Cases:
 *    - Empty list (head == NULL)
 *    - Single element
 *    - Modifying head
 *    - Position out of bounds
 * 
 * 5. Complexity:
 *    - Insert at head: O(1)
 *    - Insert at tail: O(n) - must traverse
 *    - Delete: O(n) - must search
 *    - Search: O(n)
 *    - Access by index: O(n)
 * 
 * Why use linked list?
 * - Dynamic size
 * - Fast insertion at head
 * - No reallocation like arrays
 * 
 * When to use array instead?
 * - Need random access
 * - Size known in advance
 * - Cache performance matters
 */
