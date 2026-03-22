/*
 * Huffman Coding
 *
 * Learn optimal prefix-free compression:
 * - Count symbol frequencies in input data
 * - Build a Huffman tree using a min-heap
 * - Generate variable-length bit codes
 * - Encode and decode messages
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SYMBOLS 256

// ===== Huffman Tree Node =====

typedef struct HuffNode {
    unsigned char symbol;    // The byte this leaf represents
    int frequency;           // How often this symbol appears
    struct HuffNode* left;
    struct HuffNode* right;
} HuffNode;

// ===== Min-Heap for Building the Tree =====

typedef struct {
    HuffNode** nodes;
    int size;
    int capacity;
} MinHeap;

MinHeap* heap_create(int capacity) {
    MinHeap* h = malloc(sizeof(MinHeap));
    h->nodes = malloc(capacity * sizeof(HuffNode*));
    h->size = 0;
    h->capacity = capacity;
    return h;
}

void heap_swap(MinHeap* h, int a, int b) {
    HuffNode* temp = h->nodes[a];
    h->nodes[a] = h->nodes[b];
    h->nodes[b] = temp;
}

void heap_push(MinHeap* h, HuffNode* node) {
    h->nodes[h->size] = node;
    int i = h->size++;

    // Bubble up to maintain min-heap property
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->nodes[parent]->frequency <= h->nodes[i]->frequency) break;
        heap_swap(h, parent, i);
        i = parent;
    }
}

HuffNode* heap_pop(MinHeap* h) {
    HuffNode* min = h->nodes[0];
    h->nodes[0] = h->nodes[--h->size];

    // Bubble down to restore heap
    int i = 0;
    while (1) {
        int left = 2*i + 1, right = 2*i + 2, smallest = i;
        if (left  < h->size && h->nodes[left]->frequency  < h->nodes[smallest]->frequency) smallest = left;
        if (right < h->size && h->nodes[right]->frequency < h->nodes[smallest]->frequency) smallest = right;
        if (smallest == i) break;
        heap_swap(h, i, smallest);
        i = smallest;
    }

    return min;
}

// ===== Huffman Tree =====

HuffNode* node_create(unsigned char symbol, int frequency) {
    HuffNode* node = malloc(sizeof(HuffNode));
    node->symbol    = symbol;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}

// Build the Huffman tree from a frequency table
HuffNode* huffman_build_tree(const int* freq, int num_symbols) {
    MinHeap* heap = heap_create(num_symbols);

    // Push all symbols with non-zero frequency as leaf nodes
    for (int i = 0; i < num_symbols; i++) {
        if (freq[i] > 0) {
            heap_push(heap, node_create((unsigned char)i, freq[i]));
        }
    }

    // Merge the two smallest nodes until only the root remains
    while (heap->size > 1) {
        HuffNode* left  = heap_pop(heap);
        HuffNode* right = heap_pop(heap);

        HuffNode* parent = node_create(0, left->frequency + right->frequency);
        parent->left  = left;
        parent->right = right;

        heap_push(heap, parent);
    }

    HuffNode* root = heap_pop(heap);
    free(heap->nodes);
    free(heap);
    return root;
}

void huffman_free_tree(HuffNode* node) {
    if (!node) return;
    huffman_free_tree(node->left);
    huffman_free_tree(node->right);
    free(node);
}

// ===== Code Table =====

typedef struct {
    unsigned int bits;   // The code bits (right-aligned)
    int length;          // Number of valid bits
} HuffCode;

// Recursively walk the tree and record the code for each leaf
void generate_codes(HuffNode* node, unsigned int bits, int depth, HuffCode* codes) {
    if (!node) return;

    if (!node->left && !node->right) {
        // Leaf node - this is the code for node->symbol
        codes[node->symbol].bits   = bits;
        codes[node->symbol].length = depth;
        return;
    }

    // Going left appends a 0 bit, going right appends a 1 bit
    generate_codes(node->left,  (bits << 1) | 0, depth + 1, codes);
    generate_codes(node->right, (bits << 1) | 1, depth + 1, codes);
}

// ===== Print Helpers =====

void print_bits(unsigned int bits, int length) {
    for (int i = length - 1; i >= 0; i--) {
        printf("%d", (bits >> i) & 1);
    }
}

void print_tree(HuffNode* node, const char* prefix, int is_right) {
    if (!node) return;

    printf("%s", prefix);
    printf(is_right ? "└── " : "├── ");

    if (!node->left && !node->right) {
        char ch = node->symbol >= 32 && node->symbol < 127 ? (char)node->symbol : '.';
        printf("'%c' (freq=%d)\n", ch, node->frequency);
    } else {
        printf("[freq=%d]\n", node->frequency);
    }

    char new_prefix[256];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_right ? "    " : "│   ");

    if (node->left)  print_tree(node->left,  new_prefix, 0);
    if (node->right) print_tree(node->right, new_prefix, 1);
}

// ===== Demo Functions =====

void demo_huffman_encoding() {
    printf("--- Huffman Encoding Demo ---\n\n");

    const char* input = "AABBBBCCCCCCDDDDDDDD";
    int input_len = (int)strlen(input);

    printf("Input: \"%s\" (%d bytes)\n\n", input, input_len);

    // Count frequencies
    int freq[MAX_SYMBOLS] = {0};
    for (int i = 0; i < input_len; i++) {
        freq[(unsigned char)input[i]]++;
    }

    printf("Frequencies:\n");
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) {
            printf("  '%c': %d\n", i, freq[i]);
        }
    }
    printf("\n");

    // Build tree and generate codes
    HuffNode* root = huffman_build_tree(freq, MAX_SYMBOLS);
    HuffCode codes[MAX_SYMBOLS] = {0};
    generate_codes(root, 0, 0, codes);

    printf("Huffman Codes (frequent symbols get shorter codes):\n");
    printf("  Symbol  Freq  Code       Bits\n");
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) {
            printf("    '%c'   %4d  ", i, freq[i]);
            print_bits(codes[i].bits, codes[i].length);
            printf("  (%d bit%s)\n", codes[i].length, codes[i].length == 1 ? "" : "s");
        }
    }
    printf("\n");

    // Calculate encoded size
    int total_bits = 0;
    for (int i = 0; i < input_len; i++) {
        total_bits += codes[(unsigned char)input[i]].length;
    }
    int encoded_bytes = (total_bits + 7) / 8;

    printf("Original: %d bytes (%d bits)\n", input_len, input_len * 8);
    printf("Encoded:  %d bytes (%d bits)\n", encoded_bytes, total_bits);
    float ratio = (float)encoded_bytes / input_len * 100.0f;
    printf("Ratio:    %.1f%% of original\n\n", ratio);

    huffman_free_tree(root);
}

void demo_tree_visualization() {
    printf("--- Huffman Tree Structure ---\n\n");

    const char* input = "AABBBCCCC";
    int input_len = (int)strlen(input);

    printf("Input: \"%s\"\n\n", input);

    int freq[MAX_SYMBOLS] = {0};
    for (int i = 0; i < input_len; i++) {
        freq[(unsigned char)input[i]]++;
    }

    HuffNode* root = huffman_build_tree(freq, MAX_SYMBOLS);

    printf("Tree (left=0, right=1):\n");
    printf("[freq=%d] (root)\n", root->frequency);
    if (root->left)  print_tree(root->left,  "", 0);
    if (root->right) print_tree(root->right, "", 1);
    printf("\n");

    HuffCode codes[MAX_SYMBOLS] = {0};
    generate_codes(root, 0, 0, codes);

    printf("Generated codes:\n");
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        if (freq[i] > 0) {
            printf("  '%c' (freq %d) = ", i, freq[i]);
            print_bits(codes[i].bits, codes[i].length);
            printf("\n");
        }
    }
    printf("\n");

    huffman_free_tree(root);
}

void demo_encode_decode() {
    printf("--- Encode Then Decode ---\n\n");

    const char* message = "HELLO";
    int msg_len = (int)strlen(message);

    printf("Message: \"%s\"\n\n", message);

    // Build codes from message frequencies
    int freq[MAX_SYMBOLS] = {0};
    for (int i = 0; i < msg_len; i++) {
        freq[(unsigned char)message[i]]++;
    }

    HuffNode* root = huffman_build_tree(freq, MAX_SYMBOLS);
    HuffCode codes[MAX_SYMBOLS] = {0};
    generate_codes(root, 0, 0, codes);

    // Encode: show the bit stream
    printf("Encoded bit stream:\n  ");
    int total_bits = 0;
    for (int i = 0; i < msg_len; i++) {
        unsigned char c = (unsigned char)message[i];
        print_bits(codes[c].bits, codes[c].length);
        printf(" ");
        total_bits += codes[c].length;
    }
    printf("\n  (%d total bits = %d bytes)\n\n", total_bits, (total_bits + 7) / 8);

    // Decode: walk the tree following each bit
    printf("Decoded (walk tree with bits): ");
    HuffNode* current = root;
    for (int i = 0; i < msg_len; i++) {
        unsigned char c = (unsigned char)message[i];
        for (int b = codes[c].length - 1; b >= 0; b--) {
            int bit = (codes[c].bits >> b) & 1;
            current = bit ? current->right : current->left;
            if (!current->left && !current->right) {
                printf("%c", current->symbol);
                current = root;  // Reset to root for next symbol
            }
        }
    }
    printf("\n\n");

    huffman_free_tree(root);
}

void demo_compression_comparison() {
    printf("--- Compression Ratio by Data Type ---\n\n");

    struct {
        const char* name;
        const char* data;
    } test_cases[] = {
        { "Uniform",     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
        { "Two symbols", "AAAAABBBBBAAAAAABBBBBBAAAAABB" },
        { "Four symbols","AABBBBCCCCCCDDDDDDDDDDDDDDDD" },
        { "All unique",  "ABCDEFGHIJKLMNOPQRSTUVWXYZ012" },
    };

    int n = (int)(sizeof(test_cases) / sizeof(test_cases[0]));

    printf("%-14s  %6s  %7s  %8s\n", "Type", "Input", "Huffman", "Ratio");
    printf("%-14s  %6s  %7s  %8s\n", "----", "-----", "-------", "-----");

    for (int i = 0; i < n; i++) {
        const char* data = test_cases[i].data;
        int input_len = (int)strlen(data);

        int freq[MAX_SYMBOLS] = {0};
        for (int j = 0; j < input_len; j++) freq[(unsigned char)data[j]]++;

        HuffNode* root = huffman_build_tree(freq, MAX_SYMBOLS);
        HuffCode codes[MAX_SYMBOLS] = {0};
        generate_codes(root, 0, 0, codes);

        int total_bits = 0;
        for (int j = 0; j < input_len; j++) {
            total_bits += codes[(unsigned char)data[j]].length;
        }
        int encoded_bytes = (total_bits + 7) / 8;
        float ratio = (float)encoded_bytes / input_len * 100.0f;

        printf("%-14s  %6d  %7d  %7.1f%%\n",
               test_cases[i].name, input_len, encoded_bytes, ratio);

        huffman_free_tree(root);
    }
    printf("\n");
}

int main(void) {
    printf("=== Huffman Coding ===\n\n");
    printf("Optimal prefix-free compression: frequent symbols get shorter codes.\n\n");

    demo_huffman_encoding();
    demo_tree_visualization();
    demo_encode_decode();
    demo_compression_comparison();

    printf("=== Summary ===\n");
    printf("Huffman coding demonstrated:\n");
    printf("  1. Count how often each symbol appears\n");
    printf("  2. Build a min-heap with all symbols\n");
    printf("  3. Merge two lowest-frequency nodes until one tree remains\n");
    printf("  4. Assign shorter codes to more frequent symbols\n");
    printf("  5. Decode by walking the tree bit by bit\n");
    printf("\nKey properties:\n");
    printf("  - Prefix-free: no code is a prefix of another (self-delimiting)\n");
    printf("  - Optimal: provably best fixed-code assignment for given frequencies\n");
    printf("  - Lossless: perfect reconstruction guaranteed\n");
    printf("\nReal-world uses:\n");
    printf("  - DEFLATE (ZIP, gzip, PNG)\n");
    printf("  - JPEG and MPEG (combined with other algorithms)\n");
    printf("  - MP3 audio\n");
    printf("  - Fax compression\n");

    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
