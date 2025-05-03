#!/usr/bin/env python3
"""
TinyC Test Runner

This script validates TinyC compiler implementations against a set of test files.
Each test file can contain multiple test configurations with different run types,
expected outputs, and other parameters.

Run types supported:
- parser: Test the parser by comparing AST output
- exec: Test the compiler by executing the program and comparing output or return value
- typechecker: Test the type checker output or errors
"""

import os
import sys
import re
import json
import subprocess
import argparse
from typing import Dict, List, Optional, Tuple, NamedTuple, Any, Union


class TestConfig(NamedTuple):
    """Represents a single test configuration within a test file"""
    run_type: str  # 'parser', 'exec', 'typechecker', etc.
    expect: str  # Can be 'SUCCESS', 'ERROR', or a specific expected value
    result: Optional[str]  # Expected output or None
    error_type: Optional[str]  # For error tests, specifies the type of error


class TinyCTest(NamedTuple):
    """Represents a TinyC test case with multiple test configurations"""
    name: str
    description: str
    code: str
    configs: List[TestConfig]  # List of test configurations


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
    Parse a TinyC test file to extract metadata, code, and test configurations.

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

        # Extract basic test information
        name = os.path.basename(file_path).replace('.tc', '')
        description_match = re.search(r'// INFO: (.*?)$', content, re.MULTILINE)

        if not description_match:
            print(f"Warning: {file_path} is missing required INFO metadata")
            return None

        description = description_match.group(1).strip()

        # Extract all RUN blocks
        run_matches = re.finditer(r'// RUN: (.*?)$', content, re.MULTILINE)
        if not run_matches:
            print(f"Warning: {file_path} has no RUN directives")

            # For backward compatibility, try to extract the old format
            expect_match = re.search(r'// EXPECT: (.*?)$', content, re.MULTILINE)
            if expect_match:
                expect = expect_match.group(1).strip()
                if expect in ['PARSER_ERROR', 'LEXER_ERROR']:
                    # Old error test
                    configs = [TestConfig('parser', 'ERROR', None, expect)]
                else:
                    # Old success test
                    result_match = re.search(r'// RESULT: (.*?)$', content, re.MULTILINE)
                    result = result_match.group(1).strip() if result_match else None
                    configs = [TestConfig('parser', 'SUCCESS', result, None)]
            else:
                return None
        else:
            # Process each RUN block to create test configurations
            configs = []
            positions = []

            # Find all RUN directive positions
            for match in run_matches:
                positions.append((match.start(), match.group(1).strip()))

            # Add a sentinel position at the end
            positions.append((len(content), None))

            # Process each block between RUN directives
            for i in range(len(positions) - 1):
                start_pos, run_type = positions[i]
                end_pos = positions[i + 1][0]

                # Extract the block between this RUN and the next one
                block = content[start_pos:end_pos]

                # Extract EXPECT from this block
                expect_match = re.search(r'// EXPECT: (.*?)$', block, re.MULTILINE)
                if not expect_match:
                    print(f"Warning: Missing EXPECT for RUN: {run_type} in {file_path}")
                    continue

                expect = expect_match.group(1).strip()

                # Check if this is an error test
                error_type = None
                if expect == 'ERROR':
                    error_type_match = re.search(r'// ERROR_TYPE: (.*?)$', block, re.MULTILINE)
                    if error_type_match:
                        error_type = error_type_match.group(1).strip()

                # Extract RESULT if present
                result = None
                result_match = re.search(r'// RESULT: (.*?)$', block, re.MULTILINE)
                if result_match:
                    result = result_match.group(1).strip()

                # Add the configuration
                configs.append(TestConfig(run_type, expect, result, error_type))

        # Extract code (everything after the metadata)
        # Find the last occurrence of any test directive
        last_directive_pattern = re.compile(r'^// (TINYC TEST|INFO:|RUN:|EXPECT:|RESULT:|ERROR_TYPE:).*?$', re.MULTILINE)
        last_match = None
        for match in last_directive_pattern.finditer(content):
            last_match = match

        # Extract code after the last directive
        if last_match:
            code_start = last_match.end()
            code_only = content[code_start:].lstrip()
        else:
            # Fallback - just remove all directive lines
            code_only = re.sub(last_directive_pattern, '', content).lstrip()

        return TinyCTest(name, description, code_only, configs)
    except Exception as e:
        print(f"Error parsing test file {file_path}: {e}")
        return None


def run_command(command: str, code: str, temp_file: str = "temp_code.tc") -> Tuple[str, int]:
    """
    Run a command on the provided code and return its output and exit code.

    Args:
        command: Command to run
        code: TinyC code to process
        temp_file: Name of the temporary file to create

    Returns:
        Tuple of (output, exit_code)
    """
    # Create temporary file with the code
    try:
        with open(temp_file, 'w') as f:
            f.write(code)

        # Run the command
        result = subprocess.run(f"{command} {temp_file}", shell=True, capture_output=True, text=True)

        # Combine stdout and stderr for output checking
        output = result.stdout.strip()
        if result.stderr.strip():
            if output:
                output += "\n"
            output += result.stderr.strip()

        return output, result.returncode
    except Exception as e:
        print(f"Error running command: {e}")
        return f"Error: {e}", -1
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


def validate_parser_output(config: TestConfig, actual_output: str, verbose: bool = False) -> Tuple[bool, str]:
    """
    Validate parser output against expected output.

    Args:
        config: Test configuration with expected result
        actual_output: Actual output from the parser
        verbose: Whether to print detailed comparison information

    Returns:
        Tuple of (is_valid, error_message)
    """
    if config.expect == 'SUCCESS':
        # For success tests, validate the JSON output
        try:
            expected_json = json.loads(config.result)
            actual_json = json.loads(actual_output)

            # Do a recursive comparison of the JSON objects
            is_equal, differences = compare_json_objects(expected_json, actual_json)

            if not is_equal and verbose:
                error_msg = "Differences in JSON ASTs:"
                for diff in differences[:10]:  # Show at most 10 differences
                    error_msg += f"\n    - {diff}"
                if len(differences) > 10:
                    error_msg += f"\n    ... and {len(differences) - 10} more differences"
                return False, error_msg

            return is_equal, "" if is_equal else "AST structures differ"
        except json.JSONDecodeError as e:
            return False, f"JSON parse error: {e}"

    elif config.expect == 'ERROR':
        # For error tests, check for the appropriate error message
        lowercase_output = actual_output.lower()

        if config.error_type == 'PARSER_ERROR':
            # Accept various forms of parser error messages
            if any(pattern in lowercase_output for pattern in ['parser error', 'parse error', 'parsing error', 'syntax error']):
                return True, ""
            return False, "Expected parser error but none found in output"

        elif config.error_type == 'LEXER_ERROR':
            # Accept various forms of lexer error messages
            if any(pattern in lowercase_output for pattern in ['lexer error', 'lex error', 'lexical error', 'token error']):
                return True, ""
            return False, "Expected lexer error but none found in output"

        # Other error types can be added as needed
        return False, f"Unknown error type: {config.error_type}"

    else:
        # For non-standard expect values (not SUCCESS or ERROR), try direct comparison
        return config.expect == actual_output.strip(), f"Expected '{config.expect}' but got '{actual_output.strip()}'"


def validate_exec_output(config: TestConfig, actual_output: str, exit_code: int, verbose: bool = False) -> Tuple[bool, str]:
    """
    Validate execution output against expected output.

    Args:
        config: Test configuration with expected result
        actual_output: Actual output from executing the program
        exit_code: Exit code from program execution
        verbose: Whether to print detailed comparison information

    Returns:
        Tuple of (is_valid, error_message)
    """
    if config.expect == 'SUCCESS':
        # For success tests with explicit result, compare output
        if config.result:
            expected_output = config.result.strip()
            actual_output = actual_output.strip()

            if expected_output == actual_output:
                return True, ""
            else:
                # Create a nice diff for the error message
                if verbose:
                    import difflib
                    diff = difflib.unified_diff(
                        expected_output.splitlines(keepends=True),
                        actual_output.splitlines(keepends=True),
                        fromfile='expected',
                        tofile='actual'
                    )
                    return False, "Output differs:\n" + ''.join(diff)
                return False, "Program output did not match expected output"

        # For success tests without explicit result, check exit code is 0
        return exit_code == 0, f"Program exited with non-zero code: {exit_code}" if exit_code != 0 else ""

    elif config.expect == 'ERROR':
        # For error tests, just check that the program produced an error
        if "error" in actual_output.lower() or exit_code != 0:
            return True, ""
        return False, "Expected execution error but program completed successfully"

    else:
        # For other expect values, treat as expected exit code
        try:
            expected_exit = int(config.expect)
            return exit_code == expected_exit, f"Expected exit code {expected_exit} but got {exit_code}"
        except ValueError:
            # If not a number, treat as expected output
            return config.expect == actual_output.strip(), f"Expected output '{config.expect}' but got '{actual_output.strip()}'"


def validate_typechecker_output(config: TestConfig, actual_output: str, verbose: bool = False) -> Tuple[bool, str]:
    """
    Validate type checker output against expected output.

    Args:
        config: Test configuration with expected result
        actual_output: Actual output from the type checker
        verbose: Whether to print detailed comparison information

    Returns:
        Tuple of (is_valid, error_message)
    """
    if config.expect == 'SUCCESS':
        # For success tests, expected output might be empty (no type errors)
        if not config.result and not actual_output:
            return True, ""

        # If expected output is provided, compare it with actual output
        if config.result == actual_output.strip():
            return True, ""
        return False, "Type checker output did not match expected output"

    elif config.expect == 'ERROR':
        # For error tests, check for type error messages
        if "type error" in actual_output.lower() or "type mismatch" in actual_output.lower():
            return True, ""
        return False, "Expected type error but none found in output"

    else:
        # For other expect values, treat as expected output
        return config.expect == actual_output.strip(), f"Expected '{config.expect}' but got '{actual_output.strip()}'"


def get_test_files(test_dir: str, test_num: Optional[int] = None,
                   test_range: Optional[Tuple[int, int]] = None) -> List[str]:
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
            all_test_files.append(file_path)

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
            # No filter, include all files
            filtered_files.append(file_path)

    # Sort test files by their numeric prefix
    return sorted(filtered_files, key=extract_number_prefix)


def run_tests(base_command: str, test_dir: str, test_num: Optional[int] = None,
              test_range: Optional[Tuple[int, int]] = None, run_type_filter: Optional[str] = None,
              verbose: bool = False) -> Tuple[int, int]:
    """
    Run tests against the TinyC compiler.

    Args:
        base_command: Base command to run (will be modified based on run type)
        test_dir: Directory containing test files
        test_num: If provided, only run the test with this number
        test_range: If provided, only run tests in this range (start, end) inclusive
        run_type_filter: If provided, only run test configs with this run type
        verbose: Whether to print detailed comparison information

    Returns:
        Tuple of (passed_count, failed_count)
    """
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
    total_configs = 0

    # Create a message about which tests we're running
    test_msg = f"{len(test_files)} test files"
    if test_num is not None:
        test_msg = f"test file {test_num}"
    elif test_range is not None:
        test_msg = f"test files {test_range[0]}-{test_range[1]}"

    print(f"Running {test_msg}...")

    # Set up validation functions for different test types
    validation_functions = {
        'parser': validate_parser_output,
        'exec': validate_exec_output,
        'typechecker': validate_typechecker_output
    }

    # Command-line argument mapping for different run types
    command_args = {
        'parser': '--parse',
        'exec': '--run',
        'typechecker': '--typecheck'
    }

    for i, test_file in enumerate(test_files, 1):
        test = parse_test_file(test_file)
        if not test:
            print(f"Skipping invalid test file: {test_file}")
            continue

        print(f"\nTest file {i}/{len(test_files)}: {test.name}")
        print(f"  Description: {test.description}")

        # Count configs that match the filter
        filtered_configs = [c for c in test.configs if run_type_filter is None or c.run_type == run_type_filter]
        total_configs += len(filtered_configs)

        if not filtered_configs:
            if run_type_filter:
                print(f"  Skipping - no configurations for run type '{run_type_filter}'")
            else:
                print(f"  Warning: No valid test configurations found")
            continue

        # Run each test configuration
        for j, config in enumerate(filtered_configs, 1):
            print(f"  Configuration {j}/{len(filtered_configs)}: {config.run_type} (Expect: {config.expect})")

            # Choose the appropriate command based on the run type
            cmd_arg = command_args.get(config.run_type, '')
            cmd = f"{base_command} {cmd_arg}".strip()

            # Run the command on the test code
            actual_output, exit_code = run_command(cmd, test.code)

            # Check for catastrophic failure
            if not actual_output and exit_code != 0 and config.expect == 'SUCCESS':
                print(f"    ❌ FAILED (Command failed with exit code {exit_code})")
                failed += 1
                continue

            # Get the appropriate validation function
            validation_func = validation_functions.get(config.run_type)
            if not validation_func:
                print(f"    ❌ FAILED (No validation function for run type: {config.run_type})")
                failed += 1
                continue

            # Validate the output
            if config.run_type == 'exec':
                passed_test, error_msg = validation_func(config, actual_output, exit_code, verbose)
            else:
                passed_test, error_msg = validation_func(config, actual_output, verbose)

            if passed_test:
                print(f"    ✅ PASSED")
                passed += 1
            else:
                print(f"    ❌ FAILED")
                if error_msg:
                    print(f"    {error_msg}")

                # Show preview of expected/actual (for non-verbose mode)
                if not verbose:
                    if config.expect == 'SUCCESS' and config.result:
                        expected_preview = config.result[:80] + "..." if len(config.result) > 80 else config.result
                        actual_preview = actual_output[:80] + "..." if len(actual_output) > 80 else actual_output
                        print(f"    Expected: {expected_preview}")
                        print(f"    Actual  : {actual_preview}")
                    else:
                        print(f"    Actual output: {actual_output[:80]}" + ("..." if len(actual_output) > 80 else ""))

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
    parser = argparse.ArgumentParser(description='TinyC Test Runner')
    parser.add_argument('command', help='Base command to run the compiler (e.g., "./tinyc-compiler")')
    parser.add_argument('--test-dir', '-d', default='tests', help='Directory containing test files (default: tests)')
    parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose output with detailed differences')

    # Add optional filter for run type
    parser.add_argument('--run-type', '-rt', help='Only run test configurations of a specific type')

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

    passed, failed = run_tests(args.command, args.test_dir, args.test, test_range, args.run_type, args.verbose)

    print("\n" + "="*50)
    print(f"Test Results: {passed} passed, {failed} failed")
    print(f"Overall: {'✅ PASSED' if failed == 0 else '❌ FAILED'}")
    print("="*50)

    # Return non-zero exit code if any tests failed
    return 1 if failed > 0 else 0


if __name__ == "__main__":
    sys.exit(main())