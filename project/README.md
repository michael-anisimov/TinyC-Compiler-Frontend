# TinyC Compiler Frontend Implementation

This directory contains the implementation of the TinyC compiler frontend, which includes a lexer, parser, and AST generator for the TinyC programming language. Also there is a testing suite, covering all TinyC language features, which will help students with development of their own parsers if they want.

## Project Structure

```
project/
├── include/           # Header files
├── src/              # Source files
│   ├── lexer/        # Lexical analyzer implementation
│   ├── parser/       # Parser implementation
│   ├── ast/          # Abstract Syntax Tree implementation
│   └── main.cpp      # Main executable entry point
├── tests/            # Test files
│   ├── lexer/        # Lexer unit tests
│   └── parser/       # Parser unit tests
├── test_suite/       # Testing suite
│   ├── json/         # JSON schema and test outputs
│   ├── tests/        # TinyC test files
│   ├── test_runner.py # Main test runner script
│   └── test_generator.py # Test case generator
├── docs/             # Documentation
├── CMakeLists.txt    # CMake build configuration
└── Doxyfile         # Doxygen configuration
```

## Building the Project

### Prerequisites

- CMake (version 3.10 or higher)
- C++17 compatible compiler
- Google Test (for running tests)
- Doxygen (optional, for documentation)
- Python 3.x (for running the testing suite)

### Build Steps

1. Create a build directory:
   ```bash
   mkdir build && cd build
   ```

2. Configure the project:
   ```bash
   cmake ..
   ```

3. Build the project:
   ```bash
   cmake --build .
   ```


## Using the Compiler

The compiler can be used in two ways:

1. As a library (`libtinyc`):
   - Link against the library in your project
   - Use the public headers from the `include/` directory

2. As a standalone executable:
   ```bash
   ./tinyc-compiler [options] <source_file>
   ```

   Options:
   - `--lex`, `-l`: Run in lexer mode (output tokens)
   - `--parse`, `-p`: Run in parser mode (output AST as JSON) [default]
   - `--pretty`, `-pp`: Pretty print JSON output (only with parser mode)
   - No arguments: Run in interactive mode (REPL like)

   Examples:
   ```bash
   # Parse a file and output AST as JSON
   ./tinyc-compiler input.tc

   # Parse a file with pretty-printed JSON output
   ./tinyc-compiler --pretty input.tc

   # Run in lexer mode to see tokens
   ./tinyc-compiler --lex input.tc

   # Run in interactive mode
   ./tinyc-compiler
   ```

## Testing Suite

The project includes a comprehensive testing suite for validating the compiler implementation. See [test_suite/README.md](test_suite/README.md) for details on running tests and test file format.

## Key Components

### Lexer (`src/lexer/`)
- Token definitions and lexical analysis
- Converts source code into tokens

### Parser (`src/parser/`)
- LL1 (Recursive descent)
- Handles type declarations, expressions, and statements
- Generates AST nodes

### AST (`src/ast/`)
- Abstract Syntax Tree implementation
- Includes visitors for:
  - JSON output
  - AST dumping

## Documentation

- API documentation can be generated using Doxygen
- See the `docs/` directory for additional documentation
- The main executable usage can be found in `src/main.cpp`
- [TinyC Grammar Reference](docs/grammar/Grammar_reference.md)
- [LL1 version of Grammar](docs/grammar/Grammar_LL1.txt)
- [JSON Schema](test_suite/tinyc-ast-schema.json)