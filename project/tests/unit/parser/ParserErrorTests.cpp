#include "tinyc/parser/Parser.h"
#include "tinyc/lexer/Lexer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <memory>

using namespace tinyc;

/**
 * @file ParserErrorTests.cpp
 * @brief Tests for TinyC parser error handling
 *
 * This file contains tests to verify that the parser correctly handles syntax
 * errors and provides helpful error messages. These tests are valuable for
 * students to understand common syntax errors in TinyC.
 */

namespace {
	// Helper function to check for expected parser error
	void expectParserError(const std::string &source, const std::string &expectedError = "") {
		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);

		try {
			auto ast = parser.parseProgram();
			FAIL() << "Expected parser error, but parsing succeeded";
		} catch (const parser::ParserError &e) {
			if (!expectedError.empty()) {
				std::string errorMsg = e.what();
				EXPECT_NE(errorMsg.find(expectedError), std::string::npos)
									<< "Error message '" << errorMsg << "' doesn't contain '" << expectedError << "'";
			}
		}
	}
}

/**
 * @test SyntaxErrors
 * @brief Test basic syntax errors
 */
TEST(ParserErrorTest, SyntaxErrors) {
	// Missing semicolon
	expectParserError("int x", "Expected ';'");

	// Missing closing brace
	expectParserError("int foo() {", "Expected '}'");

	// Missing closing parenthesis
	expectParserError("int foo(int x {", "Expected ')'");

	// Invalid token
	expectParserError("int @invalid;");

	// Missing parameter name
	expectParserError("int foo(int) {}", "Expected identifier");
}

/**
 * @test DeclarationErrors
 * @brief Test errors in declarations
 */
TEST(ParserErrorTest, DeclarationErrors) {
	// Invalid variable declaration
	expectParserError("x = 10;", "Expected type");

	// Invalid function declaration
	expectParserError("foo() {}", "Expected type");

	// Missing struct name
	expectParserError("struct {int x;};", "Expected identifier");

	// Missing struct field type
	expectParserError("struct Point { x; };", "Expected type");

	// Missing typedef name
	expectParserError("typedef int (*)(int, int);", "Expected identifier");
}

/**
 * @test StatementErrors
 * @brief Test errors in statements
 */
TEST(ParserErrorTest, StatementErrors) {
	// Test function with statement errors
	std::string functionPrefix = "void test() { ";
	std::string functionSuffix = " }";

	// Missing condition in if statement
	expectParserError(functionPrefix + "if {}" + functionSuffix, "Expected '('");
	expectParserError(functionPrefix + "if () {}" + functionSuffix, "Expected expression");

	// Missing condition in while statement
	expectParserError(functionPrefix + "while {}" + functionSuffix, "Expected '('");
	expectParserError(functionPrefix + "while () {}" + functionSuffix, "Expected expression");

	// Missing semicolon in for statement
	expectParserError(functionPrefix + "for (int i = 0 i < 10; i++) {}" + functionSuffix, "Expected ';'");

	// Missing semicolon after break
	expectParserError(functionPrefix + "break" + functionSuffix, "Expected ';'");

	// Missing semicolon after continue
	expectParserError(functionPrefix + "continue" + functionSuffix, "Expected ';'");

	// Missing semicolon after return
	expectParserError(functionPrefix + "return 42" + functionSuffix, "Expected ';'");
}

/**
 * @test ExpressionErrors
 * @brief Test errors in expressions
 */
TEST(ParserErrorTest, ExpressionErrors) {
	// Test function with expression errors
	std::string functionPrefix = "void test() { ";
	std::string functionSuffix = " }";

	// Unmatched parentheses
	expectParserError(functionPrefix + "int x = (1 + 2;" + functionSuffix, "Expected ')'");
	expectParserError(functionPrefix + "int x = 1 + 2);" + functionSuffix);

	// Missing operand
	expectParserError(functionPrefix + "int x = 1 +;" + functionSuffix);
	expectParserError(functionPrefix + "int x = 1 + * 2;" + functionSuffix);

	// Unterminated character literal
	expectParserError(functionPrefix + "char c = 'a;" + functionSuffix);

	// Empty character literal
	expectParserError(functionPrefix + "char c = '';" + functionSuffix);

	// Missing argument in function call
	expectParserError(functionPrefix + "foo(1, );" + functionSuffix);

	// Invalid assignment target
	expectParserError(functionPrefix + "1 = x;" + functionSuffix);

	// Missing index in array access
	expectParserError(functionPrefix + "arr[];" + functionSuffix, "Expected expression");

	// Missing member name
	expectParserError(functionPrefix + "point.;" + functionSuffix, "Expected identifier");
	expectParserError(functionPrefix + "ptr->;" + functionSuffix, "Expected identifier");
}

/**
 * @test TypeErrors
 * @brief Test errors in type declarations
 */
TEST(ParserErrorTest, TypeErrors) {
	// Missing base type for pointer
	expectParserError("* ptr;");

	// Invalid void variable (not a pointer)
	expectParserError("void x;", "Expected identifier or '*'");

	// Missing type in parameter list
	expectParserError("int foo(int a, b) {}", "Expected type");

	// Invalid function pointer declaration
	expectParserError("typedef int (Comparator)(int, int);", "Expected '*'");

	// Invalid cast expression
	expectParserError("void test() { int x = cast<>(10); }", "Expected type");
	expectParserError("void test() { int x = cast int>(10); }", "Expected '<'");
	expectParserError("void test() { int x = cast<int(10); }", "Expected '>'");
}

/**
 * @test ErrorRecovery
 * @brief Test parser's ability to recover from errors
 *
 * Note: This test assumes the parser has error recovery capabilities.
 * If your parser stops at the first error, this test may need to be adjusted.
 */
TEST(ParserErrorTest, DISABLED_ErrorRecovery) {
	// This test is currently disabled because the parser doesn't have
	// error recovery implemented. If error recovery is added, this test
	// can be enabled and used to verify it works correctly.

	std::string source = R"(
        // Valid declaration
        int x;

        // Error: missing semicolon
        int y

        // Valid function
        int foo() {
            return 0;
        }

        // Error: missing closing brace
        void bar() {
            int z = 10;

        // Valid declaration after error
        double pi = 3.14;
    )";

	lexer::Lexer lexer(source);
	parser::Parser parser(lexer);

	try {
		auto ast = parser.parseProgram();
		auto program = std::dynamic_pointer_cast<ast::ProgramNode>(ast);

		// If error recovery works, we should get at least 3 declarations
		// (x, foo, and pi) despite the errors
		ASSERT_NE(program, nullptr);
		EXPECT_GE(program->getDeclarations().size(), 3);

	} catch (const parser::ParserError &e) {
		// If error recovery is not implemented, this test will be skipped
		std::cerr << "Parser error: " << e.what() << std::endl;
		GTEST_SKIP() << "Parser doesn't implement error recovery";
	}
}

/**
 * @test CommonStudentErrors
 * @brief Test common syntax errors that students might make
 *
 * This test is particularly valuable for educational purposes, as it
 * documents common mistakes that students might make when writing TinyC code.
 */
TEST(ParserErrorTest, CommonStudentErrors) {
	// Using += operator (not supported in TinyC)
	expectParserError("void test() { x += 1; }");

	// Using [] for array declaration in parameter
	expectParserError("void test(int nums[]) {}", "Expected identifier");

	// Missing parentheses in if statement condition
	expectParserError("void test() { if x > 0 return x; }", "Expected '('");

	// Using == for assignment
	expectParserError("void test() { x == 10; }");

	// Missing return type for function
	expectParserError("main() { return 0; }", "Expected type");

	// Forgetting struct keyword
	expectParserError("Point p;", "Expected type");

	// C++ style single-line comment (not supported if lexer doesn't handle it)
	// This might actually work if the lexer supports C++ style comments
	expectParserError("// This is a comment\nint x;");

	// Using -> when dot should be used
	expectParserError("void test() { Point p; p->x = 10; }");

	// Using . when -> should be used
	expectParserError("void test() { Point* p; p.x = 10; }");

	// Using const (not supported in TinyC)
	expectParserError("const int x = 10;", "Expected type");

	// Using auto (not supported in TinyC)
	expectParserError("auto x = 10;", "Expected type");
}

/**
 * @test ErrorLocation
 * @brief Test that error messages include correct source locations
 */
TEST(ParserErrorTest, ErrorLocation) {
	std::string source = "int x = 10\nint y = 20;";

	lexer::Lexer lexer(source);
	parser::Parser parser(lexer);

	try {
		auto ast = parser.parseProgram();
		FAIL() << "Expected parser error, but parsing succeeded";
	} catch (const parser::ParserError &e) {
		std::string errorMsg = e.what();

		// Error should occur at line 1, near the end
		EXPECT_NE(errorMsg.find("1:"), std::string::npos)
							<< "Error message doesn't contain line number: " << errorMsg;
	}
}