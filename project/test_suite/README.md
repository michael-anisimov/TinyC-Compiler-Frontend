# TinyC Test Suite

A comprehensive testing framework for validating TinyC compiler implementations. This test suite supports multiple test types, JSON schema validation, and flexible test selection.

## Features

- Multiple test types:
  - Parser tests (AST output validation)
  - Execution tests (program output/return value)
  - Type checker tests
- JSON schema validation for AST output
- Support for error testing (parser errors, lexer errors)
- Flexible test selection (single test, test range, or all tests)
- Detailed test output with verbose mode

## Requirements

- Python 3.6+
- jsonschema package (`pip install jsonschema`)

## Test File Format

Test files (`.tc`) should follow this format:

```c
// TINYC TEST
// INFO: Description of the test
// RUN: parser
// EXPECT: SUCCESS
// RESULT: {"nodeType":"Program","declarations":[],"location":{"filename":"test.tc"}}

// RUN: exec
// EXPECT: 0

// Actual TinyC code here
```

### Test Directives

- `// TINYC TEST`: Required header for test files
- `// INFO: <description>`: Test description
- `// RUN: <type>`: Test type (parser, exec, typechecker)
- `// EXPECT: <result>`: Expected result (SUCCESS, ERROR, or specific value)
- `// RESULT: <json>`: Expected JSON output (for parser tests)
- `// ERROR_TYPE: <type>`: Type of error (for error tests)

## Usage

### Command Line Options

```bash
usage: test_runner.py [-h] [--test-dir TEST_DIR] [--verbose] [--skip-schema]
                      [--run-type RUN_TYPE] [--test TEST | --range RANGE]
                      command

positional arguments:
  command               Base command to run the compiler (e.g., "./tinyc-compiler")

optional arguments:
  -h, --help            show this help message and exit
  --test-dir TEST_DIR, -d TEST_DIR
                        Directory containing test files (default: tests)
  --verbose, -v         Enable verbose output with detailed differences
  --skip-schema         Skip JSON schema validation
  --run-type RUN_TYPE, -rt RUN_TYPE
                        Only run test configurations of a specific type
  --test TEST, -t TEST  Run a specific test by number (e.g., 5 for 5_*.tc)
  --range RANGE, -r RANGE
                        Run tests in a specific range (e.g., 3-7 for tests 3 to 7)
```

### Example Commands

```bash
# Run all tests
python3 test_runner.py "../build/tinyc-compiler"

# Run with verbose output
python3 test_runner.py "../build/tinyc-compiler" --verbose

# Run tests from a specific directory
python3 test_runner.py "../build/tinyc-compiler" --test-dir custom_tests

# Run a specific test by number
python3 test_runner.py "../build/tinyc-compiler" --test 5

# Run tests in a range
python3 test_runner.py "../build/tinyc-compiler" --range 1-10

# Run only parser tests
python3 test_runner.py "../build/tinyc-compiler" --run-type parser

# Skip schema validation
python3 test_runner.py "../build/tinyc-compiler" --skip-schema
```

### Output

The test runner provides detailed output for each test. Here's an example of a successful test run:

```
Running 19 test files...

Test file 1/19: 0_empty
  Description: Empty program test
  Configuration 1/1: parser (Expect: SUCCESS)
    ✅ PASSED

Test file 2/19: 1_hello_world
  Description: Basic hello world program
  Configuration 1/2: parser (Expect: SUCCESS)
    ✅ PASSED
  Configuration 2/2: exec (Expect: 0)
    ✅ PASSED
...
==================================================
Test Results: 19 passed, 0 failed
Overall: ✅ PASSED
==================================================
```

And here's an example of a failed test with detailed differences:

```
Test file 20/20: t
  Description: Test for variable declaration
  Configuration 1/1: parser (Expect: SUCCESS)
    ❌ FAILED
    Differences in JSON ASTs:
    - declarations[0]: Missing keys: name, init
    - declarations[0]: Extra keys: location, identifier
    - declarations[0].nodeType: Value mismatch - expected VarDecl, got VariableDeclaration
    - declarations[0].type: Value mismatch - expected int, got {'nodeType': 'PrimitiveType', 'kind': 'int', 'location': {'filename': 'temp_code.tc', 'line': 1, 'column': 1}}
```

## Test Types

### Parser Tests
- Validates AST output against JSON schema
- Compares output with expected JSON if provided
- Supports error testing for parser/lexer errors

### Execution Tests
- Validates program output or return value
- Supports error testing for runtime errors

### Type Checker Tests
- Validates type checker output
- Supports error testing for type errors

## Troubleshooting

- If you get a "JSON schema validation failed" error, check that your AST output matches the schema in `tinyc-ast-schema.json`
- For parser errors, make sure your error messages include the expected keywords (e.g., "parser error", "syntax error")
- Use `--verbose` to see detailed output and differences