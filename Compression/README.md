# Compression in C

Learn data compression algorithms in C. Run-Length Encoding, Huffman coding, LZ77 sliding window - everything you need to understand how compression works and build your own compressors.

## What you get

- Run-Length Encoding (RLE) - the simplest compression algorithm
- Huffman coding - optimal prefix-free codes based on symbol frequency
- LZ77 sliding window - back-references, the basis for gzip and zip
- File compression - compress and decompress real files with a custom format

All algorithms implemented from scratch. No external libraries - pure C for learning.

## Building

```bash
# Windows
cd examples
build_all.bat

# Linux / MSYS2
cd examples
./build_all.sh
```

## Documentation

- **[Compression Basics](docs/COMPRESSION_BASICS.md)** - How compression works, entropy, lossless vs lossy
- **[Huffman Coding](docs/HUFFMAN.md)** - Frequency tables, tree construction, prefix-free codes
- **[LZ Algorithms](docs/LZ_ALGORITHMS.md)** - Sliding window, LZ77, DEFLATE, real-world variants

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_rle | Run-Length Encoding: encode repeated bytes as count+value pairs |
| 02_huffman | Huffman coding: frequency table, min-heap, variable-length codes |
| 03_lz77 | LZ77: sliding window search, back-references, token encoding |
| 04_file_compression | Compress/decompress real files with a custom binary format |

Start with 01 and 02 to understand the fundamentals, then see how they come together in real file compression.

## What this teaches

- How lossless compression algorithms work internally
- Entropy and why some data compresses better than others
- Data structures: min-heaps, binary trees, sliding windows
- Bit manipulation and variable-length codes
- Binary file formats with headers and magic numbers
- Measuring and comparing compression ratios

After this, you'll understand how gzip, ZIP, and PNG compression work at a low level and can implement your own compressors.

## Quick Start

```bash
cd examples
build_all.bat

# Simplest compression
bin\01_rle.exe

# Optimal prefix-free coding
bin\02_huffman.exe

# Sliding window compression
bin\03_lz77.exe

# Compress real files
bin\04_file_compression.exe
```

---

Compression is everywhere - ZIP, gzip, PNG, MP3, video codecs. Understanding these fundamentals gives you insight into how gigabytes of data fit on small devices and stream over the internet!
