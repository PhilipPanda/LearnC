/*
 * File Compression
 *
 * Put it all together - compress and decompress real files:
 * - Custom binary format with a magic-number header
 * - RLE compression of arbitrary binary data
 * - Integrity verification after roundtrip
 * - Timing and ratio measurements
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ===== Compressed File Format =====

#define MAGIC_NUMBER  0x434F4D50u  // "COMP" in ASCII
#define FORMAT_VERSION 1u

// Header written at the start of every compressed file
typedef struct {
    unsigned int magic;            // Magic number to identify the format
    unsigned int version;          // Format version for future compatibility
    unsigned int original_size;    // Uncompressed size in bytes
    unsigned int compressed_size;  // Compressed payload size in bytes
} CompressedHeader;

// ===== RLE Compression =====

int rle_encode(const unsigned char* input, int input_len, unsigned char* output) {
    int out_pos = 0;
    int i = 0;

    while (i < input_len) {
        unsigned char current = input[i];
        int count = 1;

        while (i + count < input_len &&
               input[i + count] == current &&
               count < 255) {
            count++;
        }

        output[out_pos++] = (unsigned char)count;
        output[out_pos++] = current;
        i += count;
    }

    return out_pos;
}

int rle_decode(const unsigned char* input, int input_len, unsigned char* output) {
    int out_pos = 0;

    for (int i = 0; i + 1 < input_len; i += 2) {
        int count = input[i];
        unsigned char byte = input[i + 1];
        for (int j = 0; j < count; j++) {
            output[out_pos++] = byte;
        }
    }

    return out_pos;
}

// ===== File Utilities =====

unsigned char* read_file(const char* filename, int* out_size) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    *out_size = (int)ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* data = malloc(*out_size);
    if (!data) { fclose(f); return NULL; }

    fread(data, 1, *out_size, f);
    fclose(f);
    return data;
}

int write_file(const char* filename, const unsigned char* data, int size) {
    FILE* f = fopen(filename, "wb");
    if (!f) return -1;
    fwrite(data, 1, size, f);
    fclose(f);
    return 0;
}

// ===== Compress a File =====

int compress_file(const char* input_path, const char* output_path) {
    printf("Compressing: %s -> %s\n", input_path, output_path);

    int original_size = 0;
    unsigned char* data = read_file(input_path, &original_size);
    if (!data) {
        fprintf(stderr, "  Error: cannot read %s\n", input_path);
        return -1;
    }

    printf("  Original size:    %d bytes\n", original_size);

    // Allocate worst-case output (RLE doubles data in the worst case)
    unsigned char* compressed = malloc(original_size * 2 + 16);
    if (!compressed) { free(data); return -1; }

    clock_t start = clock();
    int compressed_size = rle_encode(data, original_size, compressed);
    clock_t end = clock();

    double ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    printf("  Compressed size:  %d bytes\n", compressed_size);
    float ratio = (float)compressed_size / original_size * 100.0f;
    printf("  Ratio:            %.1f%% (%.2fx)\n", ratio, (float)original_size / compressed_size);
    printf("  Time:             %.3f ms\n", ms);

    // Write header + compressed payload
    FILE* out = fopen(output_path, "wb");
    if (!out) {
        fprintf(stderr, "  Error: cannot write %s\n", output_path);
        free(data); free(compressed);
        return -1;
    }

    CompressedHeader header;
    header.magic           = MAGIC_NUMBER;
    header.version         = FORMAT_VERSION;
    header.original_size   = (unsigned int)original_size;
    header.compressed_size = (unsigned int)compressed_size;

    fwrite(&header, sizeof(header), 1, out);
    fwrite(compressed, 1, compressed_size, out);
    fclose(out);

    printf("  Written:          %s (%zu header + %d data)\n\n",
           output_path, sizeof(header), compressed_size);

    free(data);
    free(compressed);
    return 0;
}

// ===== Decompress a File =====

int decompress_file(const char* input_path, const char* output_path) {
    printf("Decompressing: %s -> %s\n", input_path, output_path);

    FILE* f = fopen(input_path, "rb");
    if (!f) {
        fprintf(stderr, "  Error: cannot open %s\n", input_path);
        return -1;
    }

    // Read and validate header
    CompressedHeader header;
    fread(&header, sizeof(header), 1, f);

    if (header.magic != MAGIC_NUMBER) {
        fprintf(stderr, "  Error: not a valid compressed file (bad magic)\n");
        fclose(f);
        return -1;
    }

    printf("  Format version:   %u\n", header.version);
    printf("  Original size:    %u bytes\n", header.original_size);
    printf("  Compressed size:  %u bytes\n", header.compressed_size);

    // Read compressed payload
    unsigned char* compressed = malloc(header.compressed_size);
    if (!compressed) { fclose(f); return -1; }
    fread(compressed, 1, header.compressed_size, f);
    fclose(f);

    // Decompress
    unsigned char* output = malloc(header.original_size + 1);
    if (!output) { free(compressed); return -1; }

    clock_t start = clock();
    int decoded_size = rle_decode(compressed, (int)header.compressed_size, output);
    clock_t end = clock();

    double ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    printf("  Decoded size:     %d bytes\n", decoded_size);
    printf("  Time:             %.3f ms\n", ms);

    if ((unsigned int)decoded_size != header.original_size) {
        fprintf(stderr, "  Warning: size mismatch! Expected %u, got %d\n",
                header.original_size, decoded_size);
    }

    write_file(output_path, output, decoded_size);
    printf("  Written:          %s\n\n", output_path);

    free(compressed);
    free(output);
    return 0;
}

// ===== Integrity Check =====

void verify_roundtrip(const char* original, const char* restored) {
    printf("Verifying: %s == %s\n", original, restored);

    int orig_size = 0, rest_size = 0;
    unsigned char* orig_data = read_file(original, &orig_size);
    unsigned char* rest_data = read_file(restored, &rest_size);

    if (!orig_data || !rest_data) {
        printf("  Could not open files for comparison\n");
        free(orig_data); free(rest_data);
        return;
    }

    printf("  Original size:  %d bytes\n", orig_size);
    printf("  Restored size:  %d bytes\n", rest_size);

    if (orig_size == rest_size && memcmp(orig_data, rest_data, orig_size) == 0) {
        printf("  Integrity:      PASS - files are byte-identical!\n\n");
    } else {
        printf("  Integrity:      FAIL - files differ!\n\n");
    }

    free(orig_data);
    free(rest_data);
}

// ===== Create Test Files =====

void create_test_files() {
    printf("--- Creating Test Files ---\n\n");

    // Highly compressible: lots of repetition
    FILE* f = fopen("test_repetitive.txt", "w");
    for (int i = 0; i < 5; i++) {
        fprintf(f, "AAAAAAAAAA BBBBBBBBBB CCCCCCCCCC DDDDDDDDDD\n");
    }
    fclose(f);
    printf("Created: test_repetitive.txt\n");

    // Normal text: moderate compression
    f = fopen("test_text.txt", "w");
    fprintf(f, "The quick brown fox jumps over the lazy dog.\n");
    fprintf(f, "Pack my box with five dozen liquor jugs.\n");
    fprintf(f, "Compression works by finding and eliminating redundancy.\n");
    fprintf(f, "Repetition in data leads to high compression ratios.\n");
    fclose(f);
    printf("Created: test_text.txt\n");

    // Pseudo-random bytes: poor compression (RLE will expand it)
    f = fopen("test_binary.bin", "wb");
    for (int i = 0; i < 200; i++) {
        // Simple LCG to generate varied bytes
        unsigned char b = (unsigned char)((i * 137 + 53) % 256);
        fwrite(&b, 1, 1, f);
    }
    fclose(f);
    printf("Created: test_binary.bin\n\n");
}

int main(void) {
    printf("=== File Compression ===\n\n");
    printf("Compress and decompress real files using RLE with a custom binary format.\n\n");

    create_test_files();

    // Test 1: Repetitive text
    printf("=== Test 1: Repetitive Text ===\n");
    compress_file("test_repetitive.txt", "test_repetitive.comp");
    decompress_file("test_repetitive.comp", "test_repetitive_out.txt");
    verify_roundtrip("test_repetitive.txt", "test_repetitive_out.txt");

    // Test 2: Normal text
    printf("=== Test 2: Normal Text ===\n");
    compress_file("test_text.txt", "test_text.comp");
    decompress_file("test_text.comp", "test_text_out.txt");
    verify_roundtrip("test_text.txt", "test_text_out.txt");

    // Test 3: Binary data
    printf("=== Test 3: Binary Data ===\n");
    compress_file("test_binary.bin", "test_binary.comp");
    decompress_file("test_binary.comp", "test_binary_out.bin");
    verify_roundtrip("test_binary.bin", "test_binary_out.bin");

    printf("=== Summary ===\n");
    printf("File compression demonstrated:\n");
    printf("  1. Custom header with magic number and original/compressed sizes\n");
    printf("  2. RLE compression of arbitrary binary data\n");
    printf("  3. Decompression guided by the stored original size\n");
    printf("  4. Byte-level integrity verification after roundtrip\n");
    printf("  5. Different ratios for different data types\n");
    printf("\nCreated files:\n");
    printf("  test_repetitive.txt / .comp / _out.txt\n");
    printf("  test_text.txt       / .comp / _out.txt\n");
    printf("  test_binary.bin     / .comp / _out.bin\n");
    printf("\nFor real compression use zlib, libz, or call gzip/zstd.\n");

    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
