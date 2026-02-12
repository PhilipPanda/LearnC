# C Data Structures

Build data structures from scratch. No libraries, no abstractions - see exactly how each structure works internally.

## What you get

- Linked lists (singly and doubly linked)
- Stacks and queues (array and linked implementations)
- Binary search trees with traversals
- Hash tables with collision handling
- Graphs with BFS/DFS
- Heaps for priority queues
- Visual output showing structure operations
- Memory management demonstrations

Everything implemented with malloc/free. See how dynamic data structures actually work.

## Building

Each example is standalone:

```bash
# Windows
gcc 01_linked_list.c -o 01_linked_list.exe
.\01_linked_list.exe

# Linux
gcc 01_linked_list.c -o 01_linked_list
./01_linked_list
```

Or build all at once:
```bash
cd examples
./build_all.bat    # Windows
./build_all.sh     # Linux
```

## Using it

Here's a linked list in action:

```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node* create_node(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

void print_list(Node* head) {
    while (head != NULL) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}

int main(void) {
    Node* head = create_node(10);
    head->next = create_node(20);
    head->next->next = create_node(30);
    
    print_list(head);  // 10 -> 20 -> 30 -> NULL
    return 0;
}
```

## Documentation

- **[Linked Lists](docs/LINKED_LISTS.md)** - Singly and doubly linked implementations
- **[Stacks and Queues](docs/STACKS_QUEUES.md)** - LIFO and FIFO structures
- **[Trees](docs/TREES.md)** - Binary search trees, traversals
- **[Hash Tables](docs/HASH_TABLES.md)** - Hashing, collision resolution
- **[Graphs](docs/GRAPHS.md)** - Adjacency lists, BFS, DFS

## Examples

Each example includes visualization and practical operations:

| Example | What It Teaches |
|---------|----------------|
| 01_linked_list | Dynamic memory, pointer manipulation, insertions |
| 02_doubly_linked_list | Two-way traversal, more complex pointers |
| 03_stack | LIFO operations, two implementations |
| 04_queue | FIFO operations, circular buffers |
| 05_binary_tree | Tree structure, recursive operations |
| 06_hash_table | Hashing functions, collision handling |
| 07_graph | Graph representation, traversal algorithms |
| 08_heap | Priority queues, heap operations |

Go in order. Each one builds on previous concepts.

## What this teaches

- Dynamic memory allocation and deallocation
- Pointer manipulation and complex structures
- When to use each data structure
- Time and space complexity tradeoffs
- Memory leaks and how to avoid them
- Choosing the right structure for your problem

After working through this, you'll understand how the data structures you use in higher-level languages actually work under the hood.
