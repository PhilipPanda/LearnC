# Functions

## The Big Picture

Functions are reusable pieces of code. They take inputs (parameters), do work, and return outputs.

In our language:
```javascript
fn add(a, b) {
    return a + b;
}

let result = add(10, 20);  // 30
```

We need to handle:
- Function definitions
- Function calls
- Parameters
- Return values
- Recursion

## Function as a Value

Functions are **first-class values** - can be stored in variables, passed as arguments, returned from other functions.

```c
typedef struct {
    char** params;           // Parameter names
    int param_count;
    ASTNode* body;           // Function body
    Scope* closure;          // Captured environment
} Function;

typedef enum {
    VAL_NUMBER,
    VAL_STRING,
    VAL_FUNCTION,           // NEW!
    VAL_NIL
} ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        char* string;
        Function* function;
    };
} Value;

Value make_function(char** params, int param_count, ASTNode* body, Scope* closure) {
    Function* fn = malloc(sizeof(Function));
    fn->params = params;
    fn->param_count = param_count;
    fn->body = body;
    fn->closure = closure;
    
    Value val;
    val.type = VAL_FUNCTION;
    val.function = fn;
    return val;
}
```

## AST Nodes

```c
typedef struct {
    char* name;
    char** params;
    int param_count;
    ASTNode* body;
} FunctionDef;

typedef struct {
    ASTNode* callee;         // What to call (usually identifier)
    ASTNode** args;          // Arguments
    int arg_count;
} FunctionCall;

typedef enum {
    // ...
    AST_FUNCTION_DEF,
    AST_FUNCTION_CALL,
    AST_RETURN,
} ASTType;
```

## Parsing Function Definition

```javascript
fn add(a, b) {
    return a + b;
}
```

```c
ASTNode* parse_function_def(Parser* p) {
    consume(p, TOKEN_KEYWORD, "fn");
    
    // Function name
    Token name = consume(p, TOKEN_IDENTIFIER, "Expected function name");
    
    // Parameters
    consume(p, TOKEN_LPAREN, "(");
    
    char** params = NULL;
    int param_count = 0;
    
    if (p->current.type != TOKEN_RPAREN) {
        do {
            Token param = consume(p, TOKEN_IDENTIFIER, "Expected parameter name");
            params = realloc(params, (param_count + 1) * sizeof(char*));
            params[param_count++] = strdup(param.lexeme);
        } while (match(p, TOKEN_COMMA));
    }
    
    consume(p, TOKEN_RPAREN, ")");
    
    // Body
    ASTNode* body = parse_block(p);
    
    // Create AST node
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_DEF;
    node->function_def.name = strdup(name.lexeme);
    node->function_def.params = params;
    node->function_def.param_count = param_count;
    node->function_def.body = body;
    
    return node;
}
```

## Parsing Function Call

```javascript
add(10, 20)
```

```c
ASTNode* parse_call(Parser* p, ASTNode* callee) {
    consume(p, TOKEN_LPAREN, "(");
    
    ASTNode** args = NULL;
    int arg_count = 0;
    
    if (p->current.type != TOKEN_RPAREN) {
        do {
            args = realloc(args, (arg_count + 1) * sizeof(ASTNode*));
            args[arg_count++] = parse_expression(p);
        } while (match(p, TOKEN_COMMA));
    }
    
    consume(p, TOKEN_RPAREN, ")");
    
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_CALL;
    node->call.callee = callee;
    node->call.args = args;
    node->call.arg_count = arg_count;
    
    return node;
}
```

## Evaluation: Defining Function

```c
Value evaluate(ASTNode* node, Interpreter* interp) {
    switch (node->type) {
        case AST_FUNCTION_DEF: {
            // Create function value
            Value fn = make_function(
                node->function_def.params,
                node->function_def.param_count,
                node->function_def.body,
                interp->current_scope  // Capture current scope (closure)
            );
            
            // Store in current scope
            scope_set(interp->current_scope, node->function_def.name, fn);
            
            return fn;
        }
    }
}
```

## Evaluation: Calling Function

```c
Value call_function(Function* fn, Value* args, int arg_count, Interpreter* interp) {
    // Check argument count
    if (arg_count != fn->param_count) {
        runtime_error("Expected %d arguments, got %d", fn->param_count, arg_count);
    }
    
    // Create new scope for function (child of closure scope)
    Scope* old_scope = interp->current_scope;
    interp->current_scope = scope_new(fn->closure);
    
    // Bind parameters to arguments
    for (int i = 0; i < fn->param_count; i++) {
        scope_set(interp->current_scope, fn->params[i], args[i]);
    }
    
    // Execute function body
    Value result = evaluate(fn->body, interp);
    
    // Restore previous scope
    Scope* fn_scope = interp->current_scope;
    interp->current_scope = old_scope;
    scope_free(fn_scope);
    
    return result;
}

Value evaluate(ASTNode* node, Interpreter* interp) {
    switch (node->type) {
        case AST_FUNCTION_CALL: {
            // Evaluate callee (usually just an identifier)
            Value callee = evaluate(node->call.callee, interp);
            
            if (callee.type != VAL_FUNCTION) {
                runtime_error("Cannot call non-function");
            }
            
            // Evaluate arguments
            Value* args = malloc(node->call.arg_count * sizeof(Value));
            for (int i = 0; i < node->call.arg_count; i++) {
                args[i] = evaluate(node->call.args[i], interp);
            }
            
            // Call function
            Value result = call_function(callee.function, args, node->call.arg_count, interp);
            
            free(args);
            return result;
        }
    }
}
```

## Return Statement

```c
typedef struct {
    Value value;
    int has_value;
} ReturnValue;

// Use setjmp/longjmp or exceptions for early return
// Or pass return value through special mechanism

Value evaluate_block(ASTNode* block, Interpreter* interp, ReturnValue* ret) {
    Value result = make_nil();
    
    for (int i = 0; i < block->block.count; i++) {
        result = evaluate(block->block.statements[i], interp, ret);
        
        if (ret->has_value) {
            return ret->value;  // Early return!
        }
    }
    
    return result;
}

Value evaluate(ASTNode* node, Interpreter* interp, ReturnValue* ret) {
    switch (node->type) {
        case AST_RETURN: {
            Value value = make_nil();
            if (node->return_stmt.value != NULL) {
                value = evaluate(node->return_stmt.value, interp, ret);
            }
            
            ret->has_value = 1;
            ret->value = value;
            return value;
        }
    }
}
```

## Recursion

Just works! The scope stack naturally handles it:

```javascript
fn factorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

print(factorial(5));  // 120
```

Each recursive call gets its own scope with its own `n` parameter.

## Call Stack

```
factorial(5)
    factorial(4)
        factorial(3)
            factorial(2)
                factorial(1)
                    return 1
                return 2 * 1 = 2
            return 3 * 2 = 6
        return 4 * 6 = 24
    return 5 * 24 = 120
```

Each call frame has its own:
- Parameters
- Local variables
- Return address

## Built-in Functions

Native C functions exposed to script:

```c
Value builtin_print(Value* args, int arg_count) {
    for (int i = 0; i < arg_count; i++) {
        print_value(args[i]);
        if (i < arg_count - 1) printf(" ");
    }
    printf("\n");
    return make_nil();
}

Value builtin_len(Value* args, int arg_count) {
    if (arg_count != 1) runtime_error("len() takes 1 argument");
    if (args[0].type != VAL_STRING) runtime_error("len() requires string");
    return make_number(strlen(args[0].string));
}

// Register built-ins
void init_builtins(Scope* global) {
    scope_set(global, "print", make_builtin(builtin_print));
    scope_set(global, "len", make_builtin(builtin_len));
}
```

## Variadic Functions

Functions that take variable number of arguments:

```javascript
fn sum(args...) {
    let total = 0;
    for (let i = 0; i < len(args); i = i + 1) {
        total = total + args[i];
    }
    return total;
}

print(sum(1, 2, 3, 4));  // 10
```

Implementation stores extra args in array.

## Anonymous Functions (Lambdas)

```javascript
let add = fn(a, b) { return a + b; };

let numbers = [1, 2, 3, 4];
let doubled = map(numbers, fn(x) { return x * 2; });
```

Same as named functions, just no name binding.

## Higher-Order Functions

Functions that take or return functions:

```javascript
fn makeMultiplier(factor) {
    return fn(x) {
        return x * factor;
    };
}

let double = makeMultiplier(2);
let triple = makeMultiplier(3);

print(double(5));   // 10
print(triple(5));   // 15
```

The returned function "closes over" the `factor` variable.

## Tail Call Optimization

A tail call is a function call that's the last thing a function does:

```javascript
fn factorial(n, acc) {
    if (n <= 1) return acc;
    return factorial(n - 1, n * acc);  // Tail call!
}
```

Optimization: Reuse current stack frame instead of creating new one.

```c
// Detect tail calls
int is_tail_call(ASTNode* node) {
    // Last statement in function?
    // Is a return?
    // Return value is function call?
    return /* ... */;
}

// Instead of recursive call, loop
Value call_function(Function* fn, Value* args, int arg_count) {
    while (1) {
        // Set up parameters
        // Execute body
        // If tail call: update args, continue loop
        // Else: return result
    }
}
```

## Summary

Functions are values. Define with `fn`, call with `()`. Each call creates a new scope. Return statements exit early. Closures capture enclosing scope.

Recursion works naturally. Built-ins bridge script and C. Higher-order functions enable powerful abstractions.
