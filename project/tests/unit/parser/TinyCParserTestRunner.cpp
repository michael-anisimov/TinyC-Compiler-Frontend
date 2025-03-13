#include <gtest/gtest.h>
#include <iostream>

/**
 * @file TinyCParserTestRunner.cpp
 * @brief Main entry point for running TinyC parser tests
 *
 * This test runner executes all test suites for the TinyC parser.
 * It can be used to run specific test categories based on command-line options.
 */

void printUsage() {
    std::cout << "TinyC Parser Test Runner" << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << "Usage: TinyCParserTests [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help           Show this help message" << std::endl;
    std::cout << "  --list-tests     List all available tests" << std::endl;
    std::cout << "  --type           Run only type-related tests" << std::endl;
    std::cout << "  --expression     Run only expression-related tests" << std::endl;
    std::cout << "  --statement      Run only statement-related tests" << std::endl;
    std::cout << "  --declaration    Run only declaration-related tests" << std::endl;
    std::cout << "  --program        Run only program-related tests" << std::endl;
    std::cout << "  --error          Run only error-handling tests" << std::endl;
    std::cout << "  --verbose        Run with verbose output" << std::endl;
}

int main(int argc, char **argv) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // Process custom command-line options
    bool showHelp = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help") {
            showHelp = true;
        } else if (arg == "--type") {
            ::testing::GTEST_FLAG(filter) = "ParserTypeTest.*";
        } else if (arg == "--expression") {
            ::testing::GTEST_FLAG(filter) = "ParserExpressionTest.*";
        } else if (arg == "--statement") {
            ::testing::GTEST_FLAG(filter) = "ParserStatementTest.*";
        } else if (arg == "--declaration") {
            ::testing::GTEST_FLAG(filter) = "ParserDeclarationTest.*";
        } else if (arg == "--program") {
            ::testing::GTEST_FLAG(filter) = "ParserProgramTest.*";
        } else if (arg == "--error") {
            ::testing::GTEST_FLAG(filter) = "ParserErrorTest.*";
        } else if (arg == "--verbose") {
            ::testing::GTEST_FLAG(verbose) = "true";
        }
    }

    if (showHelp) {
        printUsage();
        return 0;
    }

    std::cout << "Running TinyC Parser Tests..." << std::endl;

    // Run all tests
    return RUN_ALL_TESTS();
}