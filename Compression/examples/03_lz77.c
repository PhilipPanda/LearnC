/*
 * LZ77 Compression
 *
 * Learn sliding window compression:
 * - Search backward through a window for matching sequences
 * - Encode matches as (offset, length, next) triples
 * - Decode by copying from the already-written output buffer
 * - Understand the algorithm behind gzip and zip
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_SIZE  256   // How far back to search for matches
#define MAX_MATCH     64   // Maximum match length

// ===== LZ77 Token =====

typedef struct {
    int offset;          // How many bytes back the match starts (0 = no match)
    int length;          // How many bytes to copy from the window
    unsigned char next;  // The literal byte that follows the match
} LZ77Token;

// ===== LZ77 Compression =====

// Encode input into LZ77 tokens
// Each token: copy `length` bytes from position (current - offset), then emit `next`
// Returns number of tokens written
int lz77_encode(const unsigned char* input, int input_len, LZ77Token* tokens) {
    int pos = 0;
    int token_count = 0;

    while (pos < input_len) {
        int best_offset = 0;
        int best_length = 0;

        // Search the sliding window for the longest match
        int window_start = pos - WINDOW_SIZE;
        if (window_start < 0) window_start = 0;

        for (int search = window_start; search < pos; search++) {
            int length = 0;

            // Count matching bytes - always leave room for one trailing literal
            while (length < MAX_MATCH &&
                   pos + length < input_len - 1 &&
                   input[search + length] == input[pos + length]) {
                length++;
            }

            if (length > best_length) {
                best_length = length;
                best_offset = pos - search;
            }
        }

        // Emit token: back-reference (possibly zero length) + literal
        LZ77Token token;
        token.offset = best_offset;
        token.length = best_length;
        token.next   = input[pos + best_length];  // literal byte after the match
        tokens[token_count++] = token;
        pos += best_length + 1;
    }

    return token_count;
}

// Decode LZ77 tokens back to the original data
// Returns number of decoded bytes
int lz77_decode(const LZ77Token* tokens, int token_count, unsigned char* output) {
    int pos = 0;

    for (int i = 0; i < token_count; i++) {
        // Copy back-reference from the already-written output
        if (tokens[i].length > 0) {
            int copy_from = pos - tokens[i].offset;
            for (int j = 0; j < tokens[i].length; j++) {
                output[pos++] = output[copy_from + j];
            }
        }
        // Always emit the trailing literal
        output[pos++] = tokens[i].next;
    }

    return pos;
}

// ===== Helper Functions =====

void print_tokens(const LZ77Token* tokens, int count) {
    for (int i = 0; i < count; i++) {
        char ch = tokens[i].next >= 32 && tokens[i].next < 127
                  ? (char)tokens[i].next : '.';
        if (tokens[i].length == 0) {
            printf("  [literal '%c']\n", ch);
        } else {
            printf("  [back=%d, copy=%d, next='%c']\n",
                   tokens[i].offset, tokens[i].length, ch);
        }
    }
}

// Estimate compressed bytes: 2 bytes per token (offset + length packed) + 1 literal
int estimate_compressed_size(int token_count) {
    return token_count * 3;  // offset(1) + length(1) + literal(1)
}

// ===== Demo Functions =====

void demo_basic_lz77() {
    printf("--- Basic LZ77 Demo ---\n\n");

    const char* input = "abcabcabc";
    int input_len = (int)strlen(input);

    LZ77Token tokens[512];
    unsigned char decoded[512];

    printf("Input: \"%s\" (%d bytes)\n\n", input, input_len);

    int token_count = lz77_encode((unsigned char*)input, input_len, tokens);

    printf("Tokens (%d total):\n", token_count);
    print_tokens(tokens, token_count);
    printf("\n");

    int decoded_len = lz77_decode(tokens, token_count, decoded);
    decoded[decoded_len] = '\0';

    printf("Decoded: \"%s\"\n", decoded);
    printf("Match:   %s\n\n", strcmp(input, (char*)decoded) == 0 ? "YES" : "NO");
}

void demo_repetitive_data() {
    printf("--- Repetitive Data ---\n\n");

    const char* input = "ABCABCABCABCABC";
    int input_len = (int)strlen(input);

    LZ77Token tokens[512];
    unsigned char decoded[512];

    printf("Input:  \"%s\" (%d bytes)\n\n", input, input_len);

    int token_count = lz77_encode((unsigned char*)input, input_len, tokens);
    int compressed = estimate_compressed_size(token_count);

    printf("Tokens (%d total, ~%d bytes compressed):\n", token_count, compressed);
    print_tokens(tokens, token_count);

    int decoded_len = lz77_decode(tokens, token_count, decoded);
    decoded[decoded_len] = '\0';

    printf("\nDecoded: \"%s\"\n", decoded);
    printf("Match:   %s\n", strcmp(input, (char*)decoded) == 0 ? "YES" : "NO");

    float ratio = (float)compressed / input_len * 100.0f;
    printf("Ratio:   %.1f%% of original\n\n", ratio);
}

void demo_sliding_window() {
    printf("--- Sliding Window in Action ---\n\n");
    printf("LZ77 finds repeated phrases and replaces them with back-references.\n\n");

    const char* input = "the cat sat on the mat";
    int input_len = (int)strlen(input);

    LZ77Token tokens[512];
    unsigned char decoded[512];

    printf("Input:  \"%s\" (%d bytes)\n\n", input, input_len);

    int token_count = lz77_encode((unsigned char*)input, input_len, tokens);
    int compressed = estimate_compressed_size(token_count);

    printf("Tokens (%d total):\n", token_count);
    print_tokens(tokens, token_count);

    int decoded_len = lz77_decode(tokens, token_count, decoded);
    decoded[decoded_len] = '\0';

    printf("\nDecoded: \"%s\"\n", decoded);
    printf("Match:   %s\n", strcmp(input, (char*)decoded) == 0 ? "YES" : "NO");

    float ratio = (float)compressed / input_len * 100.0f;
    printf("Ratio:   %.1f%% of original (~%d bytes)\n\n", ratio, compressed);
}

void demo_compression_comparison() {
    printf("--- Compression Comparison ---\n\n");

    struct {
        const char* name;
        const char* data;
    } test_cases[] = {
        { "No repetition", "aZbYcXdWeVfUgThSiRjQ" },
        { "Short repeat",  "abababababababababab" },
        { "Long repeat",   "aaaaaaaaaaaaaaaaaaaaaa" },
        { "Text phrase",   "the the the the the the" },
        { "Mixed",         "hello world hello world" },
    };

    int n = (int)(sizeof(test_cases) / sizeof(test_cases[0]));

    printf("%-16s  %6s  %8s  %8s\n", "Type", "Input", "LZ77", "Ratio");
    printf("%-16s  %6s  %8s  %8s\n", "----", "-----", "----", "-----");

    for (int i = 0; i < n; i++) {
        const char* data = test_cases[i].data;
        int input_len = (int)strlen(data);
        LZ77Token tokens[512];

        int token_count = lz77_encode((unsigned char*)data, input_len, tokens);
        int compressed = estimate_compressed_size(token_count);
        float ratio = (float)compressed / input_len * 100.0f;

        printf("%-16s  %6d  %8d  %7.1f%%\n",
               test_cases[i].name, input_len, compressed, ratio);
    }
    printf("\n");
}

int main(void) {
    printf("=== LZ77 Compression ===\n\n");
    printf("Sliding window: find repeated sequences and replace with back-references.\n\n");

    demo_basic_lz77();
    demo_repetitive_data();
    demo_sliding_window();
    demo_compression_comparison();

    printf("=== Summary ===\n");
    printf("LZ77 demonstrated:\n");
    printf("  1. Sliding window: look back up to %d bytes for matches\n", WINDOW_SIZE);
    printf("  2. Find the longest match in the window\n");
    printf("  3. Emit token: (offset, match_length, next_literal)\n");
    printf("  4. Decode: copy from already-written output at offset\n");
    printf("  5. Works best on repetitive or structured data\n");
    printf("\nReal-world descendants:\n");
    printf("  - LZSS: skip token if match saves no space\n");
    printf("  - DEFLATE: LZ77 + Huffman = ZIP, gzip, PNG, zlib\n");
    printf("  - LZ4: tuned for speed over ratio\n");
    printf("  - LZW: used in GIF and old Unix compress\n");
    printf("  - Brotli (Google), Zstandard (Facebook): modern variants\n");

    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
