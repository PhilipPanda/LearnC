/*
 * Control Flow Example
 * 
 * Adds if statements and while loops to our language.
 * Now we can write real programs with conditional logic!
 * 
 * Syntax:
 *   if (condition) { statements }
 *   while (condition) { statements }
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TOKEN_NUMBER, TOKEN_IDENTIFIER,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
    TOKEN_EQUALS, TOKEN_LESS, TOKEN_GREATER,
    TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_SEMICOLON, TOKEN_COMMA,
    TOKEN_LET, TOKEN_IF, TOKEN_WHILE, TOKEN_PRINT,
    TOKEN_EOF, TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[64];
    double number;
} Token;

// Lexer (simplified)
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
    if (isdigit(c)) {
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
        
        if (strcmp(token.lexeme, "let") == 0) token.type = TOKEN_LET;
        else if (strcmp(token.lexeme, "if") == 0) token.type = TOKEN_IF;
        else if (strcmp(token.lexeme, "while") == 0) token.type = TOKEN_WHILE;
        else if (strcmp(token.lexeme, "print") == 0) token.type = TOKEN_PRINT;
        else token.type = TOKEN_IDENTIFIER;
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
        case '<': token.type = TOKEN_LESS; break;
        case '>': token.type = TOKEN_GREATER; break;
        case '(': token.type = TOKEN_LPAREN; break;
        case ')': token.type = TOKEN_RPAREN; break;
        case '{': token.type = TOKEN_LBRACE; break;
        case '}': token.type = TOKEN_RBRACE; break;
        case ';': token.type = TOKEN_SEMICOLON; break;
        case ',': token.type = TOKEN_COMMA; break;
        default: token.type = TOKEN_ERROR; break;
    }
    
    return token;
}

// Symbol table
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
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            table->vars[i].value = value;
            return;
        }
    }
    strcpy(table->vars[table->count].name, name);
    table->vars[table->count].value = value;
    table->count++;
}

double table_get(SymbolTable* table, const char* name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            return table->vars[i].value;
        }
    }
    printf("Error: Undefined variable '%s'\n", name);
    exit(1);
}

// AST
typedef enum {
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_BINARY_OP,
    AST_ASSIGN,
    AST_PRINT,
    AST_IF,
    AST_WHILE,
    AST_BLOCK
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
            char name[64];
            struct ASTNode* value;
        } assign;
        struct {
            struct ASTNode* expr;
        } print;
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } if_stmt;
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_stmt;
        struct {
            struct ASTNode** stmts;
            int count;
        } block;
    };
} ASTNode;

// Create nodes
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

ASTNode* make_assign(const char* name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGN;
    strcpy(node->assign.name, name);
    node->assign.value = value;
    return node;
}

ASTNode* make_print(ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_PRINT;
    node->print.expr = expr;
    return node;
}

ASTNode* make_if(ASTNode* condition, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF;
    node->if_stmt.condition = condition;
    node->if_stmt.body = body;
    return node;
}

ASTNode* make_while(ASTNode* condition, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_WHILE;
    node->while_stmt.condition = condition;
    node->while_stmt.body = body;
    return node;
}

ASTNode* make_block() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    node->block.stmts = NULL;
    node->block.count = 0;
    return node;
}

void block_add(ASTNode* block, ASTNode* stmt) {
    block->block.stmts = realloc(block->block.stmts, (block->block.count + 1) * sizeof(ASTNode*));
    block->block.stmts[block->block.count++] = stmt;
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
ASTNode* parse_comparison(Parser* p);
ASTNode* parse_term(Parser* p);
ASTNode* parse_factor(Parser* p);

ASTNode* parse_factor(Parser* p) {
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

ASTNode* parse_comparison(Parser* p) {
    ASTNode* left = parse_term(p);
    
    while (p->current.type == TOKEN_PLUS || p->current.type == TOKEN_MINUS ||
           p->current.type == TOKEN_LESS || p->current.type == TOKEN_GREATER) {
        Token op = consume(p);
        ASTNode* right = parse_term(p);
        left = make_binary(op.lexeme[0], left, right);
    }
    
    return left;
}

ASTNode* parse_expression(Parser* p) {
    return parse_comparison(p);
}

ASTNode* parse_block(Parser* p) {
    consume(p);  // '{'
    
    ASTNode* block = make_block();
    
    while (p->current.type != TOKEN_RBRACE && p->current.type != TOKEN_EOF) {
        block_add(block, parse_statement(p));
    }
    
    consume(p);  // '}'
    return block;
}

ASTNode* parse_statement(Parser* p) {
    // let x = value;
    if (p->current.type == TOKEN_LET) {
        consume(p);
        Token name = consume(p);
        consume(p);  // '='
        ASTNode* value = parse_expression(p);
        consume(p);  // ';'
        return make_assign(name.lexeme, value);
    }
    
    // print(expr);
    if (p->current.type == TOKEN_PRINT) {
        consume(p);
        consume(p);  // '('
        ASTNode* expr = parse_expression(p);
        consume(p);  // ')'
        consume(p);  // ';'
        return make_print(expr);
    }
    
    // if (condition) { ... }
    if (p->current.type == TOKEN_IF) {
        consume(p);
        consume(p);  // '('
        ASTNode* condition = parse_expression(p);
        consume(p);  // ')'
        ASTNode* body = parse_block(p);
        return make_if(condition, body);
    }
    
    // while (condition) { ... }
    if (p->current.type == TOKEN_WHILE) {
        consume(p);
        consume(p);  // '('
        ASTNode* condition = parse_expression(p);
        consume(p);  // ')'
        ASTNode* body = parse_block(p);
        return make_while(condition, body);
    }
    
    // x = value;
    if (p->current.type == TOKEN_IDENTIFIER) {
        Token name = consume(p);
        consume(p);  // '='
        ASTNode* value = parse_expression(p);
        consume(p);  // ';'
        return make_assign(name.lexeme, value);
    }
    
    printf("Error: Unexpected statement\n");
    exit(1);
}

// Evaluate
double evaluate(ASTNode* node, SymbolTable* table) {
    switch (node->type) {
        case AST_NUMBER:
            return node->number;
            
        case AST_IDENTIFIER:
            return table_get(table, node->identifier);
            
        case AST_BINARY_OP: {
            double left = evaluate(node->binary.left, table);
            double right = evaluate(node->binary.right, table);
            
            switch (node->binary.op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/': return left / right;
                case '<': return left < right;
                case '>': return left > right;
            }
        }
        
        case AST_ASSIGN: {
            double value = evaluate(node->assign.value, table);
            table_set(table, node->assign.name, value);
            return value;
        }
        
        case AST_PRINT: {
            double value = evaluate(node->print.expr, table);
            printf("%.2f\n", value);
            return value;
        }
        
        case AST_IF: {
            double cond = evaluate(node->if_stmt.condition, table);
            if (cond != 0) {
                return evaluate(node->if_stmt.body, table);
            }
            return 0;
        }
        
        case AST_WHILE: {
            double result = 0;
            while (evaluate(node->while_stmt.condition, table) != 0) {
                result = evaluate(node->while_stmt.body, table);
            }
            return result;
        }
        
        case AST_BLOCK: {
            double result = 0;
            for (int i = 0; i < node->block.count; i++) {
                result = evaluate(node->block.stmts[i], table);
            }
            return result;
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
        case AST_ASSIGN:
            free_ast(node->assign.value);
            break;
        case AST_PRINT:
            free_ast(node->print.expr);
            break;
        case AST_IF:
            free_ast(node->if_stmt.condition);
            free_ast(node->if_stmt.body);
            break;
        case AST_WHILE:
            free_ast(node->while_stmt.condition);
            free_ast(node->while_stmt.body);
            break;
        case AST_BLOCK:
            for (int i = 0; i < node->block.count; i++) {
                free_ast(node->block.stmts[i]);
            }
            free(node->block.stmts);
            break;
        default:
            break;
    }
    
    free(node);
}

int main(void) {
    printf("=== Control Flow Example ===\n\n");
    
    SymbolTable table;
    table_init(&table);
    
    printf("Example 1: Countdown with while loop\n\n");
    const char* program1 = 
        "let x = 5;\n"
        "while (x > 0) {\n"
        "    print(x);\n"
        "    x = x - 1;\n"
        "}\n"
        "print(0);";
    
    printf("Program:\n%s\n\nOutput:\n", program1);
    Parser p1;
    parser_init(&p1, program1);
    
    while (p1.current.type != TOKEN_EOF) {
        ASTNode* stmt = parse_statement(&p1);
        evaluate(stmt, &table);
        free_ast(stmt);
    }
    
    printf("\n---\n\n");
    
    // Reset table
    table_init(&table);
    
    printf("Example 2: Conditional logic\n\n");
    const char* program2 =
        "let score = 85;\n"
        "if (score > 90) {\n"
        "    print(1);\n"
        "}\n"
        "if (score > 60) {\n"
        "    print(2);\n"
        "}\n"
        "print(3);";
    
    printf("Program:\n%s\n\nOutput:\n", program2);
    Parser p2;
    parser_init(&p2, program2);
    
    while (p2.current.type != TOKEN_EOF) {
        ASTNode* stmt = parse_statement(&p2);
        evaluate(stmt, &table);
        free_ast(stmt);
    }
    
    printf("\n---\n\n");
    
    // Reset table
    table_init(&table);
    
    printf("Example 3: Sum numbers 1 to 10\n\n");
    const char* program3 =
        "let sum = 0;\n"
        "let i = 1;\n"
        "while (i < 11) {\n"
        "    sum = sum + i;\n"
        "    i = i + 1;\n"
        "}\n"
        "print(sum);";
    
    printf("Program:\n%s\n\nOutput:\n", program3);
    Parser p3;
    parser_init(&p3, program3);
    
    while (p3.current.type != TOKEN_EOF) {
        ASTNode* stmt = parse_statement(&p3);
        evaluate(stmt, &table);
        free_ast(stmt);
    }
    
    printf("\n=== Summary ===\n");
    printf("Our language now has:\n");
    printf("  - Variables (let, assignment)\n");
    printf("  - Math expressions (+, -, *, /)\n");
    printf("  - Comparisons (<, >)\n");
    printf("  - If statements\n");
    printf("  - While loops\n");
    printf("  - Print statements\n\n");
    
    printf("This is a real programming language!\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    return 0;
}
