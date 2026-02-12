/*
 * Tokenizer (Lexer) Example
 * 
 * First step in building a language: break source code into tokens.
 * A token is the smallest meaningful unit (number, identifier, operator, etc.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EQUALS,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char lexeme[64];
    double number;    // For TOKEN_NUMBER
    int line;
} Token;

// Lexer state
typedef struct {
    const char* source;
    int current;
    int line;
} Lexer;

// Initialize lexer
void lexer_init(Lexer* lex, const char* source) {
    lex->source = source;
    lex->current = 0;
    lex->line = 1;
}

// Look at current character without consuming
char peek(Lexer* lex) {
    return lex->source[lex->current];
}

// Consume and return current character
char advance(Lexer* lex) {
    return lex->source[lex->current++];
}

// Skip whitespace and comments
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

// Scan a number
Token scan_number(Lexer* lex) {
    Token token;
    token.line = lex->line;
    
    int start = lex->current;
    
    // Consume digits
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
    
    // Copy lexeme
    int length = lex->current - start;
    memcpy(token.lexeme, &lex->source[start], length);
    token.lexeme[length] = '\0';
    
    // Parse number
    token.type = TOKEN_NUMBER;
    token.number = atof(token.lexeme);
    
    return token;
}

// Scan an identifier or keyword
Token scan_identifier(Lexer* lex) {
    Token token;
    token.line = lex->line;
    
    int start = lex->current;
    
    // Consume alphanumeric characters and underscores
    while (isalnum(peek(lex)) || peek(lex) == '_') {
        advance(lex);
    }
    
    // Copy lexeme
    int length = lex->current - start;
    memcpy(token.lexeme, &lex->source[start], length);
    token.lexeme[length] = '\0';
    
    token.type = TOKEN_IDENTIFIER;
    
    return token;
}

// Get next token
Token next_token(Lexer* lex) {
    skip_whitespace(lex);
    
    Token token;
    token.line = lex->line;
    
    char c = peek(lex);
    
    // End of file
    if (c == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }
    
    // Numbers
    if (isdigit(c)) {
        return scan_number(lex);
    }
    
    // Identifiers
    if (isalpha(c) || c == '_') {
        return scan_identifier(lex);
    }
    
    // Single-character tokens
    advance(lex);
    
    switch (c) {
        case '+':
            token.type = TOKEN_PLUS;
            strcpy(token.lexeme, "+");
            break;
        case '-':
            token.type = TOKEN_MINUS;
            strcpy(token.lexeme, "-");
            break;
        case '*':
            token.type = TOKEN_STAR;
            strcpy(token.lexeme, "*");
            break;
        case '/':
            token.type = TOKEN_SLASH;
            strcpy(token.lexeme, "/");
            break;
        case '(':
            token.type = TOKEN_LPAREN;
            strcpy(token.lexeme, "(");
            break;
        case ')':
            token.type = TOKEN_RPAREN;
            strcpy(token.lexeme, ")");
            break;
        case '=':
            token.type = TOKEN_EQUALS;
            strcpy(token.lexeme, "=");
            break;
        case ';':
            token.type = TOKEN_SEMICOLON;
            strcpy(token.lexeme, ";");
            break;
        default:
            token.type = TOKEN_ERROR;
            snprintf(token.lexeme, sizeof(token.lexeme), "Unexpected '%c'", c);
            break;
    }
    
    return token;
}

// Print token (for debugging)
void print_token(Token* token) {
    const char* type_names[] = {
        "NUMBER", "IDENTIFIER", "PLUS", "MINUS", "STAR", "SLASH",
        "LPAREN", "RPAREN", "EQUALS", "SEMICOLON", "EOF", "ERROR"
    };
    
    printf("%-12s", type_names[token->type]);
    printf("'%-15s'", token->lexeme);
    
    if (token->type == TOKEN_NUMBER) {
        printf(" (value: %.2f)", token->number);
    }
    
    printf("\n");
}

int main(void) {
    printf("=== Tokenizer Example ===\n\n");
    
    // Example 1: Simple expression
    printf("Example 1: Simple arithmetic\n");
    printf("Source: 2 + 3 * 4\n\n");
    
    const char* source1 = "2 + 3 * 4";
    Lexer lex1;
    lexer_init(&lex1, source1);
    
    printf("Tokens:\n");
    Token token;
    do {
        token = next_token(&lex1);
        print_token(&token);
    } while (token.type != TOKEN_EOF);
    
    printf("\n---\n\n");
    
    // Example 2: Variable assignment
    printf("Example 2: Variable assignment\n");
    printf("Source: x = 10 + 5;\n\n");
    
    const char* source2 = "x = 10 + 5;";
    Lexer lex2;
    lexer_init(&lex2, source2);
    
    printf("Tokens:\n");
    do {
        token = next_token(&lex2);
        print_token(&token);
    } while (token.type != TOKEN_EOF);
    
    printf("\n---\n\n");
    
    // Example 3: Complex expression
    printf("Example 3: Complex expression\n");
    printf("Source: result = (a + b) * (c - d) / 2.5;\n\n");
    
    const char* source3 = "result = (a + b) * (c - d) / 2.5;";
    Lexer lex3;
    lexer_init(&lex3, source3);
    
    printf("Tokens:\n");
    do {
        token = next_token(&lex3);
        print_token(&token);
    } while (token.type != TOKEN_EOF);
    
    printf("\n---\n\n");
    
    // Example 4: Multi-line with comments
    printf("Example 4: Multi-line with comments\n");
    printf("Source:\n");
    printf("  // Calculate area\n");
    printf("  width = 10;\n");
    printf("  height = 20;\n");
    printf("  area = width * height;\n\n");
    
    const char* source4 = 
        "// Calculate area\n"
        "width = 10;\n"
        "height = 20;\n"
        "area = width * height;";
    
    Lexer lex4;
    lexer_init(&lex4, source4);
    
    printf("Tokens:\n");
    do {
        token = next_token(&lex4);
        print_token(&token);
    } while (token.type != TOKEN_EOF);
    
    printf("\n=== Summary ===\n");
    printf("The tokenizer breaks source code into meaningful chunks.\n");
    printf("Each token has:\n");
    printf("  - Type (NUMBER, IDENTIFIER, OPERATOR, etc.)\n");
    printf("  - Lexeme (the actual text)\n");
    printf("  - Value (for numbers)\n");
    printf("  - Line number (for error messages)\n\n");
    
    printf("Next step: Parse these tokens into a syntax tree!\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
