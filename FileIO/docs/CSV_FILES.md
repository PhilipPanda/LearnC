# CSV Files

## The Big Picture

CSV (Comma-Separated Values) is everywhere - Excel exports, databases, data analysis. Simple format but trickier than it looks because of quotes, commas in values, and escaping.

```csv
name,age,city
Alice,25,"New York"
Bob,30,"San Francisco, CA"
"Smith, John",35,Boston
```

## Basic CSV Parsing

Simple version (no quotes):

```c
#include <stdio.h>
#include <string.h>

void parse_simple_csv(const char* line) {
    char buffer[256];
    strcpy(buffer, line);
    
    char* token = strtok(buffer, ",");
    while (token != NULL) {
        printf("Field: [%s]\n", token);
        token = strtok(NULL, ",");
    }
}
```

**Problem:** Doesn't handle commas inside quoted fields!

## Proper CSV Parser

Handles quotes and embedded commas:

```c
typedef struct {
    char** fields;
    int count;
} CSVRow;

CSVRow parse_csv_line(const char* line) {
    CSVRow row = {0};
    row.fields = NULL;
    row.count = 0;
    
    char field[1024];
    int field_len = 0;
    int in_quotes = 0;
    
    for (const char* p = line; *p; p++) {
        if (*p == '"') {
            if (in_quotes && *(p + 1) == '"') {
                // Escaped quote ("" -> ")
                field[field_len++] = '"';
                p++;  // Skip next quote
            } else {
                // Toggle quote mode
                in_quotes = !in_quotes;
            }
        } else if (*p == ',' && !in_quotes) {
            // End of field
            field[field_len] = '\0';
            
            row.fields = realloc(row.fields, (row.count + 1) * sizeof(char*));
            row.fields[row.count++] = strdup(field);
            
            field_len = 0;
        } else if (*p == '\n' || *p == '\r') {
            // Skip newlines
            continue;
        } else {
            // Regular character
            field[field_len++] = *p;
        }
    }
    
    // Last field
    if (field_len > 0 || row.count > 0) {
        field[field_len] = '\0';
        row.fields = realloc(row.fields, (row.count + 1) * sizeof(char*));
        row.fields[row.count++] = strdup(field);
    }
    
    return row;
}

void free_csv_row(CSVRow* row) {
    for (int i = 0; i < row->count; i++) {
        free(row->fields[i]);
    }
    free(row->fields);
    row->fields = NULL;
    row->count = 0;
}
```

## Reading CSV File

```c
typedef struct {
    CSVRow* rows;
    int row_count;
} CSVFile;

CSVFile read_csv_file(const char* filename) {
    CSVFile csv = {0};
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return csv;
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), file) != NULL) {
        CSVRow row = parse_csv_line(line);
        
        if (row.count > 0) {
            csv.rows = realloc(csv.rows, (csv.row_count + 1) * sizeof(CSVRow));
            csv.rows[csv.row_count++] = row;
        }
    }
    
    fclose(file);
    return csv;
}

void free_csv_file(CSVFile* csv) {
    for (int i = 0; i < csv->row_count; i++) {
        free_csv_row(&csv->rows[i]);
    }
    free(csv->rows);
    csv->rows = NULL;
    csv->row_count = 0;
}
```

## Writing CSV

```c
void csv_write_field(FILE* file, const char* value) {
    int needs_quotes = 0;
    
    // Check if field needs quotes
    for (const char* p = value; *p; p++) {
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') {
            needs_quotes = 1;
            break;
        }
    }
    
    if (needs_quotes) {
        fputc('"', file);
        for (const char* p = value; *p; p++) {
            if (*p == '"') {
                // Escape quotes by doubling them
                fputc('"', file);
                fputc('"', file);
            } else {
                fputc(*p, file);
            }
        }
        fputc('"', file);
    } else {
        fputs(value, file);
    }
}

void csv_write_row(FILE* file, char** fields, int count) {
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            fputc(',', file);
        }
        csv_write_field(file, fields[i]);
    }
    fputc('\n', file);
}
```

## Example: People Database

```c
typedef struct {
    char name[50];
    int age;
    char city[50];
} Person;

void save_people_csv(const char* filename, Person* people, int count) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) return;
    
    // Header
    char* header[] = {"name", "age", "city"};
    csv_write_row(file, header, 3);
    
    // Data
    for (int i = 0; i < count; i++) {
        char age_str[20];
        snprintf(age_str, sizeof(age_str), "%d", people[i].age);
        
        char* row[] = {people[i].name, age_str, people[i].city};
        csv_write_row(file, row, 3);
    }
    
    fclose(file);
}

Person* load_people_csv(const char* filename, int* count) {
    CSVFile csv = read_csv_file(filename);
    
    if (csv.row_count <= 1) {
        *count = 0;
        return NULL;
    }
    
    // Skip header row
    *count = csv.row_count - 1;
    Person* people = malloc(*count * sizeof(Person));
    
    for (int i = 1; i < csv.row_count; i++) {
        CSVRow* row = &csv.rows[i];
        
        if (row->count >= 3) {
            strcpy(people[i - 1].name, row->fields[0]);
            people[i - 1].age = atoi(row->fields[1]);
            strcpy(people[i - 1].city, row->fields[2]);
        }
    }
    
    free_csv_file(&csv);
    return people;
}
```

## CSV Gotchas

### 1. Different delimiters

Not always commas! Sometimes semicolons, tabs, pipes:

```c
char delimiter = ',';  // Or ';', '\t', '|'

// In parser:
if (*p == delimiter && !in_quotes) {
    // End of field
}
```

### 2. Line endings

Windows (`\r\n`), Unix (`\n`), Mac (`\r`):

```c
// Strip any line ending
void strip_line_ending(char* line) {
    int len = strlen(line);
    while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
        line[--len] = '\0';
    }
}
```

### 3. BOM (Byte Order Mark)

UTF-8 CSV might start with BOM (0xEF 0xBB 0xBF):

```c
void skip_bom(FILE* file) {
    unsigned char bom[3];
    long pos = ftell(file);
    
    if (fread(bom, 1, 3, file) == 3) {
        if (!(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)) {
            // Not a BOM, rewind
            fseek(file, pos, SEEK_SET);
        }
    }
}
```

### 4. Empty fields

```csv
name,age,city
Alice,25,
Bob,,Boston
```

Make sure parser handles empty fields correctly!

## CSV to Struct Mapping

Generic approach:

```c
typedef struct {
    char* name;
    enum { TYPE_INT, TYPE_DOUBLE, TYPE_STRING } type;
    size_t offset;
} FieldDef;

#define FIELD(struct_type, field_name, field_type) \
    { #field_name, field_type, offsetof(struct_type, field_name) }

FieldDef person_fields[] = {
    FIELD(Person, name, TYPE_STRING),
    FIELD(Person, age, TYPE_INT),
    FIELD(Person, city, TYPE_STRING),
};

void csv_to_struct(CSVRow* row, void* dest, FieldDef* fields, int field_count) {
    for (int i = 0; i < field_count && i < row->count; i++) {
        void* field_ptr = (char*)dest + fields[i].offset;
        
        switch (fields[i].type) {
            case TYPE_INT:
                *(int*)field_ptr = atoi(row->fields[i]);
                break;
            case TYPE_DOUBLE:
                *(double*)field_ptr = atof(row->fields[i]);
                break;
            case TYPE_STRING:
                strcpy((char*)field_ptr, row->fields[i]);
                break;
        }
    }
}
```

## Performance Tips

**1. Buffer writes:**
```c
char buffer[8192];
setvbuf(file, buffer, _IOFBF, sizeof(buffer));
```

**2. Estimate capacity:**
```c
// Pre-allocate for expected number of rows
csv.rows = malloc(1000 * sizeof(CSVRow));
csv.capacity = 1000;
csv.row_count = 0;
```

**3. Avoid reallocations:**
```c
if (csv.row_count >= csv.capacity) {
    csv.capacity *= 2;
    csv.rows = realloc(csv.rows, csv.capacity * sizeof(CSVRow));
}
```

## CSV Libraries

For production use, consider libraries:
- **libcsv** - Fast, RFC 4180 compliant
- **csv-parser** - Simple, header-only

But writing your own teaches you:
- String parsing
- State machines
- Edge case handling
- Memory management

## Summary

CSV is simple but has edge cases:
- Quoted fields can contain commas
- Quotes are escaped by doubling them
- Watch for different delimiters and line endings
- Empty fields are valid
- BOM at start of UTF-8 files

Key techniques:
- State machine (in_quotes flag)
- Character-by-character parsing
- Dynamic memory for variable fields
- Proper escaping when writing

CSV is ubiquitous. Master it and you can exchange data with any tool.
