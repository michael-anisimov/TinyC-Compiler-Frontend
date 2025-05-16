#!/usr/bin/env python3
"""
TinyC Test File Generator

This script generates test files from existing TinyC examples and their
corresponding JSON outputs, supporting multiple test configurations per file.
"""

import os
import sys
import json
import argparse
import glob
from typing import Dict, Optional, List, Tuple


# Descriptions for the test files
TEST_DESCRIPTIONS = {
    "0_empty": "Empty program test",
    "1_hello_world": "Basic hello world program with function declaration and definition",
    "2_variables": "Variable declarations, arrays, and initializations",
    "3_arithmetic": "Arithmetic operations and expressions",
    "4_logical": "Logical and bitwise operations",
    "5_functions": "Function declarations and definitions with various return types",
    "6_if_else": "If-else statements and conditions",
    "7_loops": "Various loop constructs (for, while, do-while)",
    "8_switch_case": "Switch-case statements with fallthrough cases",
    "9_structs": "Struct declarations and usage",
    "10_pointers": "Pointer declarations and operations",
    "11_function_pointers": "Function pointer type definitions and usage",
    "12_casting": "Type casting operations",
    "13_void_pointers": "Using void pointers for generic functions",
    "14_complex_expression": "Complex nested expressions and operator precedence",
    "15_nested_blocks": "Nested code blocks and scoping",
    "16_parser_error": "Program with a parsing error (should produce a parser error)",
    "17_lexer_error": "Program with a lexical error (should produce a lexer error)"
}

# Define which test files should produce errors and what type of error
ERROR_FILES = {
    "16_parser_error": "PARSER_ERROR",
    "17_lexer_error": "LEXER_ERROR"
}

# Define the run configurations for each test file
# Format: filename -> list of (run_type, expect, result) tuples
TEST_CONFIGS = {
    # Default configuration for most files is just parser validation
    # For examples that should be executable, we can add exec configs
    # "1_hello_world": [
    #     ("parser", "SUCCESS", None),  # Normal parser test
    #     ("exec", "0", None)           # Should exit with code 0
    # ],
    # For error cases
    "16_parser_error": [
        ("parser", "ERROR", "PARSER_ERROR")
    ],
    "17_lexer_error": [
        ("parser", "ERROR", "LEXER_ERROR")
    ]
}


def read_file(file_path: str) -> Optional[str]:
    """Read the contents of a file"""
    try:
        with open(file_path, 'r') as f:
            return f.read()
    except Exception as e:
        print(f"Error reading file {file_path}: {e}")
        return None


def create_test_file(tc_file: str, json_file: str, output_file: str, use_prefix: bool = False) -> bool:
    """
    Create a test file from TinyC code and expected JSON output.

    Args:
        tc_file: Path to the TinyC code file
        json_file: Path to the expected JSON output file
        output_file: Path to write the test file
        use_prefix: Whether to add 'test_' prefix to the output filename

    Returns:
        True if successful, False otherwise
    """
    # Read input files
    tc_code = read_file(tc_file)
    if tc_code is None:
        return False

    # Extract base name for description lookup
    base_name = os.path.basename(tc_file)
    name_without_ext = os.path.splitext(base_name)[0]

    # Get description for this test
    description = TEST_DESCRIPTIONS.get(name_without_ext, f"Test for {name_without_ext}")

    # Get test configurations
    configs = TEST_CONFIGS.get(name_without_ext, [])

    # If no specific configs defined, create default parser test
    if not configs:
        if name_without_ext in ERROR_FILES:
            # Default error test
            error_type = ERROR_FILES[name_without_ext]
            configs = [("parser", "ERROR", error_type)]
        else:
            # Default success test
            configs = [("parser", "SUCCESS", None)]

    # Load JSON output for parser success tests
    json_output = None
    if any(cfg[0] == "parser" and cfg[1] == "SUCCESS" for cfg in configs):
        json_output = read_file(json_file)
        if json_output is None and not name_without_ext in ERROR_FILES:
            print(f"Warning: No JSON file found for {tc_file}")
            return False

        # Try to compact the JSON for better readability
        try:
            json_obj = json.loads(json_output)
            json_output = json.dumps(json_obj, separators=(',', ':'))
        except json.JSONDecodeError:
            print(f"Warning: Could not parse JSON from {json_file}, using as-is")
            json_output = json_output.strip() if json_output else None

    # Create test file content
    test_content = f"// TINYC TEST\n// INFO: {description}\n"

    # Add test configurations
    for run_type, expect, error_type in configs:
        test_content += f"// RUN: {run_type}\n"
        test_content += f"// EXPECT: {expect}\n"

        if expect == "ERROR" and error_type:
            test_content += f"// ERROR_TYPE: {error_type}\n"
        elif expect == "SUCCESS" and run_type == "parser" and json_output:
            test_content += f"// RESULT: {json_output}\n"

    test_content += f"\n{tc_code}"

    # Apply prefix to filename if requested
    if use_prefix and not os.path.basename(output_file).startswith('test_'):
        output_dir = os.path.dirname(output_file)
        output_base = os.path.basename(output_file)
        output_file = os.path.join(output_dir, f"test_{output_base}")

    # Write test file
    try:
        os.makedirs(os.path.dirname(output_file) or '.', exist_ok=True)
        with open(output_file, 'w') as f:
            f.write(test_content)
        print(f"Created test file: {output_file}")
        return True
    except Exception as e:
        print(f"Error creating test file {output_file}: {e}")
        return False


def main():
    """Main function"""
    parser = argparse.ArgumentParser(description='TinyC Test File Generator')
    parser.add_argument('--tc-dir', '-t', default='samples',
                        help='Directory containing TinyC (.tc) files (default: samples)')
    parser.add_argument('--json-dir', '-j', default='json',
                        help='Directory containing JSON output files (default: json)')
    parser.add_argument('--output-dir', '-o', default='tests',
                        help='Directory to write test files (default: tests)')
    parser.add_argument('--prefix', '-p', action='store_true',
                        help='Add "test_" prefix to output filenames')
    args = parser.parse_args()

    # Create output directory if it doesn't exist
    os.makedirs(args.output_dir, exist_ok=True)

    # Find all .tc files
    tc_files = glob.glob(os.path.join(args.tc_dir, "*.tc"))
    tc_files.sort()  # Process in sorted order

    if not tc_files:
        print(f"No .tc files found in {args.tc_dir}")
        return 1

    success_count = 0
    for tc_file in tc_files:
        # Determine corresponding JSON file
        basename = os.path.basename(tc_file)
        name_without_ext = os.path.splitext(basename)[0]
        json_file = os.path.join(args.json_dir, f"{name_without_ext}.json")

        # For error cases, we don't need the JSON file
        if name_without_ext in ERROR_FILES:
            # Create output path
            output_file = os.path.join(args.output_dir, basename)

            # Create the test file
            if create_test_file(tc_file, None, output_file, args.prefix):
                success_count += 1
            continue

        # For regular files, check if JSON exists
        if not os.path.exists(json_file):
            print(f"Warning: No JSON file found for {tc_file}")
            continue

        # Create output path
        output_file = os.path.join(args.output_dir, basename)

        # Create the test file
        if create_test_file(tc_file, json_file, output_file, args.prefix):
            success_count += 1

    print(f"\nSummary: Created {success_count} test files out of {len(tc_files)} TinyC files")
    return 0


if __name__ == "__main__":
    sys.exit(main())