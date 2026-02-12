# Evaluation

## The Big Picture

We've parsed the code into a tree. Now we need to **execute** it - walk the tree and compute results.

This is called **evaluation** or **interpretation**. We traverse the AST and perform the operations.

## Tree Walking Interpreter

Recursively evaluate the AST:

```c
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
                case '/': return left / right;
            }
        }
        
        case AST_UNARY_OP: {
            double operand = evaluate(node->unary.operand);
            
            switch (node->unary.op) {
                case '-': return -operand;
                case '+': return operand;
            }
        }
    }
    
    return 0;
}
```

## How It Works

For expression `2 + 3 * 4`:

```
Tree:
       +
      / \
     2   *
        / \
       3   4

Evaluation:
1. evaluate(+)
   - Evaluate left: 2 → 2
   - Evaluate right: *
     - Evaluate left: 3 → 3
     - Evaluate right: 4 → 4
     - 3 * 4 → 12
   - 2 + 12 → 14
```

The tree structure naturally gives us correct precedence!

## Operator Precedence

Different operators have different **binding strength**:

```
Highest:  ()          (parentheses)
          -x          (unary minus)
          * /         (multiplication, division)
          + -         (addition, subtraction)
Lowest:   = ==        (assignment, comparison)
```

Our parser handles this by having **layers** of parsing functions:

```c
// Lowest precedence (assignment)
ASTNode* parse_assignment() {
    ASTNode* left = parse_comparison();
    if (match('=')) {
        ASTNode* right = parse_assignment();
        return make_assign(left, right);
    }
    return left;
}

// Comparison
ASTNode* parse_comparison() {
    ASTNode* left = parse_term();
    while (match('==') || match('<') || match('>')) {
        char op = previous_op();
        ASTNode* right = parse_term();
        left = make_binary(op, left, right);
    }
    return left;
}

// Addition and subtraction
ASTNode* parse_term() {
    ASTNode* left = parse_factor();
    while (match('+') || match('-')) {
        char op = previous_op();
        ASTNode* right = parse_factor();
        left = make_binary(op, left, right);
    }
    return left;
}

// Multiplication and division
ASTNode* parse_factor() {
    ASTNode* left = parse_unary();
    while (match('*') || match('/')) {
        char op = previous_op();
        ASTNode* right = parse_unary();
        left = make_binary(op, left, right);
    }
    return left;
}

// Unary operators
ASTNode* parse_unary() {
    if (match('-') || match('!')) {
        char op = previous_op();
        ASTNode* right = parse_unary();
        return make_unary(op, right);
    }
    return parse_primary();
}

// Highest precedence (literals, identifiers, parentheses)
ASTNode* parse_primary() {
    if (match(NUMBER)) return make_number(previous_token().number);
    if (match(IDENTIFIER)) return make_identifier(previous_token().lexeme);
    if (match('(')) {
        ASTNode* expr = parse_expression();
        consume(')');
        return expr;
    }
}
```

Each level calls the **next higher precedence** level.

## Short-Circuit Evaluation

For logical operators, don't evaluate both sides unnecessarily:

```c
// AND: false && anything → false (don't evaluate right side)
// OR: true || anything → true (don't evaluate right side)

double evaluate_logical(ASTNode* node) {
    if (node->binary.op == AND) {
        double left = evaluate(node->binary.left);
        if (!left) return 0;  // Short circuit!
        return evaluate(node->binary.right);
    }
    
    if (node->binary.op == OR) {
        double left = evaluate(node->binary.left);
        if (left) return 1;  // Short circuit!
        return evaluate(node->binary.right);
    }
}
```

## Type System

Simple scripting languages often use **dynamic typing**:

```c
typedef enum {
    VAL_NUMBER,
    VAL_STRING,
    VAL_BOOL,
    VAL_NIL
} ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        char* string;
        int boolean;
    };
} Value;

Value add_values(Value a, Value b) {
    if (a.type == VAL_NUMBER && b.type == VAL_NUMBER) {
        return make_number(a.number + b.number);
    }
    if (a.type == VAL_STRING || b.type == VAL_STRING) {
        // String concatenation
        return make_string(concat(to_string(a), to_string(b)));
    }
    // Type error!
}
```

## Truthiness

What counts as "true" vs "false"?

```c
int is_truthy(Value val) {
    switch (val.type) {
        case VAL_BOOL: return val.boolean;
        case VAL_NIL: return 0;
        case VAL_NUMBER: return val.number != 0;
        case VAL_STRING: return strlen(val.string) > 0;
    }
}
```

## Error Handling

Runtime errors need clear messages:

```c
Value evaluate(ASTNode* node) {
    switch (node->type) {
        case AST_BINARY_OP: {
            Value left = evaluate(node->binary.left);
            Value right = evaluate(node->binary.right);
            
            if (node->binary.op == '/') {
                if (right.type != VAL_NUMBER) {
                    runtime_error("Divisor must be a number");
                }
                if (right.number == 0) {
                    runtime_error("Division by zero");
                }
                return make_number(left.number / right.number);
            }
        }
    }
}

void runtime_error(const char* message) {
    fprintf(stderr, "Runtime error: %s\n", message);
    exit(1);
}
```

## Optimization Hints

**Constant folding:** Evaluate constants at parse time:

```c
// "2 + 3" → just store 5 in the tree
ASTNode* parse_addition(Parser* p) {
    ASTNode* left = parse_term(p);
    
    if (match('+')) {
        ASTNode* right = parse_term(p);
        
        // Both constants? Fold!
        if (left->type == AST_NUMBER && right->type == AST_NUMBER) {
            ASTNode* result = make_number(left->number + right->number);
            free(left);
            free(right);
            return result;
        }
        
        return make_binary('+', left, right);
    }
    
    return left;
}
```

**Tail call optimization:** Reuse stack frames for recursive calls at end of function.

**Bytecode compilation:** Instead of walking AST, compile to bytecode for faster execution.

## Performance

Tree walking is **slow** but **simple**:
- Each node allocation costs time
- Recursion overhead
- Cache unfriendly (lots of pointers)

**Faster alternatives:**
- **Bytecode VM:** Compile AST to bytecode, run in virtual machine
- **JIT compilation:** Compile hot code paths to machine code
- **AOT compilation:** Compile entire program ahead of time

For learning, tree walking is perfect. For production, use bytecode.

## Complete Example

```c
// Parse: "2 + 3 * 4"
Parser p;
parser_init(&p, "2 + 3 * 4");
ASTNode* ast = parse_expression(&p);

// Evaluate
double result = evaluate(ast);
printf("Result: %.2f\n", result);  // 14.00

// Cleanup
free_ast(ast);
```

## Summary

Evaluation walks the AST and computes results. The tree structure gives us:
- Correct operator precedence (from parsing)
- Natural recursion (expressions contain expressions)
- Easy to implement (just switch on node type)

Next: Add variables so we can store and reuse values!
