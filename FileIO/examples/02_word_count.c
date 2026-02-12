/*
 * Word Count Example
 * 
 * Classic text processing: count lines, words, and characters.
 * Like the Unix `wc` command.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int lines;
    int words;
    int chars;
} Stats;

// Check if character is word separator
int is_separator(int c) {
    return isspace(c) || c == '\n' || c == '\t' || c == '\r';
}

// Count stats in a file
Stats count_file(const char* filename) {
    Stats stats = {0};
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return stats;
    }
    
    int c;
    int in_word = 0;
    
    while ((c = fgetc(file)) != EOF) {
        stats.chars++;
        
        if (c == '\n') {
            stats.lines++;
        }
        
        if (is_separator(c)) {
            in_word = 0;
        } else {
            if (!in_word) {
                stats.words++;
                in_word = 1;
            }
        }
    }
    
    // If file doesn't end with newline, count last line
    if (stats.chars > 0 && c != '\n') {
        stats.lines++;
    }
    
    fclose(file);
    return stats;
}

// Find most common words
typedef struct {
    char word[64];
    int count;
} WordCount;

void analyze_words(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    
    // Array to store unique words
    WordCount words[1000];
    int word_count = 0;
    
    // Read file word by word
    char word[64];
    while (fscanf(file, "%63s", word) == 1) {
        // Convert to lowercase for comparison
        for (char* p = word; *p; p++) {
            *p = tolower(*p);
        }
        
        // Remove punctuation at end
        int len = strlen(word);
        while (len > 0 && !isalnum(word[len-1])) {
            word[--len] = '\0';
        }
        
        if (len == 0) continue;
        
        // Find word in array
        int found = 0;
        for (int i = 0; i < word_count; i++) {
            if (strcmp(words[i].word, word) == 0) {
                words[i].count++;
                found = 1;
                break;
            }
        }
        
        // Add new word
        if (!found && word_count < 1000) {
            strcpy(words[word_count].word, word);
            words[word_count].count = 1;
            word_count++;
        }
    }
    
    fclose(file);
    
    // Sort by count (bubble sort for simplicity)
    for (int i = 0; i < word_count - 1; i++) {
        for (int j = 0; j < word_count - i - 1; j++) {
            if (words[j].count < words[j+1].count) {
                WordCount temp = words[j];
                words[j] = words[j+1];
                words[j+1] = temp;
            }
        }
    }
    
    // Print top 10 words
    printf("\nTop 10 most common words:\n");
    printf("%-20s %s\n", "Word", "Count");
    printf("%-20s %s\n", "----", "-----");
    
    int top = word_count < 10 ? word_count : 10;
    for (int i = 0; i < top; i++) {
        printf("%-20s %d\n", words[i].word, words[i].count);
    }
}

// Find longest line
void find_longest_line(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    
    char line[4096];
    char longest[4096] = {0};
    int longest_len = 0;
    int longest_line_num = 0;
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        line_num++;
        
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        int len = strlen(line);
        if (len > longest_len) {
            longest_len = len;
            longest_line_num = line_num;
            strcpy(longest, line);
        }
    }
    
    fclose(file);
    
    printf("\nLongest line:\n");
    printf("  Line %d (%d characters)\n", longest_line_num, longest_len);
    printf("  \"%s\"\n", longest);
}

// Create sample text file
void create_sample_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        return;
    }
    
    fprintf(file, "The quick brown fox jumps over the lazy dog.\n");
    fprintf(file, "This is a sample text file for word counting.\n");
    fprintf(file, "It contains several lines of text.\n");
    fprintf(file, "Some words appear multiple times in the text.\n");
    fprintf(file, "The word count program will analyze this file.\n");
    fprintf(file, "It will count lines, words, and characters.\n");
    fprintf(file, "This example demonstrates text processing in C.\n");
    fprintf(file, "File I/O operations are fundamental to programming.\n");
    fprintf(file, "Reading and writing files is a common task.\n");
    fprintf(file, "The C standard library provides powerful file functions.\n");
    
    fclose(file);
    printf("Created sample file: %s\n\n", filename);
}

int main(void) {
    printf("=== Word Count Example ===\n\n");
    
    const char* filename = "sample.txt";
    
    // Create sample file
    create_sample_file(filename);
    
    // Count stats
    printf("Analyzing file...\n");
    Stats stats = count_file(filename);
    
    printf("\nFile: %s\n", filename);
    printf("  Lines:      %d\n", stats.lines);
    printf("  Words:      %d\n", stats.words);
    printf("  Characters: %d\n", stats.chars);
    
    // Word frequency analysis
    analyze_words(filename);
    
    // Find longest line
    find_longest_line(filename);
    
    printf("\n=== Try it yourself ===\n");
    printf("Create your own text file and run this program on it!\n");
    printf("You can modify the code to:\n");
    printf("  - Count specific words\n");
    printf("  - Find average word length\n");
    printf("  - Count sentences\n");
    printf("  - Remove duplicate lines\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
