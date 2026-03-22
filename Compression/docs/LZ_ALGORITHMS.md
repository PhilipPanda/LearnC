# LZ Algorithms

## The Big Picture

The Lempel-Ziv family (LZ77, LZ78, and their descendants) replaces repeated substrings with references to earlier occurrences. This is **dictionary compression**: the already-written output is the dictionary.

```
Input:   "the cat sat on the mat"
                            ^^^
Output:  "the cat sat on [back=14, len=4]mat"
         (copies "the " from 14 bytes back)
```

## LZ77 (1977)

The original algorithm by Abraham Lempel and Jacob Ziv.

### Concept: Sliding Window

```
Already encoded        Current position
       ↓                     ↓
[... the cat sat on ] [ the mat]
 ←─── window ───────→   ↑
                    Search here for matches
```

The encoder looks backward through a **window** (e.g., 32KB) to find the longest match for the current position.

### Token Format

Each output token is a triple: **(offset, length, next)**

- **offset**: how many bytes back the match starts
- **length**: how many bytes to copy
- **next**: the literal byte that follows the match

```
Input:  a b c a b c a b c
                ↑
Position 3: "abc" matches at position 0 (offset=3, length=3)
Token: (3, 3, 'a')  ← copy 3 bytes from 3 back, then emit 'a'
```

### Encoding Example

```
Input: "ABCABCABC"

pos=0: no window, emit literal → (0, 0, 'A')
pos=1: no match   → (0, 0, 'B')
pos=2: no match   → (0, 0, 'C')
pos=3: "ABC" matches at pos 0 (offset=3, length=3) → (3, 3, 'A')
pos=7: "BC" matches at pos 1 (offset=6, length=2) → (6, 2, '\0')
```

### Decoding

The decoder replays the tokens. When it sees a back-reference, it copies from the **already-written output buffer**.

```c
for each token (offset, length, next):
    if length > 0:
        copy output[pos - offset ... pos - offset + length] to output[pos...]
        pos += length
    output[pos++] = next
```

Note: the copy can **overlap** the source! This allows "run extension":

```
Token: (1, 8, 'X')  starting from "A" in output
→ AAAAAAAAX   (copies A, then A, then A... each newly written A is copied too)
```

### Algorithm Complexity

| Operation | Time |
|---|---|
| Encode (naive search) | O(n * W) where W = window size |
| Encode (hash chains) | O(n) average |
| Decode | O(n) |

### Trade-offs

| Parameter | Larger Value | Smaller Value |
|---|---|---|
| Window size | Better ratio, more memory | Faster, less memory |
| Max match length | Better ratio | Faster |
| Token size | More overhead | Less overhead |

## LZ78 (1978)

The follow-up paper. Instead of a sliding window, LZ78 builds an **explicit dictionary** of substrings.

```
Start with empty dictionary.
Read symbols until you find a string not in the dictionary.
Emit (dict_index_of_prefix, new_symbol).
Add the new string to the dictionary.
```

Less used today than LZ77 - patent issues historically drove adoption of LZ77 variants.

## LZW (Lempel-Ziv-Welch, 1984)

An LZ78 variant where both encoder and decoder build the same dictionary simultaneously - no dictionary needs to be transmitted.

- Used in **GIF** image format
- Used in Unix **compress** utility
- Patent expired in 2003

## LZSS (Lempel-Ziv-Storer-Szymanski)

An LZ77 improvement: add a **flag bit** before each token to distinguish literals from back-references. Skip encoding a back-reference if it would be larger than the literal.

```
Flag=0: literal byte follows
Flag=1: (offset, length) back-reference follows

"AB" with no match: 0 'A'  0 'B'          (flag + literal each)
"ABC" match len 5:  1 (offset)(length)     (flag + 2 bytes)
```

This avoids the overhead of (0, 0, literal) for unmatched bytes.

## DEFLATE

The dominant general-purpose lossless algorithm. Combines LZ77 + Huffman.

```
Input
  ↓
LZ77 matching → sequence of literals and back-references
  ↓
Huffman coding → optimal bit codes for the tokens
  ↓
Output
```

Used in:
- **ZIP** (.zip)
- **gzip** (.gz)
- **PNG** images
- **zlib** library
- **HTTP** Content-Encoding: deflate

## Modern Algorithms

### LZ4

Prioritizes **speed** over ratio. Extremely fast decode (useful for RAM compression, network protocols).

```
Ratio:  Moderate (similar to early DEFLATE)
Speed:  400+ MB/s encode, 1+ GB/s decode
Use:    Real-time compression, databases, network I/O
```

### Zstandard (zstd, Facebook 2016)

Best general-purpose algorithm for most workloads today.

```
Ratio:  Better than gzip
Speed:  Faster than gzip in both directions
Use:    Linux kernel, Facebook, Python 3.11+ .pyc files
```

### Brotli (Google 2013)

Optimized for HTTP content compression.

```
Ratio:  Better than gzip, similar to zstd
Speed:  Slower to encode (acceptable for pre-compressed web assets)
Use:    Web servers, Content-Encoding: br
```

### LZMA (7-Zip)

Maximum ratio, slower speed.

```
Ratio:  Best among common algorithms
Speed:  Slow encode, moderate decode
Use:    Software distribution, archiving where ratio matters most
```

## Comparison Table

| Algorithm | Year | Ratio | Speed | Used In |
|---|---|---|---|---|
| RLE | Ancient | Poor (general) | Very fast | BMP, fax |
| LZW | 1984 | Moderate | Fast | GIF, compress |
| DEFLATE | 1993 | Good | Moderate | ZIP, gzip, PNG |
| LZ4 | 2011 | Moderate | Very fast | Databases, RAM |
| Zstandard | 2016 | Very good | Fast | Linux, Facebook |
| Brotli | 2013 | Very good | Slow encode | HTTP |
| LZMA | 1998 | Excellent | Slow | 7-Zip, xz |

## Choosing an Algorithm

```
Need maximum compatibility?     → DEFLATE/gzip (universal support)
Need maximum ratio?             → LZMA/xz
Need best ratio+speed balance?  → Zstandard
Need maximum decode speed?      → LZ4
Compressing for web?            → Brotli (pre-compress) or gzip (dynamic)
Learning / building your own?   → LZ77 or RLE (simplest to implement)
```

## Summary

The LZ family works because real data **repeats itself**:
- Text reuses common words and phrases
- Source code repeats keywords and patterns
- Binary files have repeated instruction sequences and data structures

By replacing those repetitions with short back-references, we eliminate redundancy without losing any information. Combined with Huffman coding for the remaining symbols, this is powerful enough to compress most real-world data by 50-80%.
