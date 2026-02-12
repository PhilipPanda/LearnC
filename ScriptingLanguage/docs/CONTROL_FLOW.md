# Control Flow

## The Big Picture

Programs need to make decisions and repeat actions. Control flow statements let us:
- Branch (if/else)
- Loop (while, for)
- Exit early (break, continue, return)

## If Statement

```javascript
if (condition) {
    // Execute if true
} else {
    // Execute if false
}
```

### AST Node

```c
typedef struct {
    ASTNode* condition;
    ASTNode* then_branch;
    ASTNode* else_branch;  // Optional
} IfStatement;

typedef enum {
    // ...
    AST_IF_STATEMENT,
} ASTType;
```

### Parsing

```c
ASTNode* parse_if_statement(Parser* p) {
    consume(p, TOKEN_KEYWORD, "if");
    consume(p, TOKEN_LPAREN, "(");
    
    ASTNode* condition = parse_expression(p);
    
    consume(p, TOKEN_RPAREN, ")");
    
    ASTNode* then_branch = parse_statement(p);
    
    ASTNode* else_branch = NULL;
    if (match(p, TOKEN_KEYWORD) && strcmp(p->previous.lexeme, "else") == 0) {
        else_branch = parse_statement(p);
    }
    
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF_STATEMENT;
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;
    
    return node;
}
```

### Evaluation

```c
Value evaluate(ASTNode* node, Interpreter* interp) {
    switch (node->type) {
        case AST_IF_STATEMENT: {
            Value cond = evaluate(node->if_stmt.condition, interp);
            
            if (is_truthy(cond)) {
                return evaluate(node->if_stmt.then_branch, interp);
            } else if (node->if_stmt.else_branch != NULL) {
                return evaluate(node->if_stmt.else_branch, interp);
            }
            
            return make_nil();
        }
    }
}

int is_truthy(Value val) {
    switch (val.type) {
        case VAL_BOOL: return val.boolean;
        case VAL_NIL: return 0;
        case VAL_NUMBER: return val.number != 0;
        default: return 1;
    }
}
```

## While Loop

```javascript
while (condition) {
    // Execute repeatedly while true
}
```

### AST Node

```c
typedef struct {
    ASTNode* condition;
    ASTNode* body;
} WhileLoop;
```

### Evaluation

```c
Value evaluate(ASTNode* node, Interpreter* interp) {
    switch (node->type) {
        case AST_WHILE_LOOP: {
            while (1) {
                Value cond = evaluate(node->while_loop.condition, interp);
                
                if (!is_truthy(cond)) {
                    break;
                }
                
                evaluate(node->while_loop.body, interp);
            }
            
            return make_nil();
        }
    }
}
```

## For Loop

```javascript
for (initializer; condition; increment) {
    // Execute repeatedly
}
```

Equivalent to:
```javascript
{
    initializer;
    while (condition) {
        body;
        increment;
    }
}
```

### Desugaring

Convert `for` into `while` during parsing:

```c
ASTNode* parse_for_loop(Parser* p) {
    consume(p, TOKEN_KEYWORD, "for");
    consume(p, TOKEN_LPAREN, "(");
    
    // Initializer (let i = 0)
    ASTNode* init = NULL;
    if (!match(p, TOKEN_SEMICOLON)) {
        init = parse_statement(p);
    }
    
    // Condition (i < 10)
    ASTNode* cond = NULL;
    if (!match(p, TOKEN_SEMICOLON)) {
        cond = parse_expression(p);
        consume(p, TOKEN_SEMICOLON, ";");
    }
    
    // Increment (i = i + 1)
    ASTNode* incr = NULL;
    if (!match(p, TOKEN_RPAREN)) {
        incr = parse_expression(p);
        consume(p, TOKEN_RPAREN, ")");
    }
    
    // Body
    ASTNode* body = parse_statement(p);
    
    // Desugar: for → while
    // {
    //     init;
    //     while (cond) {
    //         body;
    //         incr;
    //     }
    // }
    
    ASTNode* while_body = make_block();
    block_add(while_body, body);
    if (incr) block_add(while_body, incr);
    
    ASTNode* while_loop = make_while(cond ? cond : make_bool(1), while_body);
    
    ASTNode* block = make_block();
    if (init) block_add(block, init);
    block_add(block, while_loop);
    
    return block;
}
```

## Break and Continue

```javascript
while (condition) {
    if (should_skip) continue;  // Skip to next iteration
    if (should_exit) break;     // Exit loop
}
```

### Implementation with Exceptions

```c
typedef enum {
    FLOW_NORMAL,
    FLOW_BREAK,
    FLOW_CONTINUE,
    FLOW_RETURN
} FlowControl;

typedef struct {
    FlowControl type;
    Value value;  // For return statements
} FlowResult;

FlowResult evaluate(ASTNode* node, Interpreter* interp) {
    switch (node->type) {
        case AST_BREAK:
            return (FlowResult){ FLOW_BREAK, make_nil() };
            
        case AST_CONTINUE:
            return (FlowResult){ FLOW_CONTINUE, make_nil() };
            
        case AST_WHILE_LOOP: {
            while (1) {
                Value cond = evaluate_expr(node->while_loop.condition, interp);
                if (!is_truthy(cond)) break;
                
                FlowResult result = evaluate(node->while_loop.body, interp);
                
                if (result.type == FLOW_BREAK) {
                    break;
                } else if (result.type == FLOW_CONTINUE) {
                    continue;
                } else if (result.type == FLOW_RETURN) {
                    return result;  // Propagate up
                }
            }
            
            return (FlowResult){ FLOW_NORMAL, make_nil() };
        }
    }
}
```

## Switch Statement

```javascript
switch (expr) {
    case 1:
        print("one");
        break;
    case 2:
        print("two");
        break;
    default:
        print("other");
}
```

Can desugar to if-else chain:

```javascript
{
    let _temp = expr;
    if (_temp == 1) {
        print("one");
    } else if (_temp == 2) {
        print("two");
    } else {
        print("other");
    }
}
```

## Ternary Operator

```javascript
let result = condition ? value_if_true : value_if_false;
```

Just special syntax for if-else:

```c
Value evaluate(ASTNode* node, Interpreter* interp) {
    switch (node->type) {
        case AST_TERNARY: {
            Value cond = evaluate(node->ternary.condition, interp);
            
            if (is_truthy(cond)) {
                return evaluate(node->ternary.true_branch, interp);
            } else {
                return evaluate(node->ternary.false_branch, interp);
            }
        }
    }
}
```

## Short-Circuit Evaluation

```javascript
// AND: Stop at first false
if (x != 0 && 10 / x > 5) { }

// OR: Stop at first true  
if (x == 0 || 10 / x > 5) { }
```

Don't evaluate second operand if first determines result:

```c
Value evaluate_logical(ASTNode* node, Interpreter* interp) {
    if (node->binary.op == AND) {
        Value left = evaluate(node->binary.left, interp);
        if (!is_truthy(left)) return left;  // Short circuit!
        return evaluate(node->binary.right, interp);
    }
    
    if (node->binary.op == OR) {
        Value left = evaluate(node->binary.left, interp);
        if (is_truthy(left)) return left;  // Short circuit!
        return evaluate(node->binary.right, interp);
    }
}
```

## Pattern Matching (Advanced)

```javascript
match (value) {
    [x, y] => x + y,
    { name, age } => name,
    x if x > 10 => "big",
    _ => "default"
}
```

Requires:
- Destructuring
- Guards (if conditions)
- Exhaustiveness checking

Complex to implement, but powerful!

## Optimization: Jump Tables

For switch with many cases, use jump table instead of if-else chain:

```c
// switch (x) { case 0: ..., case 1: ..., case 2: ... }

void* jump_table[] = {
    &&case_0,
    &&case_1,
    &&case_2
};

goto *jump_table[x];

case_0:
    // ...
    goto end;
    
case_1:
    // ...
    goto end;
    
case_2:
    // ...
    
end:
```

Much faster for dense cases (0, 1, 2, 3...).

## Loop Optimization

**Loop unrolling:** Duplicate loop body to reduce branching:

```c
// Instead of:
for (int i = 0; i < 100; i++) {
    process(i);
}

// Unroll:
for (int i = 0; i < 100; i += 4) {
    process(i);
    process(i + 1);
    process(i + 2);
    process(i + 3);
}
```

**Loop invariant code motion:** Move constant expressions outside loop:

```c
// Bad:
for (int i = 0; i < 100; i++) {
    x = y * z;  // Computed 100 times!
    arr[i] = i + x;
}

// Good:
int x = y * z;  // Computed once
for (int i = 0; i < 100; i++) {
    arr[i] = i + x;
}
```

## Summary

**If/else:** Conditional branching  
**While:** Loop while condition is true  
**For:** Loop with initialization, condition, increment (desugars to while)  
**Break:** Exit loop early  
**Continue:** Skip to next iteration  
**Return:** Exit function early

Control flow changes program execution order. Implementation uses conditional evaluation and flow control flags.

Short-circuit evaluation prevents unnecessary computation and runtime errors.
