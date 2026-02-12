# Linked Lists

## The Big Picture

Arrays store elements in contiguous memory. You allocate space upfront and that's it. Want to insert in the middle? Shift everything. Want to grow? Reallocate and copy.

Linked lists fix this. Each element (node) lives anywhere in memory and points to the next one. Insert in the middle? Change two pointers. Grow? Just malloc a new node.

The tradeoff: no random access. Want element 1000? Walk through all 999 nodes before it.

## What is a Node?

A node is a struct containing data and a pointer to the next node:

```c
typedef struct Node {
    int data;           // The actual value
    struct Node* next;  // Pointer to next node
} Node;
```

Visual representation:
```
[10|*] -> [20|*] -> [30|NULL]
 ^         ^         ^
data     data      data
next     next      next
```

The `NULL` at the end marks the list's end.

## Singly Linked List

Each node points forward only. You can traverse forward but not backward.

**Creating a node:**
```c
Node* create_node(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}
```

**Inserting at head:**
```c
void insert_at_head(Node** head, int data) {
    Node* new_node = create_node(data);
    new_node->next = *head;
    *head = new_node;
}
```

Why `Node**`? Because we're modifying the head pointer itself.

Before: `head -> [20] -> [30] -> NULL`
After:  `head -> [10] -> [20] -> [30] -> NULL`

**Inserting at tail:**
```c
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
```

Walk to the end, then attach new node.

**Deleting a node:**
```c
void delete_node(Node** head, int data) {
    if (*head == NULL) return;
    
    // Special case: deleting head
    if ((*head)->data == data) {
        Node* temp = *head;
        *head = (*head)->next;
        free(temp);
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
    }
}
```

Before: `[10] -> [20] -> [30] -> NULL`
Delete 20: `[10] -> [30] -> NULL`

## Doubly Linked List

Each node points both forward AND backward:

```c
typedef struct DNode {
    int data;
    struct DNode* prev;
    struct DNode* next;
} DNode;
```

Visual:
```
NULL <- [10] <-> [20] <-> [30] -> NULL
```

**Advantages:**
- Traverse backward
- Delete node without head pointer
- Easier insertions before a node

**Disadvantage:**
- More memory (extra pointer per node)
- More pointer updates on insert/delete

## Complexity

| Operation | Array | Linked List |
|-----------|-------|-------------|
| Access by index | O(1) | O(n) |
| Insert at head | O(n) | O(1) |
| Insert at tail | O(1)* | O(n) or O(1)** |
| Delete by value | O(n) | O(n) |
| Memory | Contiguous | Scattered |

\* With dynamic array, might need reallocation  
** O(1) if you keep tail pointer

## When to Use Linked Lists

**Use linked lists when:**
- Frequent insertions/deletions at head
- Size unknown or constantly changing
- Don't need random access
- Building other structures (stacks, queues, graphs)

**Use arrays when:**
- Need random access
- Iterate sequentially
- Size known or mostly stable
- Cache performance matters

## Common Mistakes

**Memory leaks:**
```c
// BAD: Lost reference to rest of list
head = create_node(10);
head = create_node(20);  // Lost node with 10!
```

**Use after free:**
```c
// BAD: Using freed memory
Node* temp = head;
free(temp);
printf("%d\n", temp->data);  // UNDEFINED BEHAVIOR
```

**Dangling pointers:**
```c
// BAD: Pointer points to freed memory
Node* node = head;
delete_node(&head, node->data);
printf("%d\n", node->data);  // node still points to freed memory
```

Always set freed pointers to NULL:
```c
free(node);
node = NULL;
```

## Freeing a List

Always free all nodes to avoid memory leaks:

```c
void free_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
}
```

## Advanced Operations

**Reversing a list:**
```c
Node* reverse(Node* head) {
    Node* prev = NULL;
    Node* current = head;
    Node* next = NULL;
    
    while (current != NULL) {
        next = current->next;  // Save next
        current->next = prev;  // Reverse pointer
        prev = current;        // Move prev forward
        current = next;        // Move current forward
    }
    
    return prev;  // New head
}
```

**Detecting cycles:**

Use Floyd's cycle detection (tortoise and hare):
```c
int has_cycle(Node* head) {
    Node* slow = head;
    Node* fast = head;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        
        if (slow == fast) return 1;  // Cycle found
    }
    
    return 0;
}
```

**Finding middle element:**
```c
Node* find_middle(Node* head) {
    Node* slow = head;
    Node* fast = head;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;
}
```

## Summary

Linked lists trade random access for flexible insertion/deletion. Each node lives independently in memory, connected by pointers. Master pointer manipulation and dynamic memory management - these skills apply to all data structures.

Key takeaway: **Arrays for access, linked lists for modification.**
