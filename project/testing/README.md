# TinyC Parser Testing Framework

This framework allows you to test your TinyC parser implementation against a standardized
set of test cases.

## Requirements

Your parser should:
1. Accept a TinyC file as input
2. Output the Abstract Syntax Tree (AST) in JSON format
3. Follow the AST structure as defined in the TinyC AST schema

## How to Run Tests

To run the tests against your parser:

```bash
python3 test_runner.py "./your-parser-command" --test-dir tests --verbose
```

Where `./your-parser-command`  is the command to run your parser (it should accept a file path
and output JSON to stdout).

## Test Format
Each test file contains:

- TinyC code to be parsed
- Metadata about what the test is checking
- The expected JSON output

If your parser correctly produces the expected AST for all test cases, you're on the right track!