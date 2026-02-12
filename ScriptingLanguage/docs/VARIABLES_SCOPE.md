# Variables & Scope

## The Big Picture

Variables let us store values and reuse them. But where do we store them? And how do we handle blocks and functions having their own variables?

Enter: **symbol tables** and **scoping**.

## Symbol Table

A map from names to values:

```c
typedef struct {
    char* name;
    Value value;
} Variable;

typedef struct {
    Variable* vars;
    int count;
    int capacity;
} SymbolTable;

void table_init(SymbolTable* table) {
    table->vars = NULL;
    table->count = 0;
    table->capacity = 0;
}

void table_set(SymbolTable* table, const char* name, Value value) {
    // Check if variable exists
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            table->vars[i].value = value;
            return;
        }
    }
    
    // Add new variable
    if (table->count >= table->capacity) {
        table->capacity = table->capacity < 8 ? 8 : table->capacity * 2;
        table->vars = realloc(table->vars, table->capacity * sizeof(Variable));
    }
    
    table->vars[table->count].name = strdup(name);
    table->vars[table->count].value = value;
    table->count++;
}

Value* table_get(SymbolTable* table, const char* name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->vars[i].name, name) == 0) {
            return &table->vars[i].value;
        }
    }
    return NULL;  // Variable not found
}
```

## Usage

```c
SymbolTable globals;
table_init(&globals);

// let x = 10;
table_set(&globals, "x", make_number(10));

// let y = x * 2;
Value* x = table_get(&globals, "x");
if (x == NULL) {
    error("Undefined variable: x");
}
table_set(&globals, "y", make_number(x->number * 2));

// y
Value* y = table_get(&globals, "y");
printf("y = %.2f\n", y->number);  // 20.00
```

## Evaluating Variables

Add to AST:

```c
typedef enum {
    AST_NUMBER,
    AST_IDENTIFIER,      // NEW: variable reference
    AST_ASSIGN,          // NEW: variable assignment
    AST_BINARY_OP,
    // ...
} ASTType;

Value evaluate(ASTNode* node, SymbolTable* table) {
    switch (node->type) {
        case AST_IDENTIFIER: {
            Value* val = table_get(table, node->identifier);
            if (val == NULL) {
                runtime_error("Undefined variable: %s", node->identifier);
            }
            return *val;
        }
        
        case AST_ASSIGN: {
            Value value = evaluate(node->assign.value, table);
            table_set(table, node->assign.name, value);
            return value;
        }
        
        // ... other cases
    }
}
```

## Scope

Each block gets its own scope:

```javascript
let x = 10;        // Global scope

{
    let x = 20;    // Inner scope (shadows outer x)
    print(x);      // 20
}

print(x);          // 10 (back to outer scope)
```

### Scope Chain

Scopes form a chain:

```
Global scope: x = 10
    ↑
Inner scope: x = 20, y = 30
    ↑
Innermost scope: z = 40
```

Lookup searches from innermost to outermost.

### Implementation

```c
typedef struct Scope {
    SymbolTable table;
    struct Scope* parent;
} Scope;

Scope* scope_new(Scope* parent) {
    Scope* scope = malloc(sizeof(Scope));
    table_init(&scope->table);
    scope->parent = parent;
    return scope;
}

void scope_set(Scope* scope, const char* name, Value value) {
    table_set(&scope->table, name, value);
}

Value* scope_get(Scope* scope, const char* name) {
    // Look in current scope
    Value* val = table_get(&scope->table, name);
    if (val != NULL) {
        return val;
    }
    
    // Look in parent scopes
    if (scope->parent != NULL) {
        return scope_get(scope->parent, name);
    }
    
    // Not found
    return NULL;
}
```

## Variable Declaration vs Assignment

**Declaration:** Creates a new variable in current scope  
**Assignment:** Updates existing variable (searches parent scopes)

```javascript
let x = 10;     // Declaration (creates x in current scope)
x = 20;         // Assignment (updates x)
```

```c
typedef enum {
    AST_LET,        // let x = value (declare)
    AST_ASSIGN,     // x = value (assign)
} ASTType;

Value evaluate(ASTNode* node, Scope* scope) {
    switch (node->type) {
        case AST_LET: {
            Value value = evaluate(node->let.value, scope);
            scope_set(scope, node->let.name, value);  // Current scope only
            return value;
        }
        
        case AST_ASSIGN: {
            Value value = evaluate(node->assign.value, scope);
            
            // Find which scope has this variable
            Scope* target = scope;
            while (target != NULL) {
                if (table_get(&target->table, node->assign.name) != NULL) {
                    scope_set(target, node->assign.name, value);
                    return value;
                }
                target = target->parent;
            }
            
            runtime_error("Undefined variable: %s", node->assign.name);
        }
    }
}
```

## Block Scope

Entering and exiting blocks:

```c
typedef struct {
    Scope* current_scope;
} Interpreter;

void interpreter_init(Interpreter* interp) {
    interp->current_scope = scope_new(NULL);  // Global scope
}

void enter_scope(Interpreter* interp) {
    interp->current_scope = scope_new(interp->current_scope);
}

void exit_scope(Interpreter* interp) {
    Scope* old = interp->current_scope;
    interp->current_scope = old->parent;
    scope_free(old);
}

Value eval_block(Interpreter* interp, ASTNode* block) {
    enter_scope(interp);
    
    Value result = make_nil();
    for (int i = 0; i < block->block.count; i++) {
        result = evaluate(block->block.statements[i], interp->current_scope);
    }
    
    exit_scope(interp);
    return result;
}
```

## Closure Example

Functions that capture variables from outer scope:

```javascript
fn makeCounter() {
    let count = 0;
    
    fn increment() {
        count = count + 1;
        return count;
    }
    
    return increment;
}

let counter = makeCounter();
print(counter());  // 1
print(counter());  // 2
print(counter());  // 3
```

The `increment` function "closes over" the `count` variable.

### Implementation (Advanced)

```c
typedef struct {
    ASTNode* body;
    Scope* closure;  // Captured scope
} Function;

Function* make_function(ASTNode* body, Scope* defining_scope) {
    Function* fn = malloc(sizeof(Function));
    fn->body = body;
    fn->closure = defining_scope;  // Capture!
    return fn;
}

Value call_function(Function* fn, Interpreter* interp) {
    // Use closure scope, not current scope!
    Scope* old_scope = interp->current_scope;
    interp->current_scope = scope_new(fn->closure);
    
    Value result = evaluate(fn->body, interp->current_scope);
    
    exit_scope(interp);
    interp->current_scope = old_scope;
    
    return result;
}
```

## Static vs Dynamic Scope

**Static (lexical) scope:** Variables resolved based on code structure  
**Dynamic scope:** Variables resolved based on call stack

```javascript
let x = 10;

fn foo() {
    print(x);
}

fn bar() {
    let x = 20;
    foo();
}

bar();
// Static scope: prints 10 (x from foo's defining scope)
// Dynamic scope: prints 20 (x from caller's scope)
```

Most languages use **static scope** (clearer, more predictable).

## Garbage Collection

Variables need cleanup:

```c
void scope_free(Scope* scope) {
    for (int i = 0; i < scope->table.count; i++) {
        free(scope->table.vars[i].name);
        value_free(scope->table.vars[i].value);
    }
    free(scope->table.vars);
    free(scope);
}
```

Or use reference counting:

```c
typedef struct {
    int ref_count;
    ValueType type;
    // ... value data
} Value;

void value_retain(Value* val) {
    val->ref_count++;
}

void value_release(Value* val) {
    val->ref_count--;
    if (val->ref_count == 0) {
        value_free(val);
    }
}
```

## Summary

**Symbol table:** Maps names to values  
**Scope:** Each block has its own symbol table  
**Scope chain:** Search current scope, then parent scopes  
**Declaration:** Creates variable in current scope  
**Assignment:** Updates variable in any parent scope  
**Closure:** Function + captured environment

Scoping is crucial for functions, blocks, and preventing name collisions.
