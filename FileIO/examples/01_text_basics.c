/*
 * Text File Basics
 * 
 * Learn the fundamentals:
 * - Opening and closing files
 * - Writing text
 * - Reading text
 * - Error handling
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Write text to file
int write_example(const char* filename) {
    printf("=== Writing to %s ===\n", filename);
    
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }
    
    // Write different ways
    fprintf(file, "Line 1: Using fprintf\n");
    fputs("Line 2: Using fputs\n", file);
    fputc('L', file);
    fputs("ine 3: Using fputc for first char\n", file);
    
    // Write formatted data
    int age = 25;
    double score = 95.5;
    fprintf(file, "Line 4: Age=%d, Score=%.2f\n", age, score);
    
    if (fclose(file) != 0) {
        perror("fclose");
        return -1;
    }
    
    printf("Successfully wrote to file\n\n");
    return 0;
}

// Read entire file
int read_example(const char* filename) {
    printf("=== Reading from %s ===\n", filename);
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }
    
    printf("File contents:\n");
    printf("---\n");
    
    int c;
    while ((c = fgetc(file)) != EOF) {
        putchar(c);
    }
    
    printf("---\n");
    
    fclose(file);
    printf("Successfully read file\n\n");
    return 0;
}

// Read line by line
int read_lines_example(const char* filename) {
    printf("=== Reading line by line ===\n");
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }
    
    char line[256];
    int line_number = 1;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%d: %s", line_number++, line);
    }
    
    printf("\n");
    
    // Check for errors
    if (ferror(file)) {
        fprintf(stderr, "Error reading file\n");
        fclose(file);
        return -1;
    }
    
    fclose(file);
    return 0;
}

// Append to file
int append_example(const char* filename) {
    printf("=== Appending to %s ===\n", filename);
    
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }
    
    fprintf(file, "Line 5: Appended later\n");
    fprintf(file, "Line 6: Also appended\n");
    
    fclose(file);
    printf("Successfully appended to file\n\n");
    return 0;
}

// Get file size
long get_file_size(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    
    // Seek to end
    fseek(file, 0, SEEK_END);
    
    // Get position (= file size)
    long size = ftell(file);
    
    fclose(file);
    return size;
}

// Copy file
int copy_file(const char* src, const char* dst) {
    printf("=== Copying %s to %s ===\n", src, dst);
    
    FILE* in = NULL;
    FILE* out = NULL;
    int result = -1;
    
    in = fopen(src, "rb");  // Binary mode for exact copy
    if (in == NULL) {
        perror("fopen source");
        goto cleanup;
    }
    
    out = fopen(dst, "wb");
    if (out == NULL) {
        perror("fopen destination");
        goto cleanup;
    }
    
    // Copy in chunks
    char buffer[4096];
    size_t n;
    
    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        if (fwrite(buffer, 1, n, out) != n) {
            fprintf(stderr, "Write error\n");
            goto cleanup;
        }
    }
    
    if (ferror(in)) {
        fprintf(stderr, "Read error\n");
        goto cleanup;
    }
    
    result = 0;  // Success
    printf("Successfully copied file\n\n");
    
cleanup:
    if (in != NULL) fclose(in);
    if (out != NULL) fclose(out);
    return result;
}

int main(void) {
    printf("=== Text File Basics Example ===\n\n");
    
    const char* filename = "example.txt";
    const char* copy_name = "example_copy.txt";
    
    // Create and write file
    if (write_example(filename) != 0) {
        return 1;
    }
    
    // Read file
    if (read_example(filename) != 0) {
        return 1;
    }
    
    // Read line by line
    if (read_lines_example(filename) != 0) {
        return 1;
    }
    
    // Append to file
    if (append_example(filename) != 0) {
        return 1;
    }
    
    // Read again to see appended lines
    printf("=== After appending ===\n");
    if (read_lines_example(filename) != 0) {
        return 1;
    }
    
    // Get file size
    long size = get_file_size(filename);
    if (size >= 0) {
        printf("File size: %ld bytes\n\n", size);
    }
    
    // Copy file
    if (copy_file(filename, copy_name) != 0) {
        return 1;
    }
    
    printf("=== Summary ===\n");
    printf("Created files:\n");
    printf("  %s (original)\n", filename);
    printf("  %s (copy)\n", copy_name);
    printf("\nKey functions used:\n");
    printf("  fopen()  - Open file\n");
    printf("  fclose() - Close file\n");
    printf("  fprintf() - Write formatted\n");
    printf("  fputs()  - Write string\n");
    printf("  fgets()  - Read line\n");
    printf("  fread()  - Read bytes\n");
    printf("  fwrite() - Write bytes\n");
    printf("  fseek()  - Move position\n");
    printf("  ftell()  - Get position\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
