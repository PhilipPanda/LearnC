# Huffman Coding

## The Big Picture

Huffman coding assigns shorter bit sequences to more frequent symbols and longer sequences to rarer ones. The result is an **optimal prefix-free code** - no code is a prefix of another, so the bit stream is self-delimiting.

```
Symbol  Frequency   Fixed (8 bits)   Huffman code
  'D'     8           01000100           0       (1 bit)
  'C'     6           01000011           10      (2 bits)
  'B'     4           01000010           110     (3 bits)
  'A'     2           01000001           111     (3 bits)

Input "AABBBBCCCCCCDDDDDDDD":
  Fixed:   20 bytes = 160 bits
  Huffman: 2*3 + 4*3 + 6*2 + 8*1 = 38 bits ≈ 5 bytes
```

## Step 1: Count Frequencies

```c
int freq[256] = {0};
const char* input = "AABBBBCCCCCCDDDDDDDD";

for (int i = 0; input[i]; i++) {
    freq[(unsigned char)input[i]]++;
}
// freq['A'] = 2, freq['B'] = 4, freq['C'] = 6, freq['D'] = 8
```

## Step 2: Build a Min-Heap

All symbols start as leaf nodes sorted by frequency (lowest first).

```
Initial heap (lowest frequency first):
  [A:2]  [B:4]  [C:6]  [D:8]
```

A min-heap lets us always grab the two cheapest nodes in O(log n) time.

```c
typedef struct HuffNode {
    unsigned char symbol;
    int frequency;
    struct HuffNode* left;
    struct HuffNode* right;
} HuffNode;
```

## Step 3: Build the Tree

Repeatedly merge the two lowest-frequency nodes into a new internal node.

```
Round 1: Merge A:2 and B:4
  [AB:6]  [C:6]  [D:8]
   /  \
 A:2  B:4

Round 2: Merge AB:6 and C:6
  [ABC:12]  [D:8]
    /   \
  AB:6  C:6
  / \
A:2 B:4

Round 3: Merge D:8 and ABC:12
      [ABCD:20]
       /      \
     D:8     ABC:12
             /    \
           AB:6   C:6
           / \
         A:2 B:4
```

```c
while (heap->size > 1) {
    HuffNode* left  = heap_pop(heap);   // smallest
    HuffNode* right = heap_pop(heap);   // second smallest

    HuffNode* parent = node_create(0, left->frequency + right->frequency);
    parent->left  = left;
    parent->right = right;

    heap_push(heap, parent);
}
HuffNode* root = heap_pop(heap);
```

## Step 4: Generate Codes

Walk the tree. Going **left = append 0**, going **right = append 1**. Record the path to each leaf.

```
      [root]
      /    \
    D:8   [12]
    = 0   /   \
       [6]   C:6
       / \   = 10
     A:2 B:4
     =111 =110
```

```c
void generate_codes(HuffNode* node, unsigned int bits, int depth, HuffCode* codes) {
    if (!node->left && !node->right) {
        codes[node->symbol].bits   = bits;
        codes[node->symbol].length = depth;
        return;
    }
    generate_codes(node->left,  (bits << 1) | 0, depth + 1, codes);
    generate_codes(node->right, (bits << 1) | 1, depth + 1, codes);
}
```

## Step 5: Encode

Replace each symbol with its bit code and pack into bytes.

```c
// Encoding "ABCD"
// A = 111, B = 110, C = 10, D = 0
// Bit stream: 111 110 10 0 = 11111010 (padded to byte boundary)
```

## Step 6: Decode

Feed bits into the tree. When you reach a leaf, emit the symbol and reset to root.

```
Bit stream: 0 111 110 10
            │  │   │   │
            D  A   B   C   ← follow bits left(0) or right(1) from root
```

```c
HuffNode* current = root;
for each bit in stream:
    current = bit ? current->right : current->left;
    if (is_leaf(current)):
        emit(current->symbol)
        current = root
```

## Prefix-Free Property

No Huffman code is a prefix of another. This is what makes the bit stream self-delimiting - you always know when a code ends because only leaves represent valid symbols.

```
Codes: D=0, C=10, B=110, A=111

Bit stream: 0 1 0 1 1 0 1 1 1
             D   C   B     A
```

If '0' were both D's code and the start of another code, decoding would be ambiguous. The tree structure guarantees this never happens.

## Complexity

| Operation | Time | Space |
|---|---|---|
| Count frequencies | O(n) | O(k) |
| Build heap | O(k log k) | O(k) |
| Build tree | O(k log k) | O(k) |
| Generate codes | O(k) | O(k) |
| Encode | O(n) | O(n) |
| Decode | O(n) | O(n) |

n = input length, k = number of distinct symbols (≤ 256 for bytes)

## Limitations

### Static Huffman
The code table is built once from the full input. The decoder needs the same table, so you must either:
- Store the table in the compressed file (adds overhead for small inputs)
- Agree on a fixed table in advance (only works for known distributions)

### Adaptive Huffman
Both encoder and decoder build the table dynamically as they process the data, so no table needs to be transmitted. More complex to implement.

### Arithmetic Coding
Achieves entropy more closely than Huffman (which must use whole bits per symbol). Used in JPEG and modern compressors but harder to implement.

## Real-World Use

Huffman is rarely used alone. It is the **entropy coding back-end** in most serious compressors:

```
DEFLATE (ZIP/gzip/PNG):
  LZ77 (remove repeated substrings) → Huffman (encode the tokens)

JPEG:
  DCT + quantization (lossy transform) → Huffman (encode the coefficients)

MP3:
  MDCT + psychoacoustic model → Huffman (encode the quantized values)
```

## Summary

| Step | What Happens |
|---|---|
| 1. Frequencies | Count how often each symbol appears |
| 2. Min-heap | Order symbols by frequency |
| 3. Build tree | Merge two smallest nodes repeatedly |
| 4. Generate codes | Walk tree: left=0, right=1 |
| 5. Encode | Replace symbols with their bit codes |
| 6. Decode | Follow bits through tree until leaf |

The key insight: **more frequent symbols sit closer to the root**, so they get shorter paths (shorter codes). This is provably optimal for single-symbol entropy coding.
