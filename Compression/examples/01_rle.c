/*
 * Run-Length Encoding (RLE)
 *
 * Learn the simplest compression algorithm:
 * - Encode repeated bytes as count+value pairs
 * - Decode RLE back to original data
 * - Measure compression ratios
 * - Understand best and worst case scenarios
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===== RLE Encoding =====

// Encode data with run-length encoding
// Output format: [count][byte] pairs
// Returns number of encoded bytes
int rle_encode(const unsigned char* input, int input_len, unsigned char* output) {
    int out_pos = 0;
    int i = 0;

    while (i < input_len) {
        unsigned char current = input[i];
        int count = 1;

        // Count consecutive identical bytes (max 255)
        while (i + count < input_len &&
               input[i + count] == current &&
               count < 255) {
            count++;
        }

        // Write the run: count then the byte value
        output[out_pos++] = (unsigned char)count;
        output[out_pos++] = current;
        i += count;
    }

    return out_pos;
}

// Decode RLE data back to original
// Returns number of decoded bytes
int rle_decode(const unsigned char* input, int input_len, unsigned char* output) {
    int out_pos = 0;

    for (int i = 0; i + 1 < input_len; i += 2) {
        int count = input[i];
        unsigned char byte = input[i + 1];

        // Expand the run
        for (int j = 0; j < count; j++) {
            output[out_pos++] = byte;
        }
    }

    return out_pos;
}

// ===== Helper Functions =====

void print_bytes_readable(const unsigned char* data, int len) {
    for (int i = 0; i < len; i++) {
        printf("%c", data[i] >= 32 && data[i] < 127 ? data[i] : '.');
    }
}

void print_rle_pairs(const unsigned char* data, int len) {
    for (int i = 0; i + 1 < len; i += 2) {
        char ch = data[i + 1] >= 32 && data[i + 1] < 127 ? (char)data[i + 1] : '.';
        printf("  [%3d x '%c'(0x%02x)]\n", data[i], ch, data[i + 1]);
    }
}

// ===== Demo Functions =====

void demo_basic_rle() {
    printf("--- Basic RLE Demo ---\n\n");

    const char* input = "AAABBBCCCCDDDDDEEEEEEE";
    int input_len = (int)strlen(input);

    unsigned char encoded[512];
    unsigned char decoded[512];

    printf("Input:   \"%s\"\n", input);
    printf("Length:  %d bytes\n\n", input_len);

    // Encode
    int encoded_len = rle_encode((unsigned char*)input, input_len, encoded);

    printf("Encoded (%d bytes):\n", encoded_len);
    print_rle_pairs(encoded, encoded_len);
    printf("\n");

    // Decode
    int decoded_len = rle_decode(encoded, encoded_len, decoded);
    decoded[decoded_len] = '\0';

    printf("Decoded: \"%s\"\n", decoded);
    printf("Match:   %s\n", strcmp(input, (char*)decoded) == 0 ? "YES" : "NO");

    float ratio = (float)encoded_len / input_len * 100.0f;
    printf("Ratio:   %.1f%% of original (%.1fx compression)\n\n",
           ratio, (float)input_len / encoded_len);
}

void demo_worst_case() {
    printf("--- Worst Case (No Repetition) ---\n\n");

    const char* input = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int input_len = (int)strlen(input);

    unsigned char encoded[512];

    printf("Input:   \"%s\"\n", input);
    printf("Length:  %d bytes\n\n", input_len);

    int encoded_len = rle_encode((unsigned char*)input, input_len, encoded);

    printf("Encoded: %d bytes\n", encoded_len);
    printf("Note:    RLE EXPANDS data when there is no repetition!\n");

    float ratio = (float)encoded_len / input_len * 100.0f;
    printf("Ratio:   %.1f%% of original (bigger than input!)\n\n", ratio);
}

void demo_pixel_data() {
    printf("--- Simulated Image Row (Pixel Data) ---\n\n");
    printf("RLE is great for images with large solid regions.\n\n");

    // Simulate a row of pixels: background with a few foreground pixels
    unsigned char pixels[] = {
        0, 0, 0, 0, 0, 0, 0, 0,    // 8 black pixels (background)
        255, 255, 255,               // 3 white pixels (object)
        0, 0, 0, 0, 0,              // 5 black pixels
        128, 128, 128, 128,         // 4 gray pixels
        0, 0, 0, 0, 0, 0, 0, 0, 0  // 9 black pixels
    };
    int input_len = (int)sizeof(pixels);

    unsigned char encoded[512];
    unsigned char decoded[512];

    printf("Input pixels (%d bytes): ", input_len);
    for (int i = 0; i < input_len; i++) {
        printf("%3d", pixels[i]);
        if (i + 1 < input_len) printf(",");
    }
    printf("\n\n");

    int encoded_len = rle_encode(pixels, input_len, encoded);
    printf("Encoded (%d bytes):\n", encoded_len);
    print_rle_pairs(encoded, encoded_len);

    int decoded_len = rle_decode(encoded, encoded_len, decoded);
    printf("\nDecoded %d bytes, match: %s\n",
           decoded_len, memcmp(pixels, decoded, input_len) == 0 ? "YES" : "NO");

    float ratio = (float)encoded_len / input_len * 100.0f;
    printf("Ratio:   %.1f%% of original\n\n", ratio);
}

void demo_compression_analysis() {
    printf("--- Compression Ratio Analysis ---\n\n");

    struct {
        const char* name;
        const char* data;
    } test_cases[] = {
        { "All same",     "AAAAAAAAAAAAAAAAAAAAAAAAAAAA" },
        { "Alternating",  "ABABABABABABABABABABABABABABAB" },
        { "Short runs",   "AABBCCDDAABBCCDDAABBCCDD" },
        { "Long runs",    "AAAABBBBBCCCCCCDDDDDDDEEEEEEE" },
        { "Mixed text",   "HHEELLOOO   WWOORRLLDD" },
    };

    int n = (int)(sizeof(test_cases) / sizeof(test_cases[0]));

    printf("%-14s  %6s  %7s  %8s\n", "Type", "Input", "Encoded", "Ratio");
    printf("%-14s  %6s  %7s  %8s\n", "----", "-----", "-------", "-----");

    for (int i = 0; i < n; i++) {
        const char* data = test_cases[i].data;
        int input_len = (int)strlen(data);
        unsigned char encoded[512];

        int encoded_len = rle_encode((unsigned char*)data, input_len, encoded);
        float ratio = (float)encoded_len / input_len * 100.0f;

        printf("%-14s  %6d  %7d  %7.1f%%\n",
               test_cases[i].name, input_len, encoded_len, ratio);
    }
    printf("\n");
}

int main(void) {
    printf("=== Run-Length Encoding (RLE) ===\n\n");
    printf("The simplest compression: encode repeated bytes as count+value pairs.\n\n");

    demo_basic_rle();
    demo_worst_case();
    demo_pixel_data();
    demo_compression_analysis();

    printf("=== Summary ===\n");
    printf("RLE demonstrated:\n");
    printf("  1. Encode: count consecutive identical bytes, write count then byte\n");
    printf("  2. Decode: read count+byte pairs, expand back to original\n");
    printf("  3. Best case: lots of repetition (images, sparse data)\n");
    printf("  4. Worst case: no repetition (data doubles in size!)\n");
    printf("  5. O(n) time for both encode and decode\n");
    printf("\nReal-world uses:\n");
    printf("  - PCX image format\n");
    printf("  - BMP run-length variant\n");
    printf("  - Fax compression (CCITT)\n");
    printf("  - Windows BMP (4-bit and 8-bit variants)\n");
    printf("  - Simple bitmap compression\n");

    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
