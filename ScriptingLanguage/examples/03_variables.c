/*
 * Variables Example
 * 
 * Adds variables to our language. Now we can:
 * - Declare variables: let x = 10;
 * - Assign values: x = 20;
 * - Use variables in expressions: y = x * 2;
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TOKEN_NUMBER, TOKEN_IDENTIFIER,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
    TOKEN_EQUALS, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_SEMICOLON,
    TOKEN_LET,  // NEW: 'let' keyword
    TOKEN_EOF, TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[64];
    double number;
} Token;

// Lexer
typedef struct {
    const char* source;
    int current;
} Lexer;

void lexer_init(Lexer* lex, const char* source) {
    lex->source = source;
    lex->current = 0;
}

char peek(Lexer* lex) {
    return lex->source[lex->current];
}

char advance(Lexer* lex) {
    return lex->source[lex->current++];
}

void skip_whitespace(Lexer* lex) {
    while (peek(lex) == ' ' || peek(lex) == '\t' || peek(lex) == '\n' || peek(lex) == '\r') {
        advance(lex);
    }
}

Token next_token(Lexer* lex) {
    skip_whitespace(lex);
    
    Token token;
    char c = peek(lex);
    
    if (c == '\0') {
        token.type = TOKEN_EOF;
        return token;
    }
    
    // Numbers
    if (isdigit(c) || c == '.') {
        int start = lex->current;
        while (isdigit(peek(lex))) advance(lex);
        if (peek(lex) == '.') {
            advance(lex);
            while (isdigit(peek(lex))) advance(lex);
        }
        int length = lex->current - start;
        memcpy(token.lexeme, &lex->source[start], length);
        token.lexeme[length] = '\0';
        token.type = TOKEN_NUMBER;
        token.number = atof(token.lexeme);
        return token;
    }
    
    // Identifiers and keywords
    if (isalpha(c) || c == '_') {
        int start = lex->current;
        while (isalnum(peek(lex)) || peek(lex) == '_') advance(lex);
        int length = lex->current - start;
        memcpy(token.lexeme, &lex->source[start], length);
        token.lexeme[length] = '\0';
        
        if (strcmp(token.lexeme, "let") == 0) {
            token.type = TOKEN_LET;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }
        return token;
    }
    
    // Single-character tokens
    advance(lex);
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    
    switch (c) {
        case '+': token.type = TOKEN_PLUS; break;
        case '-': token.type = TOKEN_MINUS; break;
        case '*': token.type = TOKEN_STAR; break;
        case '/': token.type = TOKEN_SLASH; break;
        case '=': token.type = TOKEN_EQUALS; break;
        case '(': token.type = TOKEN_LPAREN; break;
        case ')': token.type = TOKEN_RPAREN; break;
        case ';': token.type = TOKEN_SEMICOLON; break;
        default: token.type = TOKEN_ERROR; break;
    }
    
    return token;
}

// AST
typedef enum {
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_ASSIGN,      // NEW: x = value
    AST_LET          // NEW: let x = value
} ASTType;

typedef struct ASTNode {
    ASTType type;
    union {
        double number;
        char identifier[64];
        struct {
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {
            char op;
            struct ASTNode* operand;
        } unary;
        struct {
            char name[64];
            struct ASTNode* value;
        } assign;
    };
} ASTNode;

// Symbol table for variables
typedef struct {
    char name[64];
    double value;
} Variable;

typedef struct {
    Variable vars[100];
    int count;
} SymbolTable;

void table_init(SymbolTable* table) {
    table->count = 0;
}

void table_set(SymbolTable* table, const char* name, double value) {
    // Check if variable exists
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            table->vars[i].value = value;
            return;
        }
    }
    
    // Add new variable
    if (table->count >= 100) {
        printf("Error: Too many variables\n");
        exit(1);
    }
    
    strcpy(table->vars[table->count].name, name);
    table->vars[table->count].value = value;
    table->count++;
}

double* table_get(SymbolTable* table, const char* name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            return &table->vars[i].value;
        }
    }
    return NULL;
}

// Create AST nodes
ASTNode* make_number(double value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->number = value;
    return node;
}

ASTNode* make_identifier(const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    strcpy(node->identifier, name);
    return node;
}

ASTNode* make_binary(char op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;
    return node;
}

ASTNode* make_unary(char op, ASTNode* operand) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_UNARY_OP;
    node->unary.op = op;
    node->unary.operand = operand;
    return node;
}

ASTNode* make_assign(const char* name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGN;
    strcpy(node->assign.name, name);
    node->assign.value = value;
    return node;
}

ASTNode* make_let(const char* name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_LET;
    strcpy(node->assign.name, name);
    node->assign.value = value;
    return node;
}

// Parser
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

// Forward declarations
ASTNode* parse_statement(Parser* p);
ASTNode* parse_expression(Parser* p);
ASTNode* parse_assignment(Parser* p);
ASTNode* parse_term(Parser* p);
ASTNode* parse_factor(Parser* p);

ASTNode* parse_factor(Parser* p) {
    if (p->current.type == TOKEN_MINUS) {
        consume(p);
        return make_unary('-', parse_factor(p));
    }
    
    if (p->current.type == TOKEN_NUMBER) {
        Token token = consume(p);
        return make_number(token.number);
    }
    
    if (p->current.type == TOKEN_IDENTIFIER) {
        Token token = consume(p);
        return make_identifier(token.lexeme);
    }
    
    if (p->current.type == TOKEN_LPAREN) {
        consume(p);
        ASTNode* expr = parse_expression(p);
        consume(p);  // ')'
        return expr;
    }
    
    printf("Error: Unexpected token\n");
    exit(1);
}

ASTNode* parse_term(Parser* p) {
    ASTNode* left = parse_factor(p);
    
    while (p->current.type == TOKEN_STAR || p->current.type == TOKEN_SLASH) {
        Token op = consume(p);
        ASTNode* right = parse_factor(p);
        left = make_binary(op.lexeme[0], left, right);
    }
    
    return left;
}

ASTNode* parse_expression(Parser* p) {
    ASTNode* left = parse_term(p);
    
    while (p->current.type == TOKEN_PLUS || p->current.type == TOKEN_MINUS) {
        Token op = consume(p);
        ASTNode* right = parse_term(p);
        left = make_binary(op.lexeme[0], left, right);
    }
    
    return left;
}

ASTNode* parse_assignment(Parser* p) {
    // Look ahead for assignment
    if (p->current.type == TOKEN_IDENTIFIER) {
        Token name = p->current;
        int saved_pos = p->lexer.current;
        Token next = next_token(&p->lexer);
        p->lexer.current = saved_pos;
        p->current = name;
        
        if (next.type == TOKEN_EQUALS) {
            Token name_token = consume(p);
            consume(p);  // '='
            ASTNode* value = parse_expression(p);
            return make_assign(name_token.lexeme, value);
        }
    }
    
    return parse_expression(p);
}

ASTNode* parse_statement(Parser* p) {
    if (p->current.type == TOKEN_LET) {
        consume(p);  // 'let'
        Token name = consume(p);  // identifier
        consume(p);  // '='
        ASTNode* value = parse_expression(p);
        return make_let(name.lexeme, value);
    }
    
    return parse_assignment(p);
}

// Evaluate
double evaluate(ASTNode* node, SymbolTable* table) {
    switch (node->type) {
        case AST_NUMBER:
            return node->number;
            
        case AST_IDENTIFIER: {
            double* value = table_get(table, node->identifier);
            if (value == NULL) {
                printf("Error: Undefined variable '%s'\n", node->identifier);
                exit(1);
            }
            return *value;
        }
            
        case AST_BINARY_OP: {
            double left = evaluate(node->binary.left, table);
            double right = evaluate(node->binary.right, table);
            
            switch (node->binary.op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/': return left / right;
            }
        }
        
        case AST_UNARY_OP: {
            double operand = evaluate(node->unary.operand, table);
            return node->unary.op == '-' ? -operand : operand;
        }
        
        case AST_ASSIGN:
        case AST_LET: {
            double value = evaluate(node->assign.value, table);
            table_set(table, node->assign.name, value);
            return value;
        }
    }
    
    return 0;
}

// Free AST
void free_ast(ASTNode* node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case AST_BINARY_OP:
            free_ast(node->binary.left);
            free_ast(node->binary.right);
            break;
        case AST_UNARY_OP:
            free_ast(node->unary.operand);
            break;
        case AST_ASSIGN:
        case AST_LET:
            free_ast(node->assign.value);
            break;
        default:
            break;
    }
    
    free(node);
}

int main(void) {
    printf("=== Variables Example ===\n\n");
    
    SymbolTable table;
    table_init(&table);
    
    printf("Now our language supports variables!\n");
    printf("Commands:\n");
    printf("  let x = value  - Declare variable\n");
    printf("  x = value      - Assign to variable\n");
    printf("  x              - Get variable value\n");
    printf("  quit           - Exit\n\n");
    
    char line[256];
    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        
        line[strcspn(line, "\n")] = 0;
        
        if (strcmp(line, "quit") == 0 || strcmp(line, "exit") == 0) {
            break;
        }
        
        if (strlen(line) == 0) continue;
        
        Parser p;
        parser_init(&p, line);
        
        if (p.current.type == TOKEN_ERROR) {
            printf("Error: Invalid input\n");
            continue;
        }
        
        ASTNode* ast = parse_statement(&p);
        
        double result = evaluate(ast, &table);
        printf("= %.2f\n\n", result);
        
        free_ast(ast);
    }
    
    printf("\nGoodbye!\n");
    return 0;
}
