# Text Buffers

## The Big Picture

A text editor needs to store text efficiently and support fast insert/delete operations. Different data structures make different trade-offs.

## Simple Array of Lines

Most straightforward approach:

```c
typedef struct {
    char** lines;      // Array of strings
    int line_count;
    int capacity;
} TextBuffer;

void buffer_init(TextBuffer* buf) {
    buf->lines = NULL;
    buf->line_count = 0;
    buf->capacity = 0;
}

void buffer_insert_line(TextBuffer* buf, int index, const char* text) {
    // Grow if needed
    if (buf->line_count >= buf->capacity) {
        buf->capacity = buf->capacity == 0 ? 8 : buf->capacity * 2;
        buf->lines = realloc(buf->lines, buf->capacity * sizeof(char*));
    }
    
    // Shift lines down
    for (int i = buf->line_count; i > index; i--) {
        buf->lines[i] = buf->lines[i - 1];
    }
    
    // Insert new line
    buf->lines[index] = strdup(text);
    buf->line_count++;
}

void buffer_delete_line(TextBuffer* buf, int index) {
    free(buf->lines[index]);
    
    // Shift lines up
    for (int i = index; i < buf->line_count - 1; i++) {
        buf->lines[i] = buf->lines[i + 1];
    }
    
    buf->line_count--;
}
```

**Pros:** Simple, cache-friendly  
**Cons:** Slow line insertion/deletion (must shift all lines)

## Gap Buffer

Single string with a gap for fast insertion:

```c
typedef struct {
    char* buffer;
    int size;          // Total buffer size
    int gap_start;     // Start of gap
    int gap_end;       // End of gap (exclusive)
} GapBuffer;

void gap_init(GapBuffer* gb, int initial_size) {
    gb->buffer = malloc(initial_size);
    gb->size = initial_size;
    gb->gap_start = 0;
    gb->gap_end = initial_size;
}

// Move gap to position
void gap_move_to(GapBuffer* gb, int pos) {
    while (gb->gap_start < pos) {
        // Move gap right
        gb->buffer[gb->gap_start++] = gb->buffer[gb->gap_end++];
    }
    while (gb->gap_start > pos) {
        // Move gap left
        gb->buffer[--gb->gap_end] = gb->buffer[--gb->gap_start];
    }
}

// Insert character at gap
void gap_insert(GapBuffer* gb, char c) {
    if (gb->gap_start == gb->gap_end) {
        // Gap full, grow buffer
        int gap_size = gb->size / 2;
        gb->buffer = realloc(gb->buffer, gb->size + gap_size);
        
        // Move data after gap
        memmove(gb->buffer + gb->gap_end + gap_size,
                gb->buffer + gb->gap_end,
                gb->size - gb->gap_end);
        
        gb->gap_end += gap_size;
        gb->size += gap_size;
    }
    
    gb->buffer[gb->gap_start++] = c;
}

// Delete character before gap
void gap_backspace(GapBuffer* gb) {
    if (gb->gap_start > 0) {
        gb->gap_start--;
    }
}

// Delete character after gap
void gap_delete(GapBuffer* gb) {
    if (gb->gap_end < gb->size) {
        gb->gap_end++;
    }
}
```

**Visualization:**
```
Buffer: [H][e][l][l][o][ ][ ][ ][ ][w][o][r][l][d]
                       ^gap_start  ^gap_end

Insert 't' at gap:
Buffer: [H][e][l][l][o][t][ ][ ][ ][w][o][r][l][d]
                          ^gap_start
```

**Pros:** Fast insertion/deletion at cursor  
**Cons:** Slow to move gap to distant positions

## Rope (Piece Table)

Tree of text fragments:

```c
typedef struct RopeNode {
    struct RopeNode* left;
    struct RopeNode* right;
    char* text;          // NULL for internal nodes
    int length;          // Total length of subtree
    int weight;          // Length of left subtree
} RopeNode;

RopeNode* rope_leaf(const char* text) {
    RopeNode* node = malloc(sizeof(RopeNode));
    node->left = NULL;
    node->right = NULL;
    node->text = strdup(text);
    node->length = strlen(text);
    node->weight = node->length;
    return node;
}

RopeNode* rope_concat(RopeNode* left, RopeNode* right) {
    RopeNode* node = malloc(sizeof(RopeNode));
    node->left = left;
    node->right = right;
    node->text = NULL;
    node->length = left->length + right->length;
    node->weight = left->length;
    return node;
}

// Split rope at position
void rope_split(RopeNode* node, int pos, RopeNode** left, RopeNode** right) {
    // Recursive split implementation...
}

// Insert text at position
RopeNode* rope_insert(RopeNode* rope, int pos, const char* text) {
    RopeNode* left, *right;
    rope_split(rope, pos, &left, &right);
    
    RopeNode* new_node = rope_leaf(text);
    RopeNode* temp = rope_concat(left, new_node);
    return rope_concat(temp, right);
}
```

**Pros:** Fast random insertion/deletion, good for large files  
**Cons:** Complex, cache-unfriendly

## Piece Table

Used by Visual Studio Code and others:

```c
typedef struct {
    int start;
    int length;
    int is_original;  // 0 = add buffer, 1 = original file
} Piece;

typedef struct {
    char* original;      // Original file content
    int original_len;
    char* add_buffer;    // Appended text
    int add_buffer_len;
    Piece* pieces;       // Array of pieces
    int piece_count;
} PieceTable;

char piece_table_char_at(PieceTable* pt, int index) {
    int current = 0;
    
    for (int i = 0; i < pt->piece_count; i++) {
        Piece* p = &pt->pieces[i];
        
        if (index < current + p->length) {
            int offset = index - current;
            
            if (p->is_original) {
                return pt->original[p->start + offset];
            } else {
                return pt->add_buffer[p->start + offset];
            }
        }
        
        current += p->length;
    }
    
    return '\0';
}
```

**Pros:** Efficient undo/redo, preserves original file  
**Cons:** More complex than simple array

## Choosing a Data Structure

**For learning:** Array of lines (simplest)  
**For small files (<1MB):** Gap buffer  
**For large files (>10MB):** Rope or piece table  
**For Vim-style editors:** Gap buffer  
**For VSCode-style editors:** Piece table

## Line Operations

Common operations needed:

```c
// Insert character at position
void line_insert_char(char** line, int pos, char c) {
    int len = strlen(*line);
    *line = realloc(*line, len + 2);
    
    // Shift characters right
    memmove(*line + pos + 1, *line + pos, len - pos + 1);
    
    // Insert character
    (*line)[pos] = c;
}

// Delete character at position
void line_delete_char(char** line, int pos) {
    int len = strlen(*line);
    
    // Shift characters left
    memmove(*line + pos, *line + pos + 1, len - pos);
    
    *line = realloc(*line, len);
}

// Split line at position
char* line_split(char** line, int pos) {
    char* right = strdup(*line + pos);
    
    // Truncate left part
    *line = realloc(*line, pos + 1);
    (*line)[pos] = '\0';
    
    return right;
}

// Join two lines
void line_join(char** line1, const char* line2) {
    int len1 = strlen(*line1);
    int len2 = strlen(line2);
    
    *line1 = realloc(*line1, len1 + len2 + 1);
    strcpy(*line1 + len1, line2);
}
```

## Memory Management

```c
void buffer_free(TextBuffer* buf) {
    for (int i = 0; i < buf->line_count; i++) {
        free(buf->lines[i]);
    }
    free(buf->lines);
    buf->lines = NULL;
    buf->line_count = 0;
}
```

## Performance Considerations

**Insertion at end:**
- Array: O(1) amortized
- Gap buffer: O(n) to move gap, then O(1)
- Rope: O(log n)

**Insertion in middle:**
- Array: O(n) to shift lines
- Gap buffer: O(1) if gap is there
- Rope: O(log n)

**Random access:**
- Array: O(1)
- Gap buffer: O(1)
- Rope: O(log n)

For a text editor, gap buffer offers best trade-offs for typical usage patterns (most edits happen near cursor).

## Summary

Text buffer choices:
- **Array of lines:** Simple, good for learning
- **Gap buffer:** Fast editing at cursor
- **Rope:** Good for large files
- **Piece table:** Efficient undo/redo

Key operations:
- Insert/delete characters
- Insert/delete lines
- Split/join lines
- Get character at position

Most editors use gap buffer or piece table. Start with array of lines for simplicity, upgrade if needed.
