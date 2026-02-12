# Binary Files

## The Big Picture

Text files are human-readable but inefficient. Binary files store data in compact, machine-native formats. Perfect for structured data, game saves, databases, images, and anything performance-critical.

## Binary vs Text

**Text file:**
```
Name: Alice
Age: 25
Score: 95.5
```
~30 bytes, needs parsing

**Binary file:**
```
[Alice\0\0...] [25] [95.5]
```
~20 bytes (struct), direct memory mapping

## Writing Structs

```c
#include <stdio.h>

typedef struct {
    char name[50];
    int age;
    double score;
} Person;

int save_person(const char* filename, Person* p) {
    FILE* file = fopen(filename, "wb");  // Binary write mode
    if (file == NULL) {
        perror("fopen");
        return -1;
    }
    
    // Write entire struct
    size_t written = fwrite(p, sizeof(Person), 1, file);
    
    fclose(file);
    
    return (written == 1) ? 0 : -1;
}
```

## Reading Structs

```c
int load_person(const char* filename, Person* p) {
    FILE* file = fopen(filename, "rb");  // Binary read mode
    if (file == NULL) {
        perror("fopen");
        return -1;
    }
    
    size_t read = fread(p, sizeof(Person), 1, file);
    
    fclose(file);
    
    return (read == 1) ? 0 : -1;
}
```

## Writing Arrays

```c
int save_high_scores(const char* filename, int* scores, int count) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) return -1;
    
    // Write count first
    fwrite(&count, sizeof(int), 1, file);
    
    // Write array
    fwrite(scores, sizeof(int), count, file);
    
    fclose(file);
    return 0;
}

int load_high_scores(const char* filename, int** scores, int* count) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return -1;
    
    // Read count
    fread(count, sizeof(int), 1, file);
    
    // Allocate array
    *scores = malloc(*count * sizeof(int));
    
    // Read array
    fread(*scores, sizeof(int), *count, file);
    
    fclose(file);
    return 0;
}
```

## Serialization

Writing complex structures:

```c
typedef struct {
    int id;
    char* name;  // Pointer!
    int* data;   // Pointer!
    int data_count;
} Record;

int save_record(FILE* file, Record* r) {
    // Write fixed-size fields
    fwrite(&r->id, sizeof(int), 1, file);
    fwrite(&r->data_count, sizeof(int), 1, file);
    
    // Write string (length + data)
    int name_len = strlen(r->name);
    fwrite(&name_len, sizeof(int), 1, file);
    fwrite(r->name, 1, name_len, file);
    
    // Write array
    fwrite(r->data, sizeof(int), r->data_count, file);
    
    return 0;
}

int load_record(FILE* file, Record* r) {
    // Read fixed-size fields
    fread(&r->id, sizeof(int), 1, file);
    fread(&r->data_count, sizeof(int), 1, file);
    
    // Read string
    int name_len;
    fread(&name_len, sizeof(int), 1, file);
    r->name = malloc(name_len + 1);
    fread(r->name, 1, name_len, file);
    r->name[name_len] = '\0';
    
    // Read array
    r->data = malloc(r->data_count * sizeof(int));
    fread(r->data, sizeof(int), r->data_count, file);
    
    return 0;
}
```

## Endianness

**Problem:** Different CPUs store multi-byte numbers differently.

**Little-endian** (x86, ARM): Least significant byte first  
`0x12345678` → `78 56 34 12`

**Big-endian** (network byte order): Most significant byte first  
`0x12345678` → `12 34 56 78`

### Portable Serialization

```c
#include <stdint.h>

// Write 32-bit integer in little-endian
void write_u32_le(FILE* file, uint32_t value) {
    uint8_t bytes[4];
    bytes[0] = value & 0xFF;
    bytes[1] = (value >> 8) & 0xFF;
    bytes[2] = (value >> 16) & 0xFF;
    bytes[3] = (value >> 24) & 0xFF;
    fwrite(bytes, 1, 4, file);
}

// Read 32-bit integer in little-endian
uint32_t read_u32_le(FILE* file) {
    uint8_t bytes[4];
    fread(bytes, 1, 4, file);
    return bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}

// Big-endian versions
void write_u32_be(FILE* file, uint32_t value) {
    uint8_t bytes[4];
    bytes[0] = (value >> 24) & 0xFF;
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF;
    fwrite(bytes, 1, 4, file);
}

uint32_t read_u32_be(FILE* file) {
    uint8_t bytes[4];
    fread(bytes, 1, 4, file);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}
```

## File Format Design

Good binary format includes:

```c
typedef struct {
    char magic[4];      // e.g., "MYFT" - identify file type
    uint16_t version;   // Format version for compatibility
    uint32_t flags;     // Feature flags
} FileHeader;

int save_file(const char* filename, void* data, size_t size) {
    FILE* file = fopen(filename, "wb");
    if (file == NULL) return -1;
    
    // Write header
    FileHeader header = {
        .magic = {'M', 'Y', 'F', 'T'},
        .version = 1,
        .flags = 0
    };
    fwrite(&header, sizeof(FileHeader), 1, file);
    
    // Write data
    fwrite(data, 1, size, file);
    
    fclose(file);
    return 0;
}

int load_file(const char* filename, void** data, size_t* size) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) return -1;
    
    // Read and validate header
    FileHeader header;
    fread(&header, sizeof(FileHeader), 1, file);
    
    if (memcmp(header.magic, "MYFT", 4) != 0) {
        fprintf(stderr, "Invalid file format\n");
        fclose(file);
        return -1;
    }
    
    if (header.version > 1) {
        fprintf(stderr, "Unsupported version\n");
        fclose(file);
        return -1;
    }
    
    // Get data size
    fseek(file, 0, SEEK_END);
    *size = ftell(file) - sizeof(FileHeader);
    fseek(file, sizeof(FileHeader), SEEK_SET);
    
    // Read data
    *data = malloc(*size);
    fread(*data, 1, *size, file);
    
    fclose(file);
    return 0;
}
```

## Memory Mapping (Advanced)

Instead of reading entire file, map it to memory:

### Windows
```c
#include <windows.h>

void* map_file(const char* filename, size_t* size) {
    HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ,
                             NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return NULL;
    
    *size = GetFileSize(file, NULL);
    
    HANDLE mapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
    if (mapping == NULL) {
        CloseHandle(file);
        return NULL;
    }
    
    void* data = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    
    CloseHandle(mapping);
    CloseHandle(file);
    
    return data;
}

void unmap_file(void* data) {
    UnmapViewOfFile(data);
}
```

### Linux
```c
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

void* map_file(const char* filename, size_t* size) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) return NULL;
    
    struct stat st;
    fstat(fd, &st);
    *size = st.st_size;
    
    void* data = mmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    close(fd);
    
    return (data == MAP_FAILED) ? NULL : data;
}

void unmap_file(void* data, size_t size) {
    munmap(data, size);
}
```

## Padding and Alignment

Structs may have padding:

```c
struct BadLayout {
    char a;      // 1 byte
    // 3 bytes padding!
    int b;       // 4 bytes
    char c;      // 1 byte
    // 3 bytes padding!
};  // Total: 12 bytes (not 6!)

struct GoodLayout {
    int b;       // 4 bytes
    char a;      // 1 byte
    char c;      // 1 byte
    // 2 bytes padding
};  // Total: 8 bytes
```

For portable binary files, either:

1. **Pack structs** (no padding):
```c
#pragma pack(push, 1)
struct PackedData {
    char a;
    int b;
    char c;
};  // Exactly 6 bytes
#pragma pack(pop)
```

2. **Write fields individually** (recommended):
```c
void save_data(FILE* file, Data* d) {
    fwrite(&d->a, sizeof(char), 1, file);
    fwrite(&d->b, sizeof(int), 1, file);
    fwrite(&d->c, sizeof(char), 1, file);
}
```

## Chunked Format

For extensible formats:

```c
typedef struct {
    char type[4];   // Chunk type
    uint32_t size;  // Chunk size
} ChunkHeader;

void write_chunk(FILE* file, const char* type, void* data, uint32_t size) {
    ChunkHeader header;
    memcpy(header.type, type, 4);
    header.size = size;
    
    fwrite(&header, sizeof(ChunkHeader), 1, file);
    fwrite(data, 1, size, file);
}

void read_chunks(FILE* file) {
    ChunkHeader header;
    
    while (fread(&header, sizeof(ChunkHeader), 1, file) == 1) {
        printf("Chunk: %.4s, size: %u\n", header.type, header.size);
        
        if (memcmp(header.type, "DATA", 4) == 0) {
            // Handle DATA chunk
            char* data = malloc(header.size);
            fread(data, 1, header.size, file);
            // Process...
            free(data);
        } else {
            // Skip unknown chunk
            fseek(file, header.size, SEEK_CUR);
        }
    }
}
```

This is how PNG, WAV, and other formats work!

## Common Pitfalls

**1. Forgetting binary mode:**
```c
fopen("data.bin", "w");  // BAD: Text mode on Windows corrupts binary!
fopen("data.bin", "wb"); // GOOD: Binary mode
```

**2. Writing pointers:**
```c
// BAD: Pointer values are meaningless in file!
struct Bad {
    char* name;
};
fwrite(&bad, sizeof(bad), 1, file);  // Writes pointer value!

// GOOD: Write what pointer points to
int len = strlen(name);
fwrite(&len, sizeof(int), 1, file);
fwrite(name, 1, len, file);
```

**3. Not checking return values:**
```c
fwrite(data, size, 1, file);  // BAD: Ignores errors!

if (fwrite(data, size, 1, file) != 1) {  // GOOD
    perror("fwrite");
    return -1;
}
```

## Summary

- Use binary mode (`"rb"`, `"wb"`) for non-text data
- `fwrite()` writes bytes, `fread()` reads bytes
- Design format with header (magic, version)
- Handle endianness for cross-platform files
- Don't write pointers or padded structs directly
- Always check return values
- Close files even on error

Binary files are faster and smaller than text but require more care. Design your format well and you'll have efficient, portable data storage.
