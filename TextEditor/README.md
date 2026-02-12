# Building a Text Editor

Build a real working text editor from scratch. Start simple and add features step by step - cursor movement, text insertion, file operations, search, undo/redo.

## What you get

- Text buffer management
- Cursor positioning and movement
- Character insertion and deletion
- Line operations (insert, delete, join)
- File loading and saving
- Search and replace
- Undo/redo system
- Status bar and command mode
- A fully functional console text editor!

Each example builds on the previous, showing how editors actually work internally.

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

- **[Text Buffers](docs/TEXT_BUFFERS.md)** - Storing text, gap buffers, line arrays, rope data structures
- **[Cursor & Movement](docs/CURSOR_MOVEMENT.md)** - Cursor positioning, navigation, viewport scrolling
- **[Editing Operations](docs/EDITING_OPS.md)** - Insert, delete, line operations, clipboard
- **[File Operations](docs/FILE_OPS.md)** - Loading, saving, modified flag, autosave
- **[Advanced Features](docs/ADVANCED.md)** - Undo/redo, search, syntax highlighting

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_simple_editor | Complete working text editor with cursor, editing, load/save |

A fully functional text editor in ~400 lines of C. Shows text buffer management, cursor movement, keyboard input, file I/O, and screen rendering.

## Controls (for the editors)

```
Arrow Keys    - Move cursor
Home/End      - Start/end of line
Page Up/Down  - Scroll viewport
Backspace     - Delete character before cursor
Delete        - Delete character at cursor
Enter         - Insert new line
Ctrl+S        - Save file
Ctrl+Q        - Quit
Ctrl+F        - Find
Ctrl+Z        - Undo
Ctrl+Y        - Redo
```

## What this teaches

- Data structures for text (arrays, gap buffers, ropes)
- Terminal control (ANSI escape codes, raw mode)
- User input handling (keyboard events)
- Efficient text manipulation
- Undo/redo implementation
- Search algorithms
- Viewport and scrolling
- File I/O integration
- State management

After building this, you'll understand how vim, emacs, nano, and VSCode work under the hood.

## Quick Start

```bash
cd examples
build_all.bat

# Try the editor
bin\01_simple_editor.exe myfile.txt

# Edit your code files
bin\01_simple_editor.exe test.c
```

Building a text editor teaches you about data structures, algorithms, user interfaces, and how to build complex interactive programs. It's one of the best learning projects in programming.
