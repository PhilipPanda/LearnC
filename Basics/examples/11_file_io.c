/*
 * 11_file_io.c
 * 
 * Reading and writing files.
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Writing to a file
    printf("=== Writing File ===\n");
    FILE *file = fopen("output.txt", "w");
    if (file == NULL) {
        printf("Failed to open file for writing\n");
        return 1;
    }
    
    fprintf(file, "Hello, File!\n");
    fprintf(file, "This is line 2\n");
    fprintf(file, "Number: %d\n", 42);
    
    fclose(file);
    printf("Wrote to output.txt\n");
    
    // Reading from a file
    printf("\n=== Reading File ===\n");
    file = fopen("output.txt", "r");
    if (file == NULL) {
        printf("Failed to open file for reading\n");
        return 1;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("Read: %s", line);  // fgets includes \n
    }
    
    fclose(file);
    
    // Appending to file
    printf("\n=== Appending ===\n");
    file = fopen("output.txt", "a");
    if (file == NULL) return 1;
    
    fprintf(file, "Appended line\n");
    fclose(file);
    printf("Appended to output.txt\n");
    
    // Binary file I/O
    printf("\n=== Binary Files ===\n");
    int numbers[5] = {10, 20, 30, 40, 50};
    
    file = fopen("data.bin", "wb");
    if (file == NULL) return 1;
    
    fwrite(numbers, sizeof(int), 5, file);
    fclose(file);
    printf("Wrote binary data\n");
    
    // Read it back
    int loaded[5];
    file = fopen("data.bin", "rb");
    if (file == NULL) return 1;
    
    fread(loaded, sizeof(int), 5, file);
    fclose(file);
    
    printf("Read back: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", loaded[i]);
    }
    printf("\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}

/*
 * File operations:
 * 
 * fopen(filename, mode)
 *   Modes:
 *     "r"  - Read (text)
 *     "w"  - Write (text, creates/overwrites)
 *     "a"  - Append (text)
 *     "rb" - Read binary
 *     "wb" - Write binary
 *     "ab" - Append binary
 *   Returns FILE* or NULL on error
 * 
 * Text I/O:
 *   fprintf(file, format, ...)  - Write formatted
 *   fgets(buffer, size, file)   - Read line
 *   fgetc(file)                 - Read single char
 *   fputc(char, file)           - Write single char
 * 
 * Binary I/O:
 *   fwrite(ptr, size, count, file)  - Write bytes
 *   fread(ptr, size, count, file)   - Read bytes
 * 
 * Other:
 *   fclose(file)          - Close file
 *   fseek(file, pos, ref) - Move file position
 *   ftell(file)           - Get current position
 *   feof(file)            - Check if end of file
 * 
 * Always:
 *   - Check if fopen returns NULL
 *   - Close files with fclose
 *   - Check return values of read/write
 * 
 * Text vs Binary:
 *   Text - Human readable, platform-specific line endings
 *   Binary - Exact bytes, no conversion
 * 
 * Try:
 *   - Write and read your own data format
 *   - Count lines in a file
 *   - Copy one file to another
 *   - Read file size using fseek/ftell
 */
