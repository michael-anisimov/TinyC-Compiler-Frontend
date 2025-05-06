#!/usr/bin/env python3
"""
TinyC Parser Test Runner

This script validates student parser implementations against a set of TinyC test files.
Each test file contains TinyC code along with metadata about the expected output or error.
"""

import os
import sys
import re

import json
import subprocess
import argparse
from typing import Dict, List, Optional, Tuple, NamedTuple, Any, Union

import jsonschema


class TinyCTest(NamedTuple):
    """Represents a TinyC test case"""
    name: str
    description: str
    expect_type: str  # Can be 'SUCCESS', 'PARSER_ERROR', or 'LEXER_ERROR'
    code: str
    expected_output: Optional[str]  # None for error test cases


def extract_number_prefix(filename: str) -> int:
    """
    Extract the numeric prefix from a filename (e.g., '10_pointers.tc' -> 10)
    If no numeric prefix is found, return a large number to sort it at the end
    """
    match = re.match(r'^(\d+)_', os.path.basename(filename))
    if match:
        return int(match.group(1))
    return float('inf')  # Sort files without numeric prefix at the end


def parse_test_file(file_path: str) -> Optional[TinyCTest]:
    """
    Parse a TinyC test file to extract metadata and code.

    Args:
        file_path: Path to the test file

    Returns:
        A TinyCTest object if parsing succeeds, None otherwise
    """
    try:
        with open(file_path, 'r') as f:
            content = f.read()

        # Check if this is a test file (should start with a special comment)
        if not content.lstrip().startswith('// TINYC TEST'):
            print(f"Warning: {file_path} does not appear to be a test file (missing header)")
            return None

        # Extract test information using regex
        name = os.path.basename(file_path).replace('.tc', '')
        description_match = re.search(r'// INFO: (.*?)$', content, re.MULTILINE)
        expect_match = re.search(r'// EXPECT: (.*?)$', content, re.MULTILINE)

        if not description_match or not expect_match:
            print(f"Warning: {file_path} is missing required metadata (INFO or EXPECT)")
            return None

        description = description_match.group(1).strip()
        expect_type = expect_match.group(1).strip()

        # Validate expect_type
        if expect_type not in ['SUCCESS', 'PARSER_ERROR', 'LEXER_ERROR']:
            print(f"Warning: {file_path} has invalid EXPECT value: {expect_type}")
            print("EXPECT should be one of: SUCCESS, PARSER_ERROR, LEXER_ERROR")
            return None

        # For success cases, we need the expected JSON output
        expected_output = None
        if expect_type == 'SUCCESS':
            result_match = re.search(r'// RESULT: (.*?)$', content, re.MULTILINE)
            if not result_match:
                print(f"Warning: {file_path} is a SUCCESS test but missing RESULT metadata")
                return None
            expected_output = result_match.group(1).strip()

        # Extract code (everything after the metadata)
        test_header_pattern = re.compile(r'^// (TINYC TEST|INFO:|EXPECT:|RESULT:).*?$', re.MULTILINE)
        code_only = re.sub(test_header_pattern, '', content).lstrip()

        return TinyCTest(name, description, expect_type, code_only, expected_output)
    except Exception as e:
        print(f"Error parsing test file {file_path}: {e}")
        return None

def load_json_schema(schema_path: str = "tinyc-ast-schema.json") -> Dict[str, Any]:
    """
    Load the TinyC AST JSON schema.

    Args:
        schema_path: Path to the JSON schema file

    Returns:
        The loaded JSON schema as a dictionary
    """
    try:
        with open(schema_path, 'r') as f:
            return json.load(f)
    except Exception as e:
        print(f"Error loading JSON schema from {schema_path}: {e}")
        print("Schema validation will be disabled.")
        return {}


def validate_against_schema(json_obj: Dict[str, Any], schema: Dict[str, Any]) -> Tuple[bool, List[str]]:
    """
    Validate a JSON object against the TinyC AST schema.

    Args:
        json_obj: The JSON object to validate
        schema: The JSON schema to validate against

    Returns:
        Tuple of (is_valid, validation_errors)
    """
    if not schema:
        # If no schema is available, skip validation
        return True, []

    errors = []
    try:
        # Create a validator instance
        validator = jsonschema.Draft7Validator(schema)

        # Validate and collect errors
        for error in validator.iter_errors(json_obj):
            # Format the error message with path
            path = ".".join(str(p) for p in error.absolute_path)
            if path:
                error_msg = f"At {path}: {error.message}"
            else:
                error_msg = error.message
            errors.append(error_msg)

        return len(errors) == 0, errors
    except Exception as e:
        return False, [f"Schema validation error: {str(e)}"]

def run_parser(parser_command: str, code: str) -> Tuple[str, int]:
    """
    Run the parser on the provided code and return its output and exit code.

    Args:
        parser_command: Command to run the parser
        code: TinyC code to parse

    Returns:
        Tuple of (output, exit_code)
    """
    # Create temporary file with the code
    temp_file = "temp_code.tc"
    try:
        with open(temp_file, 'w') as f:
            f.write(code)

        # Run the parser command
        cmd = f"{parser_command} {temp_file}"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)

        # Combine stdout and stderr for output checking
        output = result.stdout.strip()
        if result.stderr.strip():
            if output:
                output += "\n"
            output += result.stderr.strip()

        return output, result.returncode
    except Exception as e:
        print(f"Error running parser: {e}")
        return "", -1
    finally:
        # Clean up temporary file
        if os.path.exists(temp_file):
            os.remove(temp_file)


def compare_json_objects(expected: Dict[str, Any], actual: Dict[str, Any], path: str = "") -> Tuple[bool, List[str]]:
    """
    Recursively compare two JSON objects, ignoring specific values in location fields.

    Args:
        expected: Expected JSON object
        actual: Actual JSON object
        path: Current path in the JSON tree (for error reporting)

    Returns:
        Tuple of (is_equal, differences_list)
    """
    differences = []

    # Check for same keys
    expected_keys = set(expected.keys())
    actual_keys = set(actual.keys())

    # Check for missing keys
    missing_keys = expected_keys - actual_keys
    if missing_keys:
        differences.append(f"{path}: Missing keys: {', '.join(missing_keys)}")

    # Check for extra keys (less important but still useful feedback)
    extra_keys = actual_keys - expected_keys
    if extra_keys:
        differences.append(f"{path}: Extra keys: {', '.join(extra_keys)}")

    # Check key values
    for key in expected_keys.intersection(actual_keys):
        current_path = f"{path}.{key}" if path else key

        # Special handling for location fields - only check existence
        if key == "location":
            # For location, we only check that both objects have the required location fields
            if isinstance(expected[key], dict) and isinstance(actual[key], dict):
                expected_loc_keys = set(expected[key].keys())
                actual_loc_keys = set(actual[key].keys())

                # Check that all expected location fields exist
                missing_loc_keys = expected_loc_keys - actual_loc_keys
                if missing_loc_keys:
                    differences.append(f"{current_path}: Missing location fields: {', '.join(missing_loc_keys)}")
            else:
                differences.append(f"{current_path}: Expected location object but got {type(actual[key])}")
        # Recursive comparison for nested objects
        elif isinstance(expected[key], dict) and isinstance(actual[key], dict):
            equal, child_diffs = compare_json_objects(expected[key], actual[key], current_path)
            differences.extend(child_diffs)
        # Recursive comparison for arrays
        elif isinstance(expected[key], list) and isinstance(actual[key], list):
            if len(expected[key]) != len(actual[key]):
                differences.append(f"{current_path}: Array length mismatch - expected {len(expected[key])}, got {len(actual[key])}")
            else:
                for i, (expected_item, actual_item) in enumerate(zip(expected[key], actual[key])):
                    if isinstance(expected_item, dict) and isinstance(actual_item, dict):
                        equal, child_diffs = compare_json_objects(expected_item, actual_item, f"{current_path}[{i}]")
                        differences.extend(child_diffs)
                    elif expected_item != actual_item:
                        differences.append(f"{current_path}[{i}]: Value mismatch - expected {expected_item}, got {actual_item}")
        # Simple value comparison for non-objects
        elif expected[key] != actual[key]:
            differences.append(f"{current_path}: Value mismatch - expected {expected[key]}, got {actual[key]}")

    return len(differences) == 0, differences


def validate_json_output(expected: str, actual: str, verbose: bool = False) -> Tuple[bool, List[str]]:
    """
    Compare two JSON ASTs for semantic equivalence, ignoring specific location values.

    Args:
        expected: Expected JSON output
        actual: Actual JSON output from the parser
        verbose: Whether to print detailed comparison information

    Returns:
        Tuple of (is_equal, differences_list)
    """
    try:
        expected_json = json.loads(expected)
        actual_json = json.loads(actual)

        # Do a recursive comparison of the JSON objects
        return compare_json_objects(expected_json, actual_json)
    except json.JSONDecodeError as e:
        return False, [f"JSON parse error: {e}"]


def validate_error_output(expected_type: str, actual_output: str) -> Tuple[bool, str]:
    """
    Validate that the output contains the expected error type.

    Args:
        expected_type: Expected error type ('PARSER_ERROR' or 'LEXER_ERROR')
        actual_output: Actual output from the parser

    Returns:
        Tuple of (is_valid, error_message)
    """
    # Normalize errors to lowercase for case-insensitive comparison
    lowercase_output = actual_output.lower()

    if expected_type == 'PARSER_ERROR':
        # Accept various forms of parser error messages
        if any(pattern in lowercase_output for pattern in ['parser error', 'parse error', 'parsing error', 'syntax error']):
            return True, ""
        return False, "Expected parser error but none found in output"

    elif expected_type == 'LEXER_ERROR':
        # Accept various forms of lexer error messages
        if any(pattern in lowercase_output for pattern in ['lexer error', 'lex error', 'lexical error', 'token error']):
            return True, ""
        return False, "Expected lexer error but none found in output"

    # This should never happen if we validate expect_type properly
    return False, f"Unknown expected error type: {expected_type}"


def get_test_files(test_dir: str, test_num: Optional[int] = None, test_range: Optional[Tuple[int, int]] = None) -> List[str]:
    """
    Get a list of all test files in the given directory, sorted by numeric prefix.
    Can filter to specific test number or range.

    Args:
        test_dir: Directory to search for test files
        test_num: If provided, only include the test with this number
        test_range: If provided, only include tests in this range (start, end) inclusive

    Returns:
        List of paths to test files
    """
    if not os.path.isdir(test_dir):
        print(f"Error: Test directory {test_dir} not found")
        return []

    # Get all .tc files
    all_test_files = []
    for file in os.listdir(test_dir):
        if file.endswith('.tc'):
            file_path = os.path.join(test_dir, file)

            # Verify it's a test file by checking the first line
            try:
                with open(file_path, 'r') as f:
                    first_line = f.readline().strip()
                    if first_line.startswith('// TINYC TEST'):
                        all_test_files.append(file_path)
            except Exception as e:
                print(f"Error reading {file_path}: {e}")

    # Filter based on test number or range
    filtered_files = []
    for file_path in all_test_files:
        file_num = extract_number_prefix(file_path)

        # Skip files without numeric prefix if we're filtering
        if file_num == float('inf') and (test_num is not None or test_range is not None):
            continue

        if test_num is not None:
            # Single test number
            if file_num == test_num:
                filtered_files.append(file_path)
        elif test_range is not None:
            # Test range
            start, end = test_range
            if start <= file_num <= end:
                filtered_files.append(file_path)
        else:
            # No filter, include all test files
            filtered_files.append(file_path)

    # Sort test files by their numeric prefix
    return sorted(filtered_files, key=extract_number_prefix)


def run_tests(parser_command: str, test_dir: str, schema_path: str, test_num: Optional[int] = None,
              test_range: Optional[Tuple[int, int]] = None, verbose: bool = False) -> Tuple[int, int]:
    """
    Run tests against the parser.

    Args:
        parser_command: Command to run the parser
        test_dir: Directory containing test files
        test_num: If provided, only run the test with this number
        test_range: If provided, only run tests in this range (start, end) inclusive
        verbose: Whether to print detailed comparison information

    Returns:
        Tuple of (passed_count, failed_count)
    """
    schema = load_json_schema(schema_path)
    schema_loaded = bool(schema)

    if schema_loaded:
        print(f"Loaded JSON schema from {schema_path}")
    else:
        print(f"Warning: Could not load JSON schema. Schema validation will be skipped.")

    test_files = get_test_files(test_dir, test_num, test_range)

    if not test_files:
        if test_num is not None:
            print(f"No test file found with number {test_num}")
        elif test_range is not None:
            print(f"No test files found in range {test_range[0]}-{test_range[1]}")
        else:
            print(f"No test files found in {test_dir}")
        return 0, 0

    passed = 0
    failed = 0

    # Create a message about which tests we're running
    test_msg = f"{len(test_files)} tests"
    if test_num is not None:
        test_msg = f"test {test_num}"
    elif test_range is not None:
        test_msg = f"tests {test_range[0]}-{test_range[1]}"

    print(f"Running {test_msg}...")

    for i, test_file in enumerate(test_files, 1):
        test = parse_test_file(test_file)
        if not test:
            print(f"Skipping invalid test file: {test_file}")
            continue

        print(f"\nTest {i}/{len(test_files)}: {test.name}")
        print(f"  Description: {test.description}")
        print(f"  Expecting: {test.expect_type}")

        # Run parser on test code
        actual_output, exit_code = run_parser(parser_command, test.code)

        # For error tests, non-zero exit code is expected
        # For success tests, a non-zero exit code with no output is a problem
        if not actual_output and exit_code != 0 and test.expect_type == 'SUCCESS':
            print(f"  ❌ FAILED (Parser failed with exit code {exit_code})")
            failed += 1
            continue

        # Handle different expected result types
        passed_test = False
        error_msg = ""

        if test.expect_type == 'SUCCESS':
            # Parse JSON output for success tests
            try:
                actual_json = json.loads(actual_output)

                # First, validate against the schema if available
                if schema_loaded:
                    schema_valid, schema_errors = validate_against_schema(actual_json, schema)
                    if not schema_valid:
                        passed_test = False
                        error_msg = "Schema validation failed:"
                        for error in schema_errors[:5]:  # Show at most 5 schema errors
                            error_msg += f"\n    - {error}"
                        if len(schema_errors) > 5:
                            error_msg += f"\n    ... and {len(schema_errors) - 5} more schema errors"
                    else:
                        # Schema validation passed, proceed with output comparison
                        if test.expected_output:
                            passed_test, differences = validate_json_output(test.expected_output, actual_output, verbose)
                            if not passed_test and verbose:
                                error_msg = "\n  Structural comparison failed:"
                                for diff in differences[:10]:  # Show at most 10 differences
                                    error_msg += f"\n    - {diff}"
                                if len(differences) > 10:
                                    error_msg += f"\n    ... and {len(differences) - 10} more differences"
                        else:
                            passed_test = False
                            error_msg = "Missing expected output for SUCCESS test"
                else:
                    # No schema validation, just compare outputs
                    if test.expected_output:
                        passed_test, differences = validate_json_output(test.expected_output, actual_output, verbose)
                        if not passed_test and verbose:
                            error_msg = "\n  Differences:"
                            for diff in differences[:10]:  # Show at most 10 differences
                                error_msg += f"\n    - {diff}"
                            if len(differences) > 10:
                                error_msg += f"\n    ... and {len(differences) - 10} more differences"
                    else:
                        passed_test = False
                        error_msg = "Missing expected output for SUCCESS test"

            except json.JSONDecodeError as e:
                passed_test = False
                error_msg = f"Invalid JSON output: {e}"
        else:
            # For error tests, check for the appropriate error message
            passed_test, error_msg = validate_error_output(test.expect_type, actual_output)

        if passed_test:
            print(f"  ✅ PASSED")
            passed += 1
        else:
            print(f"  ❌ FAILED")
            if error_msg:
                print(f"  {error_msg}")

            # Show first 80 chars of expected/actual (or less if they're shorter)
            if test.expect_type == 'SUCCESS' and test.expected_output:
                expected_preview = test.expected_output[:80] + "..." if len(test.expected_output) > 80 else test.expected_output
                actual_preview = actual_output[:80] + "..." if len(actual_output) > 80 else actual_output
                print(f"  Expected: {expected_preview}")
                print(f"  Actual  : {actual_preview}")
            else:
                print(f"  Actual output: {actual_output[:80]}" + ("..." if len(actual_output) > 80 else ""))

            failed += 1

    return passed, failed


def parse_range(range_str: str) -> Optional[Tuple[int, int]]:
    """
    Parse a range string in the format 'start-end' to a tuple.

    Args:
        range_str: String in the format 'start-end'

    Returns:
        Tuple (start, end) if valid, None otherwise
    """
    try:
        if '-' in range_str:
            start, end = map(int, range_str.split('-'))
            if start <= end:
                return start, end

        # If we get here, the range is invalid
        print(f"Invalid range: {range_str}. Should be in format 'start-end' where start <= end.")
        return None
    except ValueError:
        print(f"Invalid range: {range_str}. Should contain numeric values.")
        return None


def main():
    """Main function"""
    parser = argparse.ArgumentParser(description='TinyC Parser Test Runner')
    parser.add_argument('parser_command', help='Command to run the parser (e.g., "./student-parser -p")')
    parser.add_argument('--test-dir', '-d', default='tests', help='Directory containing test files (default: tests)')
    parser.add_argument('--schema', '-s', default='tinyc-ast-schema.json', help='Path to JSON schema file (default: tinyc-ast-schema.json)')
    parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose output with detailed differences')

    # Add mutually exclusive group for test selection
    test_group = parser.add_mutually_exclusive_group()
    test_group.add_argument('--test', '-t', type=int, help='Run a specific test by number (e.g., 5 for 5_*.tc)')
    test_group.add_argument('--range', '-r', help='Run tests in a specific range (e.g., 3-7 for tests 3 to 7)')

    args = parser.parse_args()

    # Parse range if provided
    test_range = None
    if args.range:
        test_range = parse_range(args.range)
        if test_range is None:
            return 1

    passed, failed = run_tests(args.parser_command, args.test_dir, args.schema, args.test, test_range, args.verbose)

    print("\n" + "="*50)
    print(f"Test Results: {passed} passed, {failed} failed")
    print(f"Overall: {'✅ PASSED' if failed == 0 else '❌ FAILED'}")
    print("="*50)

    # Return non-zero exit code if any tests failed
    return 1 if failed > 0 else 0


if __name__ == "__main__":
    sys.exit(main())