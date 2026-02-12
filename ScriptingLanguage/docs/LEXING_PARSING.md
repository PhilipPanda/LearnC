# Lexing & Parsing

## The Big Picture

Source code is just text. To run it, we need to understand its structure. This happens in two phases:

**Lexing (Tokenization):** Break text into meaningful chunks (tokens)  
**Parsing:** Build a structure (syntax tree) from those tokens

```
"x = 2 + 3" → [x, =, 2, +, 3] → Tree:
                                    =
                                   / \
                                  x   +
                                     / \
                                    2   3
```

## Lexing (Tokenization)

Turn source code into tokens.

### What is a Token?

A token is the smallest meaningful unit:

```c
// Source: let x = 10 + 5;
// Tokens:
let     → KEYWORD
x       → IDENTIFIER
=       → OPERATOR
10      → NUMBER
+       → OPERATOR
5       → NUMBER
;       → SEMICOLON
```

### Token Types

```c
typedef enum {
    TOKEN_NUMBER,        // 123, 45.67
    TOKEN_IDENTIFIER,    // x, myVar, functionName
    TOKEN_STRING,        // "hello"
    TOKEN_KEYWORD,       // let, if, while, fn, return
    TOKEN_OPERATOR,      // +, -, *, /, =, ==, <, >
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_LBRACE,        // {
    TOKEN_RBRACE,        // }
    TOKEN_SEMICOLON,     // ;
    TOKEN_COMMA,         // ,
    TOKEN_EOF,           // End of file
    TOKEN_ERROR          // Unrecognized character
} TokenType;

typedef struct {
    TokenType type;
    char* lexeme;        // The actual text
    double number;       // For TOKEN_NUMBER
    int line;            // For error messages
} Token;
```

### Lexer Implementation

```c
typedef struct {
    const char* source;
    int current;
    int line;
} Lexer;

void lexer_init(Lexer* lex, const char* source) {
    lex->source = source;
    lex->current = 0;
    lex->line = 1;
}

char peek(Lexer* lex) {
    return lex->source[lex->current];
}

char advance(Lexer* lex) {
    return lex->source[lex->current++];
}

void skip_whitespace(Lexer* lex) {
    while (1) {
        char c = peek(lex);
        if (c == ' ' || c == '\t' || c == '\r') {
            advance(lex);
        } else if (c == '\n') {
            lex->line++;
            advance(lex);
        } else if (c == '/' && lex->source[lex->current + 1] == '/') {
            // Skip comment until end of line
            while (peek(lex) != '\n' && peek(lex) != '\0') {
                advance(lex);
            }
        } else {
            break;
        }
    }
}

Token scan_number(Lexer* lex) {
    int start = lex->current;
    
    while (isdigit(peek(lex))) {
        advance(lex);
    }
    
    // Handle decimal point
    if (peek(lex) == '.' && isdigit(lex->source[lex->current + 1])) {
        advance(lex);  // Consume '.'
        while (isdigit(peek(lex))) {
            advance(lex);
        }
    }
    
    int length = lex->current - start;
    char* lexeme = malloc(length + 1);
    memcpy(lexeme, &lex->source[start], length);
    lexeme[length] = '\0';
    
    Token token;
    token.type = TOKEN_NUMBER;
    token.lexeme = lexeme;
    token.number = atof(lexeme);
    token.line = lex->line;
    
    return token;
}

Token scan_identifier(Lexer* lex) {
    int start = lex->current;
    
    while (isalnum(peek(lex)) || peek(lex) == '_') {
        advance(lex);
    }
    
    int length = lex->current - start;
    char* lexeme = malloc(length + 1);
    memcpy(lexeme, &lex->source[start], length);
    lexeme[length] = '\0';
    
    Token token;
    token.lexeme = lexeme;
    token.line = lex->line;
    
    // Check if it's a keyword
    if (strcmp(lexeme, "let") == 0 || strcmp(lexeme, "if") == 0 ||
        strcmp(lexeme, "else") == 0 || strcmp(lexeme, "while") == 0 ||
        strcmp(lexeme, "for") == 0 || strcmp(lexeme, "fn") == 0 ||
        strcmp(lexeme, "return") == 0) {
        token.type = TOKEN_KEYWORD;
    } else {
        token.type = TOKEN_IDENTIFIER;
    }
    
    return token;
}

Token next_token(Lexer* lex) {
    skip_whitespace(lex);
    
    char c = peek(lex);
    
    // End of file
    if (c == '\0') {
        Token token;
        token.type = TOKEN_EOF;
        token.lexeme = "";
        token.line = lex->line;
        return token;
    }
    
    // Numbers
    if (isdigit(c)) {
        return scan_number(lex);
    }
    
    // Identifiers and keywords
    if (isalpha(c) || c == '_') {
        return scan_identifier(lex);
    }
    
    // Single character tokens
    advance(lex);
    Token token;
    token.line = lex->line;
    
    switch (c) {
        case '(': token.type = TOKEN_LPAREN; break;
        case ')': token.type = TOKEN_RPAREN; break;
        case '{': token.type = TOKEN_LBRACE; break;
        case '}': token.type = TOKEN_RBRACE; break;
        case ';': token.type = TOKEN_SEMICOLON; break;
        case ',': token.type = TOKEN_COMMA; break;
        case '+': case '-': case '*': case '/':
        case '=': case '<': case '>': case '!':
            token.type = TOKEN_OPERATOR;
            break;
        default:
            token.type = TOKEN_ERROR;
            break;
    }
    
    return token;
}
```

## Parsing

Build a syntax tree from tokens.

### Abstract Syntax Tree (AST)

Represents program structure:

```
Expression: 2 + 3 * 4

Tree:
       +
      / \
     2   *
        / \
       3   4
       
Evaluates as: 2 + (3 * 4) = 14
```

### AST Node Types

```c
typedef enum {
    AST_NUMBER,          // 123
    AST_IDENTIFIER,      // x
    AST_BINARY_OP,       // a + b
    AST_UNARY_OP,        // -x
    AST_ASSIGN,          // x = 5
    AST_CALL,            // foo(a, b)
} ASTType;

typedef struct ASTNode {
    ASTType type;
    
    union {
        double number;           // AST_NUMBER
        char* identifier;        // AST_IDENTIFIER
        
        struct {                 // AST_BINARY_OP
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        
        struct {                 // AST_UNARY_OP
            char op;
            struct ASTNode* operand;
        } unary;
        
        struct {                 // AST_ASSIGN
            char* name;
            struct ASTNode* value;
        } assign;
    };
} ASTNode;
```

### Recursive Descent Parser

Parse expressions with correct precedence:

```c
typedef struct {
    Lexer lexer;
    Token current;
} Parser;

void parser_init(Parser* p, const char* source) {
    lexer_init(&p->lexer, source);
    p->current = next_token(&p->lexer);
}

Token consume(Parser* p) {
    Token old = p->current;
    p->current = next_token(&p->lexer);
    return old;
}

// Grammar:
// expression → term (('+' | '-') term)*
// term       → factor (('*' | '/') factor)*
// factor     → NUMBER | '(' expression ')'

ASTNode* parse_factor(Parser* p) {
    if (p->current.type == TOKEN_NUMBER) {
        Token token = consume(p);
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_NUMBER;
        node->number = token.number;
        return node;
    }
    
    if (p->current.type == TOKEN_LPAREN) {
        consume(p);  // '('
        ASTNode* expr = parse_expression(p);
        consume(p);  // ')'
        return expr;
    }
    
    // Error handling
    return NULL;
}

ASTNode* parse_term(Parser* p) {
    ASTNode* left = parse_factor(p);
    
    while (p->current.type == TOKEN_OPERATOR &&
           (p->current.lexeme[0] == '*' || p->current.lexeme[0] == '/')) {
        Token op = consume(p);
        ASTNode* right = parse_factor(p);
        
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_BINARY_OP;
        node->binary.op = op.lexeme[0];
        node->binary.left = left;
        node->binary.right = right;
        
        left = node;
    }
    
    return left;
}

ASTNode* parse_expression(Parser* p) {
    ASTNode* left = parse_term(p);
    
    while (p->current.type == TOKEN_OPERATOR &&
           (p->current.lexeme[0] == '+' || p->current.lexeme[0] == '-')) {
        Token op = consume(p);
        ASTNode* right = parse_term(p);
        
        ASTNode* node = malloc(sizeof(ASTNode));
        node->type = AST_BINARY_OP;
        node->binary.op = op.lexeme[0];
        node->binary.left = left;
        node->binary.right = right;
        
        left = node;
    }
    
    return left;
}
```

This naturally handles operator precedence:
- `*` and `/` bind tighter (in `parse_term`)
- `+` and `-` bind looser (in `parse_expression`)

## Error Handling

```c
void parser_error(Parser* p, const char* message) {
    fprintf(stderr, "Error on line %d: %s\n", p->current.line, message);
    fprintf(stderr, "  Near: '%s'\n", p->current.lexeme);
}
```

## Putting It Together

```c
int main() {
    const char* source = "2 + 3 * 4";
    
    // Lex
    Lexer lexer;
    lexer_init(&lexer, source);
    
    Token token;
    while ((token = next_token(&lexer)).type != TOKEN_EOF) {
        printf("Token: %s\n", token.lexeme);
    }
    
    // Parse
    Parser parser;
    parser_init(&parser, source);
    ASTNode* ast = parse_expression(&parser);
    
    // Evaluate (next step!)
    double result = evaluate(ast);
    printf("Result: %f\n", result);  // 14.0
}
```

## Summary

**Lexing:** Text → Tokens (meaningful chunks)  
**Parsing:** Tokens → AST (tree structure)

The AST captures program structure and operator precedence. Next step: evaluate it to get results!
