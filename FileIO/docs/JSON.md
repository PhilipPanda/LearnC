# JSON Parsing

## The Big Picture

JSON (JavaScript Object Notation) is the most common data exchange format on the web. APIs, config files, and data storage all use JSON.

```json
{
    "name": "Alice",
    "age": 25,
    "active": true,
    "scores": [95, 87, 92],
    "address": {
        "city": "New York",
        "zip": "10001"
    }
}
```

Building a full JSON parser from scratch teaches lexing, parsing, and tree structures.

## JSON Types

```c
typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
} JSONType;

typedef struct JSONValue {
    JSONType type;
    union {
        int boolean;           // For JSON_BOOL
        double number;         // For JSON_NUMBER
        char* string;          // For JSON_STRING
        struct {               // For JSON_ARRAY
            struct JSONValue** items;
            int count;
        } array;
        struct {               // For JSON_OBJECT
            char** keys;
            struct JSONValue** values;
            int count;
        } object;
    };
} JSONValue;
```

## Tokenizer

Break JSON into tokens:

```c
typedef enum {
    TOKEN_LBRACE,     // {
    TOKEN_RBRACE,     // }
    TOKEN_LBRACKET,   // [
    TOKEN_RBRACKET,   // ]
    TOKEN_COLON,      // :
    TOKEN_COMMA,      // ,
    TOKEN_STRING,     // "text"
    TOKEN_NUMBER,     // 123, 45.67
    TOKEN_TRUE,       // true
    TOKEN_FALSE,      // false
    TOKEN_NULL,       // null
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    const char* source;
    int pos;
} JSONLexer;

void skip_whitespace(JSONLexer* lex) {
    while (lex->source[lex->pos] == ' ' || 
           lex->source[lex->pos] == '\t' ||
           lex->source[lex->pos] == '\n' ||
           lex->source[lex->pos] == '\r') {
        lex->pos++;
    }
}

TokenType next_token(JSONLexer* lex, char** value) {
    skip_whitespace(lex);
    
    char c = lex->source[lex->pos];
    
    if (c == '\0') return TOKEN_EOF;
    
    // Single-character tokens
    if (c == '{') { lex->pos++; return TOKEN_LBRACE; }
    if (c == '}') { lex->pos++; return TOKEN_RBRACE; }
    if (c == '[') { lex->pos++; return TOKEN_LBRACKET; }
    if (c == ']') { lex->pos++; return TOKEN_RBRACKET; }
    if (c == ':') { lex->pos++; return TOKEN_COLON; }
    if (c == ',') { lex->pos++; return TOKEN_COMMA; }
    
    // String
    if (c == '"') {
        return parse_string(lex, value);
    }
    
    // Number
    if (c == '-' || isdigit(c)) {
        return parse_number(lex, value);
    }
    
    // Keywords
    if (strncmp(&lex->source[lex->pos], "true", 4) == 0) {
        lex->pos += 4;
        return TOKEN_TRUE;
    }
    if (strncmp(&lex->source[lex->pos], "false", 5) == 0) {
        lex->pos += 5;
        return TOKEN_FALSE;
    }
    if (strncmp(&lex->source[lex->pos], "null", 4) == 0) {
        lex->pos += 4;
        return TOKEN_NULL;
    }
    
    return TOKEN_ERROR;
}
```

## Parse String

Handle escape sequences:

```c
TokenType parse_string(JSONLexer* lex, char** value) {
    lex->pos++;  // Skip opening "
    
    char buffer[4096];
    int len = 0;
    
    while (lex->source[lex->pos] != '"') {
        if (lex->source[lex->pos] == '\0') {
            return TOKEN_ERROR;  // Unterminated string
        }
        
        if (lex->source[lex->pos] == '\\') {
            lex->pos++;
            switch (lex->source[lex->pos]) {
                case '"':  buffer[len++] = '"'; break;
                case '\\': buffer[len++] = '\\'; break;
                case '/':  buffer[len++] = '/'; break;
                case 'b':  buffer[len++] = '\b'; break;
                case 'f':  buffer[len++] = '\f'; break;
                case 'n':  buffer[len++] = '\n'; break;
                case 'r':  buffer[len++] = '\r'; break;
                case 't':  buffer[len++] = '\t'; break;
                case 'u':  // Unicode escape \uXXXX
                    // TODO: Parse hex digits
                    break;
                default:
                    return TOKEN_ERROR;
            }
            lex->pos++;
        } else {
            buffer[len++] = lex->source[lex->pos++];
        }
    }
    
    lex->pos++;  // Skip closing "
    buffer[len] = '\0';
    *value = strdup(buffer);
    
    return TOKEN_STRING;
}
```

## Parser

Recursive descent parser:

```c
typedef struct {
    JSONLexer lexer;
    TokenType current_token;
    char* current_value;
} JSONParser;

void parser_init(JSONParser* p, const char* json) {
    p->lexer.source = json;
    p->lexer.pos = 0;
    p->current_token = next_token(&p->lexer, &p->current_value);
}

void consume(JSONParser* p) {
    if (p->current_value) {
        free(p->current_value);
        p->current_value = NULL;
    }
    p->current_token = next_token(&p->lexer, &p->current_value);
}

JSONValue* parse_value(JSONParser* p);

JSONValue* parse_object(JSONParser* p) {
    JSONValue* obj = malloc(sizeof(JSONValue));
    obj->type = JSON_OBJECT;
    obj->object.keys = NULL;
    obj->object.values = NULL;
    obj->object.count = 0;
    
    consume(p);  // {
    
    while (p->current_token != TOKEN_RBRACE) {
        // Parse key
        if (p->current_token != TOKEN_STRING) {
            // Error: Expected string key
            return NULL;
        }
        char* key = strdup(p->current_value);
        consume(p);
        
        // Expect :
        if (p->current_token != TOKEN_COLON) {
            free(key);
            return NULL;
        }
        consume(p);
        
        // Parse value
        JSONValue* value = parse_value(p);
        
        // Add to object
        obj->object.keys = realloc(obj->object.keys, 
                                   (obj->object.count + 1) * sizeof(char*));
        obj->object.values = realloc(obj->object.values,
                                     (obj->object.count + 1) * sizeof(JSONValue*));
        obj->object.keys[obj->object.count] = key;
        obj->object.values[obj->object.count] = value;
        obj->object.count++;
        
        // Optional comma
        if (p->current_token == TOKEN_COMMA) {
            consume(p);
        } else {
            break;
        }
    }
    
    consume(p);  // }
    return obj;
}

JSONValue* parse_array(JSONParser* p) {
    JSONValue* arr = malloc(sizeof(JSONValue));
    arr->type = JSON_ARRAY;
    arr->array.items = NULL;
    arr->array.count = 0;
    
    consume(p);  // [
    
    while (p->current_token != TOKEN_RBRACKET) {
        JSONValue* item = parse_value(p);
        
        arr->array.items = realloc(arr->array.items,
                                   (arr->array.count + 1) * sizeof(JSONValue*));
        arr->array.items[arr->array.count++] = item;
        
        if (p->current_token == TOKEN_COMMA) {
            consume(p);
        } else {
            break;
        }
    }
    
    consume(p);  // ]
    return arr;
}

JSONValue* parse_value(JSONParser* p) {
    JSONValue* value = malloc(sizeof(JSONValue));
    
    switch (p->current_token) {
        case TOKEN_NULL:
            value->type = JSON_NULL;
            consume(p);
            break;
            
        case TOKEN_TRUE:
            value->type = JSON_BOOL;
            value->boolean = 1;
            consume(p);
            break;
            
        case TOKEN_FALSE:
            value->type = JSON_BOOL;
            value->boolean = 0;
            consume(p);
            break;
            
        case TOKEN_NUMBER:
            value->type = JSON_NUMBER;
            value->number = atof(p->current_value);
            consume(p);
            break;
            
        case TOKEN_STRING:
            value->type = JSON_STRING;
            value->string = strdup(p->current_value);
            consume(p);
            break;
            
        case TOKEN_LBRACE:
            free(value);
            return parse_object(p);
            
        case TOKEN_LBRACKET:
            free(value);
            return parse_array(p);
            
        default:
            free(value);
            return NULL;
    }
    
    return value;
}

JSONValue* json_parse(const char* json_string) {
    JSONParser parser;
    parser_init(&parser, json_string);
    return parse_value(&parser);
}
```

## Accessing Values

```c
JSONValue* json_object_get(JSONValue* obj, const char* key) {
    if (obj->type != JSON_OBJECT) return NULL;
    
    for (int i = 0; i < obj->object.count; i++) {
        if (strcmp(obj->object.keys[i], key) == 0) {
            return obj->object.values[i];
        }
    }
    
    return NULL;
}

JSONValue* json_array_get(JSONValue* arr, int index) {
    if (arr->type != JSON_ARRAY) return NULL;
    if (index < 0 || index >= arr->array.count) return NULL;
    
    return arr->array.items[index];
}

// Usage
JSONValue* root = json_parse(json_string);
JSONValue* name = json_object_get(root, "name");
if (name && name->type == JSON_STRING) {
    printf("Name: %s\n", name->string);
}
```

## Generating JSON

```c
void json_write_value(FILE* file, JSONValue* value, int indent);

void json_write_indent(FILE* file, int indent) {
    for (int i = 0; i < indent; i++) {
        fprintf(file, "  ");
    }
}

void json_write_string(FILE* file, const char* str) {
    fputc('"', file);
    for (const char* p = str; *p; p++) {
        switch (*p) {
            case '"':  fputs("\\\"", file); break;
            case '\\': fputs("\\\\", file); break;
            case '\b': fputs("\\b", file); break;
            case '\f': fputs("\\f", file); break;
            case '\n': fputs("\\n", file); break;
            case '\r': fputs("\\r", file); break;
            case '\t': fputs("\\t", file); break;
            default:   fputc(*p, file); break;
        }
    }
    fputc('"', file);
}

void json_write_object(FILE* file, JSONValue* obj, int indent) {
    fputs("{\n", file);
    
    for (int i = 0; i < obj->object.count; i++) {
        json_write_indent(file, indent + 1);
        json_write_string(file, obj->object.keys[i]);
        fputs(": ", file);
        json_write_value(file, obj->object.values[i], indent + 1);
        
        if (i < obj->object.count - 1) {
            fputs(",", file);
        }
        fputs("\n", file);
    }
    
    json_write_indent(file, indent);
    fputc('}', file);
}

void json_write_array(FILE* file, JSONValue* arr, int indent) {
    fputs("[\n", file);
    
    for (int i = 0; i < arr->array.count; i++) {
        json_write_indent(file, indent + 1);
        json_write_value(file, arr->array.items[i], indent + 1);
        
        if (i < arr->array.count - 1) {
            fputs(",", file);
        }
        fputs("\n", file);
    }
    
    json_write_indent(file, indent);
    fputc(']', file);
}

void json_write_value(FILE* file, JSONValue* value, int indent) {
    switch (value->type) {
        case JSON_NULL:
            fputs("null", file);
            break;
            
        case JSON_BOOL:
            fputs(value->boolean ? "true" : "false", file);
            break;
            
        case JSON_NUMBER:
            fprintf(file, "%.10g", value->number);
            break;
            
        case JSON_STRING:
            json_write_string(file, value->string);
            break;
            
        case JSON_ARRAY:
            json_write_array(file, value, indent);
            break;
            
        case JSON_OBJECT:
            json_write_object(file, value, indent);
            break;
    }
}
```

## Memory Management

```c
void json_free(JSONValue* value) {
    if (value == NULL) return;
    
    switch (value->type) {
        case JSON_STRING:
            free(value->string);
            break;
            
        case JSON_ARRAY:
            for (int i = 0; i < value->array.count; i++) {
                json_free(value->array.items[i]);
            }
            free(value->array.items);
            break;
            
        case JSON_OBJECT:
            for (int i = 0; i < value->object.count; i++) {
                free(value->object.keys[i]);
                json_free(value->object.values[i]);
            }
            free(value->object.keys);
            free(value->object.values);
            break;
            
        default:
            break;
    }
    
    free(value);
}
```

## Builder API

Easier way to create JSON:

```c
JSONValue* json_object_create() {
    JSONValue* obj = malloc(sizeof(JSONValue));
    obj->type = JSON_OBJECT;
    obj->object.keys = NULL;
    obj->object.values = NULL;
    obj->object.count = 0;
    return obj;
}

void json_object_set(JSONValue* obj, const char* key, JSONValue* value) {
    obj->object.keys = realloc(obj->object.keys,
                               (obj->object.count + 1) * sizeof(char*));
    obj->object.values = realloc(obj->object.values,
                                 (obj->object.count + 1) * sizeof(JSONValue*));
    obj->object.keys[obj->object.count] = strdup(key);
    obj->object.values[obj->object.count] = value;
    obj->object.count++;
}

JSONValue* json_string(const char* str) {
    JSONValue* val = malloc(sizeof(JSONValue));
    val->type = JSON_STRING;
    val->string = strdup(str);
    return val;
}

JSONValue* json_number(double num) {
    JSONValue* val = malloc(sizeof(JSONValue));
    val->type = JSON_NUMBER;
    val->number = num;
    return val;
}

// Usage
JSONValue* person = json_object_create();
json_object_set(person, "name", json_string("Alice"));
json_object_set(person, "age", json_number(25));
```

## JSON Libraries

For production:
- **cJSON** - Simple, single-file
- **jansson** - Robust, well-documented
- **json-c** - Fast, widely used

But writing your own teaches:
- Parsing techniques
- Recursive data structures
- Memory management
- Error handling

## Summary

JSON parsing involves:
- Tokenization (lexing)
- Recursive descent parsing
- Tree building
- Proper memory management

Key points:
- Handle escape sequences in strings
- Parse numbers correctly (scientific notation, etc.)
- Recursive structures (objects contain objects)
- Always free memory
- Validate input (malformed JSON is common)

JSON is ubiquitous in modern programming. Understanding how parsers work makes you better at using and debugging any data format.
