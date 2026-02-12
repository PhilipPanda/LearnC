# Hash Tables

## The Big Picture

Arrays give O(1) access by index. But what if you want O(1) access by key? What if you want to map "alice" to 42 instantly?

Hash tables solve this. They use a **hash function** to convert keys into array indices. Insert "alice" with value 42? Hash "alice" to index 3, store 42 there. Look up "alice"? Hash to 3, retrieve value.

Perfect for dictionaries, caches, counting, deduplication - anywhere you need fast key-value lookup.

## How It Works

```
Key -> Hash Function -> Index -> Value

"alice" -> hash("alice") = 5 -> array[5] = 42
"bob"   -> hash("bob")   = 2 -> array[2] = 17
```

The hash function deterministically maps keys to indices:
- Same key always produces same index
- Different keys should produce different indices (ideally)
- Output should be uniformly distributed

## Hash Functions

Simple example for strings:

```c
unsigned int hash(const char* key, int table_size) {
    unsigned int hash = 0;
    
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];  // 31 is a good prime
    }
    
    return hash % table_size;
}
```

Why 31? It's prime, gives good distribution, and `31 * x = (x << 5) - x` (fast).

Better hash functions:
- **DJB2:** `hash = hash * 33 + c`
- **FNV-1a:** XOR-based, very fast
- **MurmurHash:** Industry standard, excellent distribution

## Collisions

**Problem:** Two keys hash to same index.

```
hash("alice") = 3
hash("eve")   = 3  // Collision!
```

No hash function is perfect. Collisions are inevitable (pigeonhole principle). We need collision resolution strategies.

## Collision Resolution: Chaining

Each array slot holds a linked list:

```c
typedef struct HashNode {
    char* key;
    int value;
    struct HashNode* next;
} HashNode;

typedef struct HashTable {
    HashNode** buckets;
    int size;
    int count;
} HashTable;
```

Visual:
```
[0] -> NULL
[1] -> ("bob", 17) -> NULL
[2] -> ("charlie", 99) -> NULL
[3] -> ("alice", 42) -> ("eve", 7) -> NULL  // Collision handled
[4] -> NULL
```

**Insert:**
```c
void insert(HashTable* table, const char* key, int value) {
    int index = hash(key, table->size);
    
    // Check if key exists
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;  // Update
            return;
        }
        current = current->next;
    }
    
    // Insert new node at head
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->count++;
}
```

**Search:**
```c
int search(HashTable* table, const char* key, int* value) {
    int index = hash(key, table->size);
    
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            *value = current->value;
            return 1;  // Found
        }
        current = current->next;
    }
    
    return 0;  // Not found
}
```

**Pros:**
- Simple to implement
- Never "fills up" (can always add more to chains)
- Performance degrades gradually

**Cons:**
- Extra memory for pointers
- Cache-unfriendly (linked list traversal)
- Worst case O(n) if all hash to same bucket

## Collision Resolution: Open Addressing

Store everything in the array itself. On collision, probe for next empty slot.

### Linear Probing

If slot k is taken, try k+1, k+2, k+3, ...

```c
void insert(HashTable* table, const char* key, int value) {
    int index = hash(key, table->size);
    int original = index;
    
    while (table->keys[index] != NULL) {
        if (strcmp(table->keys[index], key) == 0) {
            table->values[index] = value;  // Update
            return;
        }
        index = (index + 1) % table->size;  // Linear probe
        
        if (index == original) {
            // Table is full
            return;
        }
    }
    
    table->keys[index] = strdup(key);
    table->values[index] = value;
    table->count++;
}
```

**Problem:** Clustering. Consecutive occupied slots slow down search.

```
[_][X][X][X][_][_]  // Cluster forms
     ^
   Probing here takes multiple steps
```

### Quadratic Probing

Try k+1², k+2², k+3², ... Reduces clustering.

```c
index = (original_index + i * i) % table_size;
```

### Double Hashing

Use second hash function for probe step:

```c
int step = hash2(key);
index = (original_index + i * step) % table_size;
```

Best distribution, minimal clustering.

## Load Factor

**Load factor** = count / size

- Low load factor: Fast but wastes memory
- High load factor: Slow but space-efficient
- **Sweet spot:** 0.7-0.75 for chaining, 0.5 for open addressing

## Resizing

When load factor exceeds threshold, resize:

```c
void resize(HashTable* table) {
    int old_size = table->size;
    HashNode** old_buckets = table->buckets;
    
    table->size *= 2;  // Double size
    table->buckets = (HashNode**)calloc(table->size, sizeof(HashNode*));
    table->count = 0;
    
    // Rehash all elements
    for (int i = 0; i < old_size; i++) {
        HashNode* current = old_buckets[i];
        while (current != NULL) {
            insert(table, current->key, current->value);
            HashNode* temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    
    free(old_buckets);
}
```

**Why rehash?** Hash values depend on table size. Indices change after resize.

## Complexity

| Operation | Average | Worst |
|-----------|---------|-------|
| Insert    | O(1)    | O(n)  |
| Search    | O(1)    | O(n)  |
| Delete    | O(1)    | O(n)  |
| Space     | O(n)    | O(n)  |

With good hash function and proper load factor, hash tables provide **actual O(1)** in practice.

Worst case (all collisions) is rare with good hash function.

## When to Use Hash Tables

**Use hash tables when:**
- Need fast lookup by key
- Counting occurrences
- Caching/memoization
- Deduplication
- Symbol tables (compilers)
- Database indexing

**Don't use when:**
- Need sorted order (use BST)
- Need range queries (use BST)
- Small dataset (array/list is simpler)
- Memory constrained (overhead for hash structure)

## Common Applications

**Counting:**
```c
void count_words(char* text) {
    HashTable* counts = create_table(100);
    char* word = strtok(text, " ");
    
    while (word != NULL) {
        int count;
        if (search(counts, word, &count)) {
            insert(counts, word, count + 1);
        } else {
            insert(counts, word, 1);
        }
        word = strtok(NULL, " ");
    }
}
```

**Deduplication:**
```c
int has_duplicates(int arr[], int n) {
    HashTable* seen = create_table(n);
    
    for (int i = 0; i < n; i++) {
        char key[20];
        sprintf(key, "%d", arr[i]);
        
        int dummy;
        if (search(seen, key, &dummy)) {
            return 1;  // Duplicate found
        }
        insert(seen, key, 1);
    }
    
    return 0;
}
```

**Two Sum Problem:**
```c
int* two_sum(int arr[], int n, int target) {
    HashTable* table = create_table(n);
    
    for (int i = 0; i < n; i++) {
        int complement = target - arr[i];
        char key[20];
        sprintf(key, "%d", complement);
        
        int index;
        if (search(table, key, &index)) {
            // Found pair
            return create_pair(index, i);
        }
        
        sprintf(key, "%d", arr[i]);
        insert(table, key, i);
    }
    
    return NULL;
}
```

## Hash Table vs BST

| Feature | Hash Table | BST |
|---------|-----------|-----|
| Lookup | O(1) avg | O(log n) |
| Sorted | No | Yes |
| Range query | No | Yes |
| Min/Max | O(n) | O(log n) |
| Memory | More | Less |
| Ordered traversal | No | Yes |

Hash tables for speed. BSTs for order.

## Implementation Tips

**1. Choose table size as prime:**
Better distribution, fewer collisions.

**2. Handle deletions carefully:**
With open addressing, use tombstones (mark as deleted, don't actually remove).

**3. Implement good hash function:**
Poor hash = many collisions = poor performance.

**4. Monitor load factor:**
Resize before performance degrades.

**5. Free memory properly:**
Free keys, nodes, and table itself.

## Summary

Hash tables trade some memory and complexity for incredible speed. A good hash function converts keys to indices, collisions are handled with chaining or probing, and resizing maintains performance. When you need fast key-value lookup, hash tables are usually the answer.

Key insight: **Arrays are fast but inflexible. Hash functions bridge keys to indices, giving O(1) lookup for any key.**
