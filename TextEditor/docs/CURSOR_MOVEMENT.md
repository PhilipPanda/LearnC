# Cursor & Movement

## The Big Picture

Cursor is the user's position in the text. Handle arrow keys, bounds checking, scrolling, and viewport management.

## Cursor State

```c
typedef struct {
    int row;     // Current line (0-based)
    int col;     // Current column (0-based)
} Cursor;

typedef struct {
    int row_offset;   // Top visible line
    int col_offset;   // Left visible column
    int screen_rows;  // Terminal height
    int screen_cols;  // Terminal width
} Viewport;
```

## Movement Functions

```c
void cursor_move_up(Cursor* cursor, TextBuffer* buf) {
    if (cursor->row > 0) {
        cursor->row--;
        
        // Clamp column to new line length
        int line_len = strlen(buf->lines[cursor->row]);
        if (cursor->col > line_len) {
            cursor->col = line_len;
        }
    }
}

void cursor_move_down(Cursor* cursor, TextBuffer* buf) {
    if (cursor->row < buf->line_count - 1) {
        cursor->row++;
        
        int line_len = strlen(buf->lines[cursor->row]);
        if (cursor->col > line_len) {
            cursor->col = line_len;
        }
    }
}

void cursor_move_left(Cursor* cursor) {
    if (cursor->col > 0) {
        cursor->col--;
    }
}

void cursor_move_right(Cursor* cursor, TextBuffer* buf) {
    int line_len = strlen(buf->lines[cursor->row]);
    if (cursor->col < line_len) {
        cursor->col++;
    }
}

void cursor_move_line_start(Cursor* cursor) {
    cursor->col = 0;
}

void cursor_move_line_end(Cursor* cursor, TextBuffer* buf) {
    cursor->col = strlen(buf->lines[cursor->row]);
}
```

## Scrolling

```c
void viewport_scroll(Viewport* vp, Cursor* cursor) {
    // Scroll up
    if (cursor->row < vp->row_offset) {
        vp->row_offset = cursor->row;
    }
    
    // Scroll down
    if (cursor->row >= vp->row_offset + vp->screen_rows) {
        vp->row_offset = cursor->row - vp->screen_rows + 1;
    }
    
    // Scroll left
    if (cursor->col < vp->col_offset) {
        vp->col_offset = cursor->col;
    }
    
    // Scroll right
    if (cursor->col >= vp->col_offset + vp->screen_cols) {
        vp->col_offset = cursor->col - vp->screen_cols + 1;
    }
}
```

## Reading Arrow Keys

### Windows
```c
#include <conio.h>

int read_key() {
    int ch = _getch();
    
    if (ch == 0 || ch == 224) {  // Extended key
        ch = _getch();
        switch (ch) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            case 71: return KEY_HOME;
            case 79: return KEY_END;
            case 73: return KEY_PAGE_UP;
            case 81: return KEY_PAGE_DOWN;
            case 83: return KEY_DELETE;
        }
    }
    
    return ch;
}
```

### Linux (ANSI Escape Sequences)
```c
#include <unistd.h>
#include <termios.h>

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int read_key() {
    char c;
    read(STDIN_FILENO, &c, 1);
    
    if (c == '\x1b') {  // Escape sequence
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
                case '3': return KEY_DELETE;
                case '5': return KEY_PAGE_UP;
                case '6': return KEY_PAGE_DOWN;
            }
        }
    }
    
    return c;
}
```

## Screen Positioning

```c
// Move cursor on screen (ANSI escape codes)
void screen_move_cursor(int row, int col) {
    printf("\x1b[%d;%dH", row + 1, col + 1);
}

// Clear screen
void screen_clear() {
    printf("\x1b[2J");     // Clear entire screen
    printf("\x1b[H");      // Move cursor to home
}

// Clear line
void screen_clear_line() {
    printf("\x1b[K");
}
```

## Rendering

```c
void editor_render(TextBuffer* buf, Cursor* cursor, Viewport* vp) {
    // Clear screen
    screen_clear();
    
    // Render visible lines
    for (int i = 0; i < vp->screen_rows - 1; i++) {
        int file_row = i + vp->row_offset;
        
        if (file_row < buf->line_count) {
            char* line = buf->lines[file_row];
            int len = strlen(line);
            
            // Render visible part of line
            int start = vp->col_offset;
            int end = start + vp->screen_cols;
            
            for (int j = start; j < end && j < len; j++) {
                putchar(line[j]);
            }
        } else {
            printf("~");  // Empty line marker (like vim)
        }
        
        printf("\r\n");
    }
    
    // Render status bar
    printf("\x1b[7m");  // Invert colors
    printf("Row: %d Col: %d", cursor->row + 1, cursor->col + 1);
    printf("\x1b[m");   // Reset
    
    // Position cursor
    int screen_row = cursor->row - vp->row_offset;
    int screen_col = cursor->col - vp->col_offset;
    screen_move_cursor(screen_row, screen_col);
    
    fflush(stdout);
}
```

## Page Up/Down

```c
void cursor_page_up(Cursor* cursor, Viewport* vp) {
    cursor->row -= vp->screen_rows;
    if (cursor->row < 0) {
        cursor->row = 0;
    }
}

void cursor_page_down(Cursor* cursor, TextBuffer* buf, Viewport* vp) {
    cursor->row += vp->screen_rows;
    if (cursor->row >= buf->line_count) {
        cursor->row = buf->line_count - 1;
    }
}
```

## Word Movement

```c
int is_word_char(char c) {
    return isalnum(c) || c == '_';
}

void cursor_word_forward(Cursor* cursor, TextBuffer* buf) {
    char* line = buf->lines[cursor->row];
    int len = strlen(line);
    
    // Skip current word
    while (cursor->col < len && is_word_char(line[cursor->col])) {
        cursor->col++;
    }
    
    // Skip whitespace
    while (cursor->col < len && !is_word_char(line[cursor->col])) {
        cursor->col++;
    }
}

void cursor_word_backward(Cursor* cursor, TextBuffer* buf) {
    if (cursor->col == 0) return;
    
    char* line = buf->lines[cursor->row];
    cursor->col--;
    
    // Skip whitespace
    while (cursor->col > 0 && !is_word_char(line[cursor->col])) {
        cursor->col--;
    }
    
    // Skip word
    while (cursor->col > 0 && is_word_char(line[cursor->col - 1])) {
        cursor->col--;
    }
}
```

## Summary

Key concepts:
- Cursor position (row, col)
- Viewport (visible region)
- Arrow key handling (platform-specific)
- Scrolling to follow cursor
- Screen rendering with ANSI codes
- Bounds checking (don't go past line end)

Movement types:
- Character (left/right)
- Line (up/down)
- Word (Ctrl+Left/Right)
- Page (Page Up/Down)
- Document (Home/End, Ctrl+Home/End)

Always clamp cursor to valid positions after movement!
