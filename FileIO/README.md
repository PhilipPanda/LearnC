# File I/O & Serialization

Learn to read and write files in C. Text files, binary files, CSV, JSON, configuration files - everything you need to persist data.

## What you get

- Text file operations (read, write, append)
- Binary file I/O (structured data)
- CSV parsing and generation
- JSON parsing and serialization
- INI/config file handling
- Error handling patterns
- Real-world file processing

All examples include proper error handling and resource cleanup.

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

- **[Text Files](docs/TEXT_FILES.md)** - Reading and writing text, line by line processing
- **[Binary Files](docs/BINARY_FILES.md)** - Structured data, serialization, endianness
- **[CSV Files](docs/CSV_FILES.md)** - Parsing CSV, handling quoted fields, writing CSV
- **[JSON](docs/JSON.md)** - Simple JSON parser and serializer
- **[Error Handling](docs/ERROR_HANDLING.md)** - File errors, cleanup patterns, errno

## Examples

| Example | What It Teaches |
|---------|----------------|
| 01_text_basics | Reading and writing text files, fopen/fclose, fprintf/fscanf |
| 02_line_processing | Reading files line by line, word counting, text processing |
| 03_binary_files | Writing/reading structs, binary serialization, endianness |
| 04_csv_parser | Parsing CSV files, handling quotes and commas |
| 05_json_parser | Simple JSON parser for objects and arrays |
| 06_config_files | INI-style config file parser |
| 07_log_system | Rotating log files, buffered writing, timestamps |

Each example includes error handling and proper resource cleanup.

## What this teaches

- File operations (open, read, write, close)
- Text vs binary mode
- Error handling with errno
- Resource management (RAII-style cleanup)
- Parsing techniques
- Data serialization
- Real-world file formats
- Buffer management
- Cross-platform file handling

After this, you'll be able to work with any file format and persist program data reliably.

## Quick Start

```bash
cd examples
build_all.bat

# Create and read a text file
bin\01_text_basics.exe

# Process a CSV file
bin\04_csv_parser.exe

# Parse JSON data
bin\05_json_parser.exe
```

Files are everywhere in programming. Master file I/O and you can build tools that process logs, parse data, save game state, read configuration, and more.
