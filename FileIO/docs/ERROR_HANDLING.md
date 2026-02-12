# Error Handling for File I/O

## The Big Picture

File operations fail. Disk full, permissions denied, file doesn't exist, corrupted data. Robust programs handle errors gracefully and clean up resources properly.

## errno and perror

When system calls fail, they set `errno`:

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

FILE* file = fopen("missing.txt", "r");
if (file == NULL) {
    // Method 1: perror (prints to stderr)
    perror("fopen");  // "fopen: No such file or directory"
    
    // Method 2: strerror (get error string)
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    
    // Method 3: Check specific error
    if (errno == ENOENT) {
        fprintf(stderr, "File does not exist\n");
    } else if (errno == EACCES) {
        fprintf(stderr, "Permission denied\n");
    }
    
    return 1;
}
```

Common errno values:
- `ENOENT` - No such file or directory
- `EACCES` - Permission denied
- `EISDIR` - Is a directory
- `ENOSPC` - No space left on device
- `EMFILE` - Too many open files

## Checking All Operations

Don't just check `fopen`:

```c
FILE* file = fopen("data.txt", "w");
if (file == NULL) {
    perror("fopen");
    return -1;
}

// Check write
if (fprintf(file, "data\n") < 0) {
    perror("fprintf");
    fclose(file);
    return -1;
}

// Check close!
if (fclose(file) != 0) {
    perror("fclose");  // Might indicate write failed
    return -1;
}
```

`fclose` can fail! Buffered data might not flush to disk.

## Resource Cleanup Pattern

Always close files, even on error:

```c
int process_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }
    
    char* buffer = malloc(4096);
    if (buffer == NULL) {
        fclose(file);
        return -1;
    }
    
    // Process...
    if (some_error) {
        free(buffer);
        fclose(file);
        return -1;
    }
    
    // More processing...
    if (another_error) {
        free(buffer);
        fclose(file);
        return -1;
    }
    
    free(buffer);
    fclose(file);
    return 0;
}
```

**Problem:** Lots of duplication!

## Goto Cleanup Pattern

Better approach:

```c
int process_file(const char* filename) {
    FILE* file = NULL;
    char* buffer = NULL;
    int result = -1;
    
    file = fopen(filename, "r");
    if (file == NULL) {
        goto cleanup;
    }
    
    buffer = malloc(4096);
    if (buffer == NULL) {
        goto cleanup;
    }
    
    // Process...
    if (some_error) {
        goto cleanup;
    }
    
    // More processing...
    if (another_error) {
        goto cleanup;
    }
    
    result = 0;  // Success!
    
cleanup:
    if (buffer != NULL) {
        free(buffer);
    }
    if (file != NULL) {
        fclose(file);
    }
    return result;
}
```

Clean, single cleanup path. This is standard C practice.

## Multiple Files

```c
int process_files(const char* input, const char* output) {
    FILE* in = NULL;
    FILE* out = NULL;
    char* buffer = NULL;
    int result = -1;
    
    in = fopen(input, "r");
    if (in == NULL) {
        fprintf(stderr, "Failed to open input: %s\n", strerror(errno));
        goto cleanup;
    }
    
    out = fopen(output, "w");
    if (out == NULL) {
        fprintf(stderr, "Failed to open output: %s\n", strerror(errno));
        goto cleanup;
    }
    
    buffer = malloc(4096);
    if (buffer == NULL) {
        fprintf(stderr, "Out of memory\n");
        goto cleanup;
    }
    
    // Process...
    
    result = 0;
    
cleanup:
    if (buffer != NULL) free(buffer);
    if (out != NULL) fclose(out);
    if (in != NULL) fclose(in);
    return result;
}
```

## Error Reporting Strategies

### 1. Return Codes

```c
// 0 = success, -1 = error (check errno)
int save_data(const char* filename, Data* data);

// Usage
if (save_data("output.txt", &data) != 0) {
    fprintf(stderr, "Save failed: %s\n", strerror(errno));
}
```

### 2. Return Pointers (NULL = error)

```c
Data* load_data(const char* filename);  // NULL on error

// Usage
Data* data = load_data("input.txt");
if (data == NULL) {
    fprintf(stderr, "Load failed: %s\n", strerror(errno));
}
```

### 3. Out Parameters

```c
int load_data(const char* filename, Data** out);  // 0 = success

// Usage
Data* data;
if (load_data("input.txt", &data) != 0) {
    fprintf(stderr, "Load failed\n");
}
```

### 4. Error Codes

```c
typedef enum {
    ERR_OK = 0,
    ERR_FILE_NOT_FOUND,
    ERR_PERMISSION_DENIED,
    ERR_OUT_OF_MEMORY,
    ERR_INVALID_FORMAT
} ErrorCode;

ErrorCode load_data(const char* filename, Data** out);

// Usage
Data* data;
ErrorCode err = load_data("input.txt", &data);
if (err != ERR_OK) {
    fprintf(stderr, "Load failed: %s\n", error_string(err));
}
```

## Validating File Contents

```c
int validate_and_load(const char* filename, Data* data) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }
    
    // Check magic number
    uint32_t magic;
    if (fread(&magic, sizeof(magic), 1, file) != 1) {
        fprintf(stderr, "Failed to read magic number\n");
        fclose(file);
        return -1;
    }
    
    if (magic != 0x44415441) {  // "DATA"
        fprintf(stderr, "Invalid file format\n");
        fclose(file);
        return -1;
    }
    
    // Check version
    uint16_t version;
    if (fread(&version, sizeof(version), 1, file) != 1) {
        fprintf(stderr, "Failed to read version\n");
        fclose(file);
        return -1;
    }
    
    if (version > 1) {
        fprintf(stderr, "Unsupported version: %d\n", version);
        fclose(file);
        return -1;
    }
    
    // Read data...
    
    fclose(file);
    return 0;
}
```

## Atomic File Writes

Don't corrupt existing file on error:

```c
int save_data_atomic(const char* filename, Data* data) {
    // Write to temporary file first
    char temp[PATH_MAX];
    snprintf(temp, sizeof(temp), "%s.tmp", filename);
    
    FILE* file = fopen(temp, "wb");
    if (file == NULL) {
        return -1;
    }
    
    // Write data
    if (write_data(file, data) != 0) {
        fclose(file);
        remove(temp);  // Clean up temp file
        return -1;
    }
    
    if (fclose(file) != 0) {
        remove(temp);
        return -1;
    }
    
    // Atomic rename (replaces old file)
    if (rename(temp, filename) != 0) {
        perror("rename");
        remove(temp);
        return -1;
    }
    
    return 0;
}
```

On Unix, `rename()` is atomic. On Windows, use `ReplaceFile()` or `MoveFileEx()`.

## Checking Disk Space

Before writing large files:

```c
#ifdef _WIN32
#include <windows.h>

int check_disk_space(const char* path, size_t required) {
    ULARGE_INTEGER free_bytes;
    
    if (GetDiskFreeSpaceEx(path, &free_bytes, NULL, NULL)) {
        return free_bytes.QuadPart >= required;
    }
    
    return 0;
}
#else
#include <sys/statvfs.h>

int check_disk_space(const char* path, size_t required) {
    struct statvfs stat;
    
    if (statvfs(path, &stat) == 0) {
        uint64_t available = stat.f_bavail * stat.f_frsize;
        return available >= required;
    }
    
    return 0;
}
#endif
```

## Recovering from Partial Writes

```c
int robust_write(FILE* file, const void* data, size_t size) {
    const char* ptr = data;
    size_t remaining = size;
    
    while (remaining > 0) {
        size_t written = fwrite(ptr, 1, remaining, file);
        
        if (written == 0) {
            if (ferror(file)) {
                return -1;  // Error
            }
            // Else: EOF or full buffer, try again
        }
        
        ptr += written;
        remaining -= written;
    }
    
    return 0;
}
```

## Logging Errors

```c
void log_error(const char* format, ...) {
    FILE* log = fopen("error.log", "a");
    if (log == NULL) {
        // Can't log to file, use stderr
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        return;
    }
    
    // Timestamp
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", 
             localtime(&now));
    
    fprintf(log, "[%s] ", timestamp);
    
    // Error message
    va_list args;
    va_start(args, format);
    vfprintf(log, format, args);
    va_end(args);
    
    fprintf(log, "\n");
    
    fclose(log);
}

// Usage
if (fopen("data.txt", "r") == NULL) {
    log_error("Failed to open data.txt: %s", strerror(errno));
}
```

## Summary

File I/O error handling checklist:
- ✓ Check `fopen()` return value
- ✓ Check `fread()`/`fwrite()` return values
- ✓ Check `fclose()` return value (buffered writes!)
- ✓ Use `perror()` or `strerror(errno)` for messages
- ✓ Use goto cleanup pattern for resource management
- ✓ Validate file format (magic numbers, versions)
- ✓ Write to temp file, then rename (atomic updates)
- ✓ Check disk space before large writes
- ✓ Clean up temp files on error
- ✓ Log errors for debugging

Good error handling makes the difference between a toy program and production code.
