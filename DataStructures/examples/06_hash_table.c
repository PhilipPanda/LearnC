/*
 * 06_hash_table.c
 * 
 * Hash table implementation with chaining for collision resolution.
 * Demonstrates fast O(1) key-value lookups.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_SIZE 10
#define LOAD_FACTOR_THRESHOLD 0.75

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

// Hash function (DJB2)
unsigned int hash(const char* key, int table_size) {
    unsigned int hash = 5381;
    
    while (*key) {
        hash = ((hash << 5) + hash) + (*key);  // hash * 33 + c
        key++;
    }
    
    return hash % table_size;
}

// Create hash table
HashTable* create_table(int size) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->buckets = (HashNode**)calloc(size, sizeof(HashNode*));
    return table;
}

// Insert or update key-value pair
void insert(HashTable* table, const char* key, int value) {
    int index = hash(key, table->size);
    
    // Check if key already exists
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;  // Update existing
            return;
        }
        current = current->next;
    }
    
    // Insert new node at head of chain
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->count++;
}

// Search for key
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

// Delete key
void delete_key(HashTable* table, const char* key) {
    int index = hash(key, table->size);
    
    HashNode* current = table->buckets[index];
    HashNode* prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                // Head of chain
                table->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->key);
            free(current);
            table->count--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Get load factor
float load_factor(HashTable* table) {
    return (float)table->count / table->size;
}

// Print hash table
void print_table(HashTable* table) {
    printf("Hash Table (size=%d, count=%d, load=%.2f):\n", 
           table->size, table->count, load_factor(table));
    
    for (int i = 0; i < table->size; i++) {
        if (table->buckets[i] != NULL) {
            printf("  [%d]: ", i);
            HashNode* current = table->buckets[i];
            while (current != NULL) {
                printf("(\"%s\": %d) ", current->key, current->value);
                if (current->next != NULL) printf("-> ");
                current = current->next;
            }
            printf("\n");
        }
    }
}

// Free hash table
void free_table(HashTable* table) {
    for (int i = 0; i < table->size; i++) {
        HashNode* current = table->buckets[i];
        while (current != NULL) {
            HashNode* temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}

// Count word frequencies
void word_frequency_example(const char* text) {
    HashTable* freq = create_table(20);
    char text_copy[1000];
    strcpy(text_copy, text);
    
    char* word = strtok(text_copy, " ");
    while (word != NULL) {
        int count;
        if (search(freq, word, &count)) {
            insert(freq, word, count + 1);
        } else {
            insert(freq, word, 1);
        }
        word = strtok(NULL, " ");
    }
    
    printf("Word frequencies:\n");
    for (int i = 0; i < freq->size; i++) {
        HashNode* current = freq->buckets[i];
        while (current != NULL) {
            printf("  \"%s\": %d times\n", current->key, current->value);
            current = current->next;
        }
    }
    
    free_table(freq);
}

int main(void) {
    printf("=== Hash Table ===\n\n");
    
    // Example 1: Basic operations
    printf("Example 1: Basic operations\n");
    printf("---------------------------\n");
    HashTable* table = create_table(7);  // Small size to show collisions
    
    printf("Inserting key-value pairs...\n");
    insert(table, "apple", 100);
    insert(table, "banana", 200);
    insert(table, "cherry", 300);
    insert(table, "date", 400);
    insert(table, "elderberry", 500);
    
    printf("\n");
    print_table(table);
    
    // Example 2: Searching
    printf("\n\nExample 2: Searching\n");
    printf("--------------------\n");
    const char* keys[] = {"apple", "banana", "grape"};
    for (int i = 0; i < 3; i++) {
        int value;
        if (search(table, keys[i], &value)) {
            printf("  Found \"%s\": %d\n", keys[i], value);
        } else {
            printf("  \"%s\" not found\n", keys[i]);
        }
    }
    
    // Example 3: Updating values
    printf("\n\nExample 3: Updating values\n");
    printf("--------------------------\n");
    printf("Before: apple = ");
    int val;
    search(table, "apple", &val);
    printf("%d\n", val);
    
    insert(table, "apple", 999);
    
    printf("After update: apple = ");
    search(table, "apple", &val);
    printf("%d\n", val);
    
    // Example 4: Deletion
    printf("\n\nExample 4: Deletion\n");
    printf("-------------------\n");
    printf("Before deletion:\n");
    print_table(table);
    
    printf("\nDeleting \"banana\"...\n");
    delete_key(table, "banana");
    
    printf("\nAfter deletion:\n");
    print_table(table);
    
    // Example 5: Hash collisions
    printf("\n\nExample 5: Understanding hash collisions\n");
    printf("----------------------------------------\n");
    HashTable* collision_demo = create_table(5);  // Very small to force collisions
    
    printf("Table size: 5\n\n");
    const char* words[] = {"cat", "dog", "rat", "bat", "hat"};
    
    for (int i = 0; i < 5; i++) {
        unsigned int h = hash(words[i], 5);
        printf("  \"%s\" hashes to bucket %u\n", words[i], h);
        insert(collision_demo, words[i], (i + 1) * 10);
    }
    
    printf("\nCollisions are handled with chaining:\n");
    print_table(collision_demo);
    
    // Example 6: Word frequency counter
    printf("\n\nExample 6: Word frequency counter\n");
    printf("----------------------------------\n");
    const char* text = "the quick brown fox jumps over the lazy dog the fox";
    printf("Text: \"%s\"\n\n", text);
    word_frequency_example(text);
    
    // Example 7: Load factor
    printf("\n\nExample 7: Load factor monitoring\n");
    printf("----------------------------------\n");
    HashTable* load_test = create_table(10);
    
    printf("Initial load factor: %.2f\n", load_factor(load_test));
    
    for (int i = 0; i < 8; i++) {
        char key[20];
        sprintf(key, "key%d", i);
        insert(load_test, key, i);
        printf("After inserting %s: load = %.2f\n", key, load_factor(load_test));
    }
    
    if (load_factor(load_test) > LOAD_FACTOR_THRESHOLD) {
        printf("\nLoad factor exceeded %.2f - should resize!\n", LOAD_FACTOR_THRESHOLD);
    }
    
    // Cleanup
    free_table(table);
    free_table(collision_demo);
    free_table(load_test);
    
    printf("\n\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * Hash Table Concepts:
 * 
 * Goal: O(1) lookup by key
 * 
 * How it works:
 * 1. Hash function converts key -> index
 * 2. Store value at that index
 * 3. Retrieve value using same hash
 * 
 * Example:
 *   hash("alice") = 3 -> buckets[3] = 42
 *   lookup("alice") -> hash("alice") = 3 -> return buckets[3]
 * 
 * Hash Function Requirements:
 * - Deterministic (same key -> same hash)
 * - Fast to compute
 * - Uniform distribution
 * - Minimize collisions
 * 
 * Collisions:
 * - Two keys hash to same index
 * - Inevitable (pigeonhole principle)
 * - Must handle gracefully
 * 
 * Collision Resolution - Chaining:
 * - Each bucket is a linked list
 * - Multiple items can share bucket
 * - Search within chain
 * 
 * Visual:
 * [0] -> NULL
 * [1] -> ("bob", 17) -> NULL
 * [2] -> NULL
 * [3] -> ("alice", 42) -> ("eve", 7) -> NULL  // Collision!
 * [4] -> NULL
 * 
 * Alternative: Open Addressing
 * - Store everything in array
 * - On collision, probe for next empty slot
 * - Linear probing: try next slot
 * - Quadratic probing: try 1², 2², 3² slots away
 * - Double hashing: use second hash function
 * 
 * Load Factor:
 * - load = count / size
 * - High load = more collisions = slower
 * - Low load = wasted space
 * - Sweet spot: 0.7-0.75
 * - Resize when exceeded
 * 
 * Complexity:
 * - Average: O(1) for insert, search, delete
 * - Worst: O(n) if all hash to same bucket
 * - With good hash function: actual O(1)
 * 
 * When to Use:
 * - Fast lookup by key
 * - Counting frequencies
 * - Caching
 * - Deduplication
 * - Symbol tables
 * - Database indexing
 * 
 * Hash Table vs BST:
 * 
 * Hash Table:
 * + Faster lookup O(1) vs O(log n)
 * - No ordering
 * - No range queries
 * - More memory
 * 
 * BST:
 * + Ordered traversal
 * + Range queries
 * + Min/max in O(log n)
 * - Slower lookup O(log n)
 * 
 * Real Applications:
 * - Dictionaries/maps in languages
 * - Database indexing
 * - Caching (LRU cache uses hash + list)
 * - Compiler symbol tables
 * - Deduplication
 * - Counting problems
 * 
 * Famous Hash Functions:
 * - MD5, SHA (cryptographic, slow)
 * - MurmurHash, CityHash (fast, good distribution)
 * - DJB2 (simple, pretty good - used here)
 * 
 * Try:
 * - Implement resizing
 * - Try open addressing instead
 * - Different hash functions
 * - Benchmark vs BST
 * - Two-sum problem with hash table
 */
