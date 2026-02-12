/*
 * Calculator with Parser and Evaluator
 * 
 * Builds on the tokenizer - now we parse expressions into a tree
 * and evaluate them with correct operator precedence.
 * 
 * Grammar:
 *   expression → term (('+' | '-') term)*
 *   term       → factor (('*' | '/') factor)*
 *   factor     → NUMBER | '-' factor | '(' expression ')'
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Token types
typedef enum {
    TOKEN_NUMBER,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
    TOKEN_LPAREN, TOKEN_RPAREN,
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
    
    advance(lex);
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    
    switch (c) {
        case '+': token.type = TOKEN_PLUS; break;
        case '-': token.type = TOKEN_MINUS; break;
        case '*': token.type = TOKEN_STAR; break;
        case '/': token.type = TOKEN_SLASH; break;
        case '(': token.type = TOKEN_LPAREN; break;
        case ')': token.type = TOKEN_RPAREN; break;
        default: token.type = TOKEN_ERROR; break;
    }
    
    return token;
}

// AST (Abstract Syntax Tree)
typedef enum {
    AST_NUMBER,
    AST_BINARY_OP,
    AST_UNARY_OP
} ASTType;

typedef struct ASTNode {
    ASTType type;
    union {
        double number;
        struct {
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary;
        struct {
            char op;
            struct ASTNode* operand;
        } unary;
    };
} ASTNode;

// Create AST nodes
ASTNode* make_number(double value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->number = value;
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
ASTNode* parse_expression(Parser* p);
ASTNode* parse_term(Parser* p);
ASTNode* parse_factor(Parser* p);

// Parse factor: NUMBER | '-' factor | '(' expression ')'
ASTNode* parse_factor(Parser* p) {
    // Unary minus
    if (p->current.type == TOKEN_MINUS) {
        consume(p);
        return make_unary('-', parse_factor(p));
    }
    
    // Number
    if (p->current.type == TOKEN_NUMBER) {
        Token token = consume(p);
        return make_number(token.number);
    }
    
    // Parentheses
    if (p->current.type == TOKEN_LPAREN) {
        consume(p);  // '('
        ASTNode* expr = parse_expression(p);
        if (p->current.type != TOKEN_RPAREN) {
            printf("Error: Expected ')'\n");
            exit(1);
        }
        consume(p);  // ')'
        return expr;
    }
    
    printf("Error: Unexpected token\n");
    exit(1);
}

// Parse term: factor (('*' | '/') factor)*
ASTNode* parse_term(Parser* p) {
    ASTNode* left = parse_factor(p);
    
    while (p->current.type == TOKEN_STAR || p->current.type == TOKEN_SLASH) {
        Token op = consume(p);
        ASTNode* right = parse_factor(p);
        left = make_binary(op.lexeme[0], left, right);
    }
    
    return left;
}

// Parse expression: term (('+' | '-') term)*
ASTNode* parse_expression(Parser* p) {
    ASTNode* left = parse_term(p);
    
    while (p->current.type == TOKEN_PLUS || p->current.type == TOKEN_MINUS) {
        Token op = consume(p);
        ASTNode* right = parse_term(p);
        left = make_binary(op.lexeme[0], left, right);
    }
    
    return left;
}

// Evaluate AST
double evaluate(ASTNode* node) {
    switch (node->type) {
        case AST_NUMBER:
            return node->number;
            
        case AST_BINARY_OP: {
            double left = evaluate(node->binary.left);
            double right = evaluate(node->binary.right);
            
            switch (node->binary.op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/':
                    if (right == 0) {
                        printf("Error: Division by zero\n");
                        exit(1);
                    }
                    return left / right;
            }
        }
        
        case AST_UNARY_OP: {
            double operand = evaluate(node->unary.operand);
            if (node->unary.op == '-') {
                return -operand;
            }
            return operand;
        }
    }
    
    return 0;
}

// Print AST (for visualization)
void print_ast(ASTNode* node, int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
    
    switch (node->type) {
        case AST_NUMBER:
            printf("%.2f\n", node->number);
            break;
            
        case AST_BINARY_OP:
            printf("%c\n", node->binary.op);
            print_ast(node->binary.left, depth + 1);
            print_ast(node->binary.right, depth + 1);
            break;
            
        case AST_UNARY_OP:
            printf("%c (unary)\n", node->unary.op);
            print_ast(node->unary.operand, depth + 1);
            break;
    }
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
        default:
            break;
    }
    
    free(node);
}

// Evaluate expression from string
void eval_and_print(const char* expr) {
    printf("Expression: %s\n", expr);
    
    Parser p;
    parser_init(&p, expr);
    ASTNode* ast = parse_expression(&p);
    
    printf("AST:\n");
    print_ast(ast, 0);
    
    double result = evaluate(ast);
    printf("Result: %.2f\n\n", result);
    
    free_ast(ast);
}

int main(void) {
    printf("=== Calculator with Parser ===\n\n");
    
    printf("This calculator parses expressions into a tree and evaluates them.\n");
    printf("Operator precedence is handled automatically!\n\n");
    
    printf("---\n\n");
    
    // Examples
    eval_and_print("2 + 3");
    eval_and_print("2 + 3 * 4");
    eval_and_print("(2 + 3) * 4");
    eval_and_print("10 / 2 + 3");
    eval_and_print("10 / (2 + 3)");
    eval_and_print("-5 + 3");
    eval_and_print("2 * -3");
    eval_and_print("(10 + 20) * (5 - 3) / 2");
    
    printf("---\n\n");
    
    // Interactive mode
    printf("=== Interactive Calculator ===\n");
    printf("Enter expressions (or 'quit' to exit)\n\n");
    
    char line[256];
    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        
        // Remove newline
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
        
        ASTNode* ast = parse_expression(&p);
        
        if (p.current.type != TOKEN_EOF) {
            printf("Error: Unexpected token after expression\n");
            free_ast(ast);
            continue;
        }
        
        double result = evaluate(ast);
        printf("= %.2f\n\n", result);
        
        free_ast(ast);
    }
    
    printf("\nGoodbye!\n");
    return 0;
}
