#!/usr/bin/env python3
"""
TinyC Test File Generator

This script generates test files from existing TinyC examples and their
corresponding JSON outputs.
"""

import os
import sys
import json
import argparse
import glob
from typing import Dict, Optional


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

# Define which test files should produce errors
ERROR_FILES = {
    "16_parser_error": "PARSER_ERROR",
    "17_lexer_error": "LEXER_ERROR"
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

    # Determine if this should produce an error
    expect_type = ERROR_FILES.get(name_without_ext, "SUCCESS")

    # For success cases, we need the expected JSON output
    compact_json = ""
    if expect_type == "SUCCESS":
        json_output = read_file(json_file)
        if json_output is None:
            return False

        # Try to compact the JSON for better readability
        try:
            json_obj = json.loads(json_output)
            compact_json = json.dumps(json_obj, separators=(',', ':'))
        except json.JSONDecodeError:
            print(f"Warning: Could not parse JSON from {json_file}, using as-is")
            compact_json = json_output.strip()

    # Create test file content
    test_content = f"// TINYC TEST\n// INFO: {description}\n// EXPECT: {expect_type}\n"

    # Only include RESULT for SUCCESS cases
    if expect_type == "SUCCESS":
        test_content += f"// RESULT: {compact_json}\n"

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
    parser.add_argument('--tc-dir', '-t', default='tinyc',
                        help='Directory containing TinyC (.tc) files (default: tinyc)')
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
        expect_type = ERROR_FILES.get(name_without_ext, "SUCCESS")

        # Check if JSON file exists (only for SUCCESS cases)
        if expect_type == "SUCCESS" and not os.path.exists(json_file):
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