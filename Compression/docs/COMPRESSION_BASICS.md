# Compression Basics

## The Big Picture

Data compression reduces the number of bits needed to represent information. It works by finding and eliminating **redundancy** - patterns, repetitions, or predictable structure in data.

```
Original:   AAAAAABBBBCCCC  (14 bytes)
      ↓
  Compress
      ↓
Compressed: 6A4B4C         (6 bytes)
      ↓
  Decompress
      ↓
Original:   AAAAAABBBBCCCC  (14 bytes)
```

## Lossless vs Lossy

### Lossless Compression

Every single bit is recovered exactly.

```
Original  →  Compress  →  Decompress  →  Identical copy
```

**Uses:** ZIP, gzip, PNG, text files, source code, executables

**How:** Exploit statistical redundancy (repeated patterns, skewed symbol frequencies)

### Lossy Compression

Some information is permanently discarded - accepted for formats where small errors are imperceptible.

```
Original  →  Compress  →  Decompress  →  Approximation (good enough)
```

**Uses:** JPEG (images), MP3 (audio), H.264 (video)

**How:** Discard information humans cannot easily perceive (high-frequency details, quiet audio)

## Entropy: The Theoretical Limit

**Entropy** (Claude Shannon, 1948) measures the minimum bits needed per symbol given a probability distribution.

```
H = -∑ p(x) * log₂(p(x))
```

| Distribution | Entropy | Meaning |
|---|---|---|
| 50% A, 50% B | 1.0 bit/symbol | Maximum uncertainty |
| 75% A, 25% B | 0.81 bits/symbol | Some predictability |
| 90% A, 10% B | 0.47 bits/symbol | Very predictable |
| 100% A | 0.0 bits/symbol | No information |

**Practical takeaway:** If your data has many repeated or predictable patterns, entropy is low and it compresses well. Random data has maximum entropy and barely compresses at all.

```c
// This compresses well - low entropy
char low_entropy[] = "AAAAAAAAAAAABBBBBBCCCC";

// This compresses poorly - high entropy
char high_entropy[] = "aZbYcXdWeVfUgThSiRjQ";
```

## How Compression Algorithms Work

### Statistical Coding (Huffman, Arithmetic)

Assign short codes to frequent symbols, long codes to rare symbols.

```
Symbol  Freq   Fixed (8 bits)   Huffman
  'A'    50%      01000001         0       (1 bit!)
  'B'    30%      01000010         10      (2 bits)
  'C'    20%      01000011         11      (2 bits)

Average bits per symbol:
  Fixed:   8.0 bits
  Huffman: 0.5*1 + 0.3*2 + 0.2*2 = 1.5 bits
```

### Dictionary Coding (LZ77, LZ78, LZW)

Replace repeated substrings with references to earlier occurrences.

```
Input:  "the cat sat on the mat"
                            ^^^
                     References back to "the " at position 0
Output: "the cat sat on [back=14,len=4]mat"
```

### Transform Coding

Rearrange data to make it more compressible, then apply statistical coding.

```
BWT (Burrows-Wheeler Transform):
  "banana" → "nnbaaa" (more runs, compresses better)
```

## Compression Ratio

```c
// Always expressed relative to original size
float ratio = (float)compressed_size / original_size;

// Examples:
ratio = 0.5   // 2:1 compression (compressed to half the size)
ratio = 0.25  // 4:1 compression
ratio = 1.5   // Expansion! (data got bigger - can happen with RLE on random data)
```

## What Compresses Well

| Data Type | Why It Compresses |
|---|---|
| Text | Limited alphabet, common words repeat |
| Source code | Small character set, repeated keywords |
| Log files | Timestamps and labels repeat |
| Database exports | Field names and formats repeat |
| Simple images | Large solid-color regions |
| Executables | Instruction patterns repeat |

## What Compresses Poorly

| Data Type | Why It Doesn't Compress |
|---|---|
| Already compressed (ZIP, MP3, JPEG) | Redundancy already removed |
| Encrypted data | Looks random by design |
| Truly random data | No pattern to exploit |
| Small files | Header overhead dominates |

## Algorithm Families

### Run-Length Encoding (RLE)
- Replace runs of identical bytes with count+value
- `AAAAAABBB` → `6A3B`
- Extremely simple, fast
- Only helps with literal repetition

### Huffman Coding
- Variable-length codes based on symbol frequency
- Frequent symbols get short codes
- Optimal for single-symbol statistics
- Used inside nearly every modern compressor

### LZ Family (Lempel-Ziv)
- Find repeated substrings, replace with back-references
- LZ77: sliding window
- LZ78/LZW: dictionary built on the fly
- Foundation of ZIP, gzip, GIF

### DEFLATE
- LZ77 + Huffman combined
- Used in ZIP, gzip, PNG, zlib
- Best general-purpose lossless algorithm for decades

### Modern Algorithms
- **Brotli** (Google): DEFLATE successor, used in HTTP compression
- **Zstandard** (Facebook): high speed + high ratio
- **LZMA** (7-Zip): maximum ratio, slower

## Compression Pipeline (Real-World)

Modern compressors use multiple stages:

```
Input
  ↓
Preprocessing (delta filter, BWT, etc.)
  ↓
LZ dictionary matching (find back-references)
  ↓
Huffman / arithmetic coding (encode symbols optimally)
  ↓
Compressed output
```

## Summary

| Property | Meaning |
|---|---|
| Lossless | Perfect reconstruction |
| Ratio | compressed_size / original_size |
| Entropy | Theoretical minimum bits per symbol |
| Redundancy | Exploitable patterns |

**Key insight:** Compression trades computation for space. More passes, smarter algorithms, and larger dictionaries all give better ratios at the cost of CPU and memory.

Understanding these fundamentals makes the internals of ZIP, PNG, HTTP compression, and video codecs much less mysterious!
