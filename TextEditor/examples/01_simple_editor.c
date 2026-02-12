/*
 * Simple Text Editor
 * 
 * A minimal but functional text editor with:
 * - Text buffer (array of lines)
 * - Cursor movement (arrow keys)
 * - Character insert/delete
 * - Line operations
 * - File load/save
 * 
 * Controls:
 *   Arrow keys  - Move cursor
 *   Backspace   - Delete character
 *   Enter       - New line
 *   Ctrl+S      - Save
 *   Ctrl+Q      - Quit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    
    #define KEY_UP 72
    #define KEY_DOWN 80
    #define KEY_LEFT 75
    #define KEY_RIGHT 77
    #define KEY_HOME 71
    #define KEY_END 79
    #define CTRL_S 19
    #define CTRL_Q 17
    
    void clear_screen() {
        system("cls");
    }
    
    void move_cursor(int row, int col) {
        COORD pos = {col, row};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
#else
    #include <unistd.h>
    #include <termios.h>
    
    #define KEY_UP 1000
    #define KEY_DOWN 1001
    #define KEY_LEFT 1002
    #define KEY_RIGHT 1003
    #define KEY_HOME 1004
    #define KEY_END 1005
    #define CTRL_S 19
    #define CTRL_Q 17
    
    struct termios orig_termios;
    
    void enable_raw_mode() {
        tcgetattr(STDIN_FILENO, &orig_termios);
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }
    
    void disable_raw_mode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }
    
    void clear_screen() {
        printf("\x1b[2J\x1b[H");
    }
    
    void move_cursor(int row, int col) {
        printf("\x1b[%d;%dH", row + 1, col + 1);
    }
#endif

// Text buffer
typedef struct {
    char** lines;
    int line_count;
    int capacity;
    int modified;
} TextBuffer;

// Cursor position
typedef struct {
    int row;
    int col;
} Cursor;

// Initialize buffer
void buffer_init(TextBuffer* buf) {
    buf->lines = NULL;
    buf->line_count = 0;
    buf->capacity = 0;
    buf->modified = 0;
    
    // Start with one empty line
    buf->lines = malloc(sizeof(char*));
    buf->lines[0] = strdup("");
    buf->line_count = 1;
    buf->capacity = 1;
}

// Insert line
void buffer_insert_line(TextBuffer* buf, int index, const char* text) {
    if (buf->line_count >= buf->capacity) {
        buf->capacity = buf->capacity * 2;
        buf->lines = realloc(buf->lines, buf->capacity * sizeof(char*));
    }
    
    for (int i = buf->line_count; i > index; i--) {
        buf->lines[i] = buf->lines[i - 1];
    }
    
    buf->lines[index] = strdup(text);
    buf->line_count++;
    buf->modified = 1;
}

// Insert character
void buffer_insert_char(TextBuffer* buf, int row, int col, char c) {
    char* line = buf->lines[row];
    int len = strlen(line);
    
    char* new_line = malloc(len + 2);
    memcpy(new_line, line, col);
    new_line[col] = c;
    memcpy(new_line + col + 1, line + col, len - col + 1);
    
    free(line);
    buf->lines[row] = new_line;
    buf->modified = 1;
}

// Delete character
void buffer_delete_char(TextBuffer* buf, int row, int col) {
    char* line = buf->lines[row];
    int len = strlen(line);
    
    if (col >= len) return;
    
    memmove(line + col, line + col + 1, len - col);
    buf->modified = 1;
}

// Split line at cursor (Enter key)
void buffer_split_line(TextBuffer* buf, int row, int col) {
    char* line = buf->lines[row];
    char* right = strdup(line + col);
    
    line[col] = '\0';
    buf->lines[row] = realloc(line, col + 1);
    
    buffer_insert_line(buf, row + 1, right);
    free(right);
}

// Join line with next (Backspace at line start)
void buffer_join_lines(TextBuffer* buf, int row) {
    if (row >= buf->line_count - 1) return;
    
    char* line1 = buf->lines[row];
    char* line2 = buf->lines[row + 1];
    
    int len1 = strlen(line1);
    int len2 = strlen(line2);
    
    line1 = realloc(line1, len1 + len2 + 1);
    strcpy(line1 + len1, line2);
    buf->lines[row] = line1;
    
    free(buf->lines[row + 1]);
    
    for (int i = row + 1; i < buf->line_count - 1; i++) {
        buf->lines[i] = buf->lines[i + 1];
    }
    
    buf->line_count--;
    buf->modified = 1;
}

// Load file
int buffer_load(TextBuffer* buf, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    
    // Clear buffer
    for (int i = 0; i < buf->line_count; i++) {
        free(buf->lines[i]);
    }
    buf->line_count = 0;
    
    // Read lines
    char line[4096];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        if (buf->line_count >= buf->capacity) {
            buf->capacity = buf->capacity == 0 ? 8 : buf->capacity * 2;
            buf->lines = realloc(buf->lines, buf->capacity * sizeof(char*));
        }
        
        buf->lines[buf->line_count++] = strdup(line);
    }
    
    fclose(file);
    
    // Ensure at least one line
    if (buf->line_count == 0) {
        buf->lines[0] = strdup("");
        buf->line_count = 1;
    }
    
    buf->modified = 0;
    return 0;
}

// Save file
int buffer_save(TextBuffer* buf, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return -1;
    }
    
    for (int i = 0; i < buf->line_count; i++) {
        fprintf(file, "%s\n", buf->lines[i]);
    }
    
    fclose(file);
    buf->modified = 0;
    return 0;
}

// Read key with extended key support
int read_key() {
    #ifdef _WIN32
        int ch = _getch();
        if (ch == 0 || ch == 224) {
            return _getch();  // Extended key
        }
        return ch;
    #else
        char c;
        read(STDIN_FILENO, &c, 1);
        
        if (c == '\x1b') {
            char seq[3];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) return c;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return c;
            
            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': return KEY_UP;
                    case 'B': return KEY_DOWN;
                    case 'C': return KEY_RIGHT;
                    case 'D': return KEY_LEFT;
                    case 'H': return KEY_HOME;
                    case 'F': return KEY_END;
                }
            }
        }
        
        return c;
    #endif
}

// Render editor
void editor_render(TextBuffer* buf, Cursor* cursor, const char* filename) {
    clear_screen();
    
    // Render lines
    for (int i = 0; i < buf->line_count; i++) {
        printf("%s\n", buf->lines[i]);
    }
    
    // Status bar
    move_cursor(24, 0);
    printf("--- %s %s | Row: %d Col: %d | Ctrl+S: Save  Ctrl+Q: Quit ---",
           filename, buf->modified ? "[Modified]" : "",
           cursor->row + 1, cursor->col + 1);
    
    // Position cursor
    move_cursor(cursor->row, cursor->col);
    fflush(stdout);
}

int main(int argc, char** argv) {
    const char* filename;
    
    if (argc < 2) {
        // Default filename if none provided
        filename = "untitled.txt";
        printf("No filename provided. Using: %s\n", filename);
        printf("Press Enter to continue...\n");
        getchar();
    } else {
        filename = argv[1];
    }
    
    #ifndef _WIN32
        enable_raw_mode();
    #endif
    
    TextBuffer buf;
    buffer_init(&buf);
    
    // Load file if exists
    if (buffer_load(&buf, filename) != 0) {
        printf("Creating new file: %s\n", filename);
    }
    
    Cursor cursor = {0, 0};
    
    editor_render(&buf, &cursor, filename);
    
    int running = 1;
    while (running) {
        int key = read_key();
        
        switch (key) {
            case KEY_UP:
                if (cursor.row > 0) {
                    cursor.row--;
                    int len = strlen(buf.lines[cursor.row]);
                    if (cursor.col > len) cursor.col = len;
                }
                break;
                
            case KEY_DOWN:
                if (cursor.row < buf.line_count - 1) {
                    cursor.row++;
                    int len = strlen(buf.lines[cursor.row]);
                    if (cursor.col > len) cursor.col = len;
                }
                break;
                
            case KEY_LEFT:
                if (cursor.col > 0) {
                    cursor.col--;
                }
                break;
                
            case KEY_RIGHT: {
                int len = strlen(buf.lines[cursor.row]);
                if (cursor.col < len) {
                    cursor.col++;
                }
                break;
            }
            
            case KEY_HOME:
                cursor.col = 0;
                break;
                
            case KEY_END:
                cursor.col = strlen(buf.lines[cursor.row]);
                break;
                
            case '\r':
            case '\n':
                buffer_split_line(&buf, cursor.row, cursor.col);
                cursor.row++;
                cursor.col = 0;
                break;
                
            case 8:   // Backspace
            case 127:
                if (cursor.col > 0) {
                    cursor.col--;
                    buffer_delete_char(&buf, cursor.row, cursor.col);
                } else if (cursor.row > 0) {
                    int prev_len = strlen(buf.lines[cursor.row - 1]);
                    buffer_join_lines(&buf, cursor.row - 1);
                    cursor.row--;
                    cursor.col = prev_len;
                }
                break;
                
            case CTRL_S:
                if (buffer_save(&buf, filename) == 0) {
                    // Show saved message briefly
                }
                break;
                
            case CTRL_Q:
                if (buf.modified) {
                    // Could add "unsaved changes" prompt here
                }
                running = 0;
                break;
                
            default:
                if (isprint(key)) {
                    buffer_insert_char(&buf, cursor.row, cursor.col, key);
                    cursor.col++;
                }
                break;
        }
        
        editor_render(&buf, &cursor, filename);
    }
    
    #ifndef _WIN32
        disable_raw_mode();
    #endif
    
    clear_screen();
    move_cursor(0, 0);
    
    return 0;
}
