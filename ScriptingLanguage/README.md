# Building a Scripting Language

Build a real programming language from scratch. Learn how interpreters work by creating one yourself - tokenizer, parser, evaluator, and runtime.

## What you get

- Tokenizer (lexical analysis)
- Expression parser and evaluator
- Variables and scopes
- Functions with parameters
- Control flow (if/else, while, for)
- Interactive REPL
- A working scripting language!

Each example builds on the previous. By the end, you'll have a language that can run real programs.

## Building

```bash
# Windows
cd examples
build_all.bat

# Linux / MSYS2
cd examples
./build_all.sh
```

## Documentation

- **[Lexing & Parsing](docs/LEXING_PARSING.md)** - Breaking code into tokens, building syntax trees
- **[Evaluation](docs/EVALUATION.md)** - Running expressions, operator precedence
- **[Variables & Scope](docs/VARIABLES_SCOPE.md)** - Symbol tables, scoping rules
- **[Functions](docs/FUNCTIONS.md)** - Function calls, parameters, return values
- **[Control Flow](docs/CONTROL_FLOW.md)** - If statements, loops, break/continue

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_tokenizer | Breaking source code into tokens (numbers, operators, identifiers) |
| 02_calculator | Expression parser and evaluator with operator precedence |
| 03_variables | Variable assignment and lookup, symbol table |
| 04_control_flow | If statements, while loops, print statements - a real programming language! |

Each example builds on the previous, showing the complete journey from raw text to executing programs.

## Our Language Syntax

```javascript
// Variables
let x = 10;
let name = "Hello";

// Math
let result = (x + 5) * 2 - 3;

// Functions
fn add(a, b) {
    return a + b;
}

let sum = add(10, 20);

// Control flow
if (x > 5) {
    print("x is big");
} else {
    print("x is small");
}

// Loops
for (let i = 0; i < 10; i = i + 1) {
    print(i);
}

while (x > 0) {
    x = x - 1;
}
```

## What this teaches

- How compilers and interpreters work
- Lexical analysis (tokenization)
- Parsing techniques (recursive descent)
- Abstract Syntax Trees (AST)
- Symbol tables and scoping
- Runtime evaluation
- Memory management for language runtime
- Error handling and reporting

After this, you'll understand how Python, JavaScript, and other interpreters work under the hood.

## Quick Start

```bash
cd examples
build_all.bat

# Try the tokenizer - see how code is broken into tokens
bin\01_tokenizer.exe

# Try the calculator - interactive math with correct precedence
bin\02_calculator.exe
> 2 + 3 * 4
= 14.00
> (10 + 20) * (5 - 3) / 2
= 30.00

# Try variables - store and reuse values
bin\03_variables.exe
> let x = 10
= 10.00
> let y = x * 2
= 20.00
> y
= 20.00

# Try control flow - a complete programming language!
bin\04_control_flow.exe
(Runs example programs with if statements and loops)
```

This is hands-on compiler/interpreter construction. You'll build every part from scratch and watch your language come to life.
