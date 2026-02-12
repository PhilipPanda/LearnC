# Text Files

## The Big Picture

Text files are everywhere - logs, configs, source code, data. Reading and writing them is fundamental to programming.

C provides FILE* streams for file operations. Always check for errors and close files when done.

## Opening Files

```c
#include <stdio.h>

FILE* file = fopen("data.txt", "r");  // Open for reading
if (file == NULL) {
    perror("Failed to open file");
    return 1;
}

// Use file...

fclose(file);  // Always close!
```

### File Modes

| Mode | Description | Creates if missing? | Truncates? |
|------|-------------|---------------------|------------|
| `"r"` | Read only | No (error) | No |
| `"w"` | Write only | Yes | Yes |
| `"a"` | Append | Yes | No |
| `"r+"` | Read and write | No (error) | No |
| `"w+"` | Read and write | Yes | Yes |
| `"a+"` | Read and append | Yes | No |

Add `b` for binary mode: `"rb"`, `"wb"`, etc.

## Writing Text

### Character by character

```c
FILE* file = fopen("output.txt", "w");
if (file == NULL) {
    perror("Failed to open file");
    return 1;
}

fputc('H', file);
fputc('i', file);
fputc('\n', file);

fclose(file);
```

### String at a time

```c
FILE* file = fopen("output.txt", "w");
fputs("Hello, World!\n", file);
fputs("Second line\n", file);
fclose(file);
```

### Formatted output (like printf)

```c
FILE* file = fopen("data.txt", "w");

int age = 25;
char name[] = "Alice";
double score = 95.5;

fprintf(file, "Name: %s\n", name);
fprintf(file, "Age: %d\n", age);
fprintf(file, "Score: %.2f\n", score);

fclose(file);
```

## Reading Text

### Character by character

```c
FILE* file = fopen("input.txt", "r");
if (file == NULL) {
    perror("Failed to open file");
    return 1;
}

int c;
while ((c = fgetc(file)) != EOF) {
    putchar(c);  // Print to stdout
}

fclose(file);
```

**Important:** `fgetc()` returns `int`, not `char`, because it needs to distinguish EOF from valid bytes.

### Line by line

```c
FILE* file = fopen("input.txt", "r");
char line[256];

while (fgets(line, sizeof(line), file) != NULL) {
    printf("Read: %s", line);  // line includes newline
}

fclose(file);
```

`fgets()` reads up to N-1 characters or until newline. Always null-terminates.

### Formatted input (like scanf)

```c
FILE* file = fopen("data.txt", "r");

char name[50];
int age;
double score;

if (fscanf(file, "Name: %49s\n", name) == 1 &&
    fscanf(file, "Age: %d\n", &age) == 1 &&
    fscanf(file, "Score: %lf\n", &score) == 1) {
    printf("Read: %s, %d, %.2f\n", name, age, score);
} else {
    printf("Failed to parse file\n");
}

fclose(file);
```

Always check return value! `fscanf` returns number of items successfully read.

## File Position

```c
FILE* file = fopen("data.txt", "r");

// Get current position
long pos = ftell(file);
printf("Position: %ld\n", pos);

// Seek to position
fseek(file, 0, SEEK_SET);  // Beginning
fseek(file, 0, SEEK_END);  // End
fseek(file, 10, SEEK_CUR); // Current + 10

// Get file size
fseek(file, 0, SEEK_END);
long size = ftell(file);
fseek(file, 0, SEEK_SET);  // Back to start
printf("File size: %ld bytes\n", size);

fclose(file);
```

## Error Handling

```c
FILE* file = fopen("input.txt", "r");
if (file == NULL) {
    perror("fopen");  // Prints error message
    // Or:
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    return 1;
}

// Check for errors during reading
while (fgets(line, sizeof(line), file) != NULL) {
    // Process line
}

if (ferror(file)) {
    fprintf(stderr, "Error reading file\n");
}

if (feof(file)) {
    printf("Reached end of file\n");
}

fclose(file);
```

## Resource Cleanup

Always close files, even on error:

```c
FILE* file = fopen("data.txt", "r");
if (file == NULL) {
    return 1;
}

int error = 0;

// Do work...
if (some_error) {
    error = 1;
    goto cleanup;
}

// More work...
if (another_error) {
    error = 1;
    goto cleanup;
}

cleanup:
    fclose(file);
    return error;
```

Or use a pattern:

```c
FILE* file = NULL;

file = fopen("data.txt", "r");
if (file == NULL) {
    goto cleanup;
}

// Work...

cleanup:
    if (file != NULL) {
        fclose(file);
    }
    return 0;
```

## Reading Entire File

```c
FILE* file = fopen("input.txt", "r");
if (file == NULL) return 1;

// Get size
fseek(file, 0, SEEK_END);
long size = ftell(file);
fseek(file, 0, SEEK_SET);

// Allocate buffer
char* content = malloc(size + 1);
if (content == NULL) {
    fclose(file);
    return 1;
}

// Read entire file
size_t read = fread(content, 1, size, file);
content[read] = '\0';  // Null terminate

printf("Content:\n%s\n", content);

free(content);
fclose(file);
```

## Checking if File Exists

```c
#include <stdio.h>

int file_exists(const char* path) {
    FILE* file = fopen(path, "r");
    if (file != NULL) {
        fclose(file);
        return 1;
    }
    return 0;
}
```

Or use `access()` on Unix or `_access()` on Windows.

## Appending to File

```c
FILE* file = fopen("log.txt", "a");  // Append mode
if (file == NULL) return 1;

fprintf(file, "[%s] Log message\n", get_timestamp());

fclose(file);
```

In append mode, all writes go to end of file, regardless of seeks.

## Temporary Files

```c
// Create temporary file
FILE* temp = tmpfile();
if (temp == NULL) {
    perror("tmpfile");
    return 1;
}

// Use it...
fprintf(temp, "Temporary data\n");
rewind(temp);  // Back to start

char line[256];
fgets(line, sizeof(line), temp);

fclose(temp);  // Automatically deleted
```

Or create with name:

```c
char name[L_tmpnam];
tmpnam(name);  // Generate unique name
printf("Temp file: %s\n", name);

FILE* temp = fopen(name, "w+");
// Use and close...
fclose(temp);
remove(name);  // Delete
```

## Buffering

Files are buffered by default. Control it:

```c
FILE* file = fopen("output.txt", "w");

// No buffering (immediate write)
setvbuf(file, NULL, _IONBF, 0);

// Line buffering (flush on newline)
setvbuf(file, NULL, _IOLBF, 0);

// Full buffering with custom size
char buffer[8192];
setvbuf(file, buffer, _IOFBF, sizeof(buffer));

// Manually flush
fprintf(file, "Data");
fflush(file);  // Write buffer to disk now

fclose(file);
```

## Common Patterns

### Copy file

```c
int copy_file(const char* src, const char* dst) {
    FILE* in = fopen(src, "rb");
    if (in == NULL) return -1;
    
    FILE* out = fopen(dst, "wb");
    if (out == NULL) {
        fclose(in);
        return -1;
    }
    
    char buffer[4096];
    size_t n;
    
    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        if (fwrite(buffer, 1, n, out) != n) {
            fclose(in);
            fclose(out);
            return -1;
        }
    }
    
    fclose(in);
    fclose(out);
    return 0;
}
```

### Count lines

```c
int count_lines(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) return -1;
    
    int count = 0;
    int c;
    
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }
    
    fclose(file);
    return count;
}
```

## Cross-Platform Considerations

**Line endings:**
- Windows: `\r\n` (CRLF)
- Unix/Linux: `\n` (LF)
- Mac (old): `\r` (CR)

**Text mode** (default) handles this automatically. Use **binary mode** (`"rb"`, `"wb"`) when you need exact bytes.

**Path separators:**
- Windows: `\` (backslash)
- Unix/Linux: `/` (forward slash)

Use forward slashes where possible (works on both), or:

```c
#ifdef _WIN32
    #define PATH_SEP "\\"
#else
    #define PATH_SEP "/"
#endif
```

## Summary

- Always check `fopen()` return value
- Use `perror()` or `strerror(errno)` for error messages
- Always `fclose()` - use goto cleanup pattern for errors
- Text mode for text files, binary mode for exact bytes
- `fgets()` for lines, `fscanf()` for formatted, `fread()` for chunks
- Check `ferror()` and `feof()` after reading
- Flush with `fflush()` when needed (logs, interactive)

Text files are simple but powerful. Master these basics and you can process any text data.
