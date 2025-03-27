#include "tinyc/parser/Parser.h"
#include "tinyc/lexer/Lexer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <memory>

using namespace tinyc;

// Helper function to parse a string and return the AST
ast::ASTNodePtr parseString(const std::string &source) {
	lexer::Lexer lexer(source);
	parser::Parser parser(lexer);
	return parser.parseProgram();
}

// Test empty program
TEST(ParserTest, EmptyProgram) {
	auto ast = parseString("");

	// Check that we got a program node
	auto program = std::dynamic_pointer_cast<ast::ProgramNode>(ast);
	ASSERT_NE(program, nullptr);

	// An empty program should have no declarations
	EXPECT_EQ(program->getDeclarations().size(), 0);
}

// Test variable declarations
TEST(ParserTest, VariableDeclarations) {
	auto ast = parseString("int x; double y = 3.14; char z = 'a';");

	// Check that we got a program node
	auto program = std::dynamic_pointer_cast<ast::ProgramNode>(ast);
	ASSERT_NE(program, nullptr);

	// Should have 3 declarations
	ASSERT_EQ(program->getDeclarations().size(), 3);

	// Check first declaration (int x)
	auto intDecl = std::dynamic_pointer_cast<ast::VariableNode>(program->getDeclarations()[0]);
	ASSERT_NE(intDecl, nullptr);
	EXPECT_EQ(intDecl->getIdentifier(), "x");
	EXPECT_FALSE(intDecl->hasInitializer());

	// Check second declaration (double y = 3.14)
	auto doubleDecl = std::dynamic_pointer_cast<ast::VariableNode>(program->getDeclarations()[1]);
	ASSERT_NE(doubleDecl, nullptr);
	EXPECT_EQ(doubleDecl->getIdentifier(), "y");
	EXPECT_TRUE(doubleDecl->hasInitializer());

	// Check third declaration (char z = 'a')
	auto charDecl = std::dynamic_pointer_cast<ast::VariableNode>(program->getDeclarations()[2]);
	ASSERT_NE(charDecl, nullptr);
	EXPECT_EQ(charDecl->getIdentifier(), "z");
	EXPECT_TRUE(charDecl->hasInitializer());
}

// Test function declarations
TEST(ParserTest, FunctionDeclarations) {
	auto ast = parseString("int add(int a, int b); void print(char *msg);");

	// Check that we got a program node
	auto program = std::dynamic_pointer_cast<ast::ProgramNode>(ast);
	ASSERT_NE(program, nullptr);

	// Should have 2 declarations
	ASSERT_EQ(program->getDeclarations().size(), 2);

	// Check first declaration (int add(int a, int b))
	auto addFunc = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(program->getDeclarations()[0]);
	ASSERT_NE(addFunc, nullptr);
	EXPECT_EQ(addFunc->getIdentifier(), "add");
	EXPECT_FALSE(addFunc->isDefinition());
	ASSERT_EQ(addFunc->getParameters().size(), 2);

	// Check second declaration (void print(char *msg))
	auto printFunc = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(program->getDeclarations()[1]);
	ASSERT_NE(printFunc, nullptr);
	EXPECT_EQ(printFunc->getIdentifier(), "print");
	EXPECT_FALSE(printFunc->isDefinition());
	ASSERT_EQ(printFunc->getParameters().size(), 1);
}

// Test function definition
TEST(ParserTest, FunctionDefinition) {
	auto ast = parseString(R"(
        int max(int a, int b) {
            if (a > b) {
                return a;
            } else {
                return b;
            }
        }
    )");

	// Check that we got a program node
	auto program = std::dynamic_pointer_cast<ast::ProgramNode>(ast);
	ASSERT_NE(program, nullptr);

	// Should have 1 declaration
	ASSERT_EQ(program->getDeclarations().size(), 1);

	// Check function declaration
	auto maxFunc = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(program->getDeclarations()[0]);
	ASSERT_NE(maxFunc, nullptr);
	EXPECT_EQ(maxFunc->getIdentifier(), "max");
	EXPECT_TRUE(maxFunc->isDefinition());
	ASSERT_EQ(maxFunc->getParameters().size(), 2);

	// Check function body
	auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(maxFunc->getBody());
	ASSERT_NE(body, nullptr);
	ASSERT_EQ(body->getStatements().size(), 1);

	// Check if statement
	auto ifStmt = std::dynamic_pointer_cast<ast::IfStatementNode>(body->getStatements()[0]);
	ASSERT_NE(ifStmt, nullptr);
	EXPECT_TRUE(ifStmt->hasElseBranch());
}

// Test expressions
TEST(ParserTest, Expressions) {
	auto ast = parseString(R"(
        void test() {
            int a = 1 + 2 * 3;
            int b = (4 + 5) * 6;
            int d = a & b | c;
        }
    )");

	// Check that we got a program node
	auto program = std::dynamic_pointer_cast<ast::ProgramNode>(ast);
	ASSERT_NE(program, nullptr);

	// Should have 1 declaration
	ASSERT_EQ(program->getDeclarations().size(), 1);

	// Check function declaration
	auto testFunc = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(program->getDeclarations()[0]);
	ASSERT_NE(testFunc, nullptr);
	EXPECT_EQ(testFunc->getIdentifier(), "test");
	EXPECT_TRUE(testFunc->isDefinition());

	// Check function body
	auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(testFunc->getBody());
	ASSERT_NE(body, nullptr);
	ASSERT_EQ(body->getStatements().size(), 3);
}

// Test struct declaration
TEST(ParserTest, StructDeclaration) {
	auto ast = parseString(R"(
        struct Point {
            int x;
            int y;
        };
        
        struct Point3D;  // Forward declaration
    )");

	// Check that we got a program node
	auto program = std::dynamic_pointer_cast<ast::ProgramNode>(ast);
	ASSERT_NE(program, nullptr);

	// Should have 2 declarations
	ASSERT_EQ(program->getDeclarations().size(), 2);

	// Check first declaration (struct Point)
	auto pointStruct = std::dynamic_pointer_cast<ast::StructDeclarationNode>(program->getDeclarations()[0]);
	ASSERT_NE(pointStruct, nullptr);
	EXPECT_EQ(pointStruct->getIdentifier(), "Point");
	EXPECT_TRUE(pointStruct->isDefinition());
	ASSERT_EQ(pointStruct->getFields().size(), 2);

	// Check second declaration (struct Point3D)
	auto point3dStruct = std::dynamic_pointer_cast<ast::StructDeclarationNode>(program->getDeclarations()[1]);
	ASSERT_NE(point3dStruct, nullptr);
	EXPECT_EQ(point3dStruct->getIdentifier(), "Point3D");
	EXPECT_FALSE(point3dStruct->isDefinition());
	ASSERT_EQ(point3dStruct->getFields().size(), 0);
}

// Test function pointer declaration
TEST(ParserTest, FunctionPointerDeclaration) {
	auto ast = parseString("typedef int (*Comparator)(int, int);");

	// Check that we got a program node
	auto program = std::dynamic_pointer_cast<ast::ProgramNode>(ast);
	ASSERT_NE(program, nullptr);

	// Should have 1 declaration
	ASSERT_EQ(program->getDeclarations().size(), 1);

	// Check function pointer declaration
	auto funcPtr = std::dynamic_pointer_cast<ast::FunctionPointerDeclarationNode>(program->getDeclarations()[0]);
	ASSERT_NE(funcPtr, nullptr);
	EXPECT_EQ(funcPtr->getIdentifier(), "Comparator");
	ASSERT_EQ(funcPtr->getParameterTypes().size(), 2);
}

// Test errors
TEST(ParserTest, Errors) {
	// Missing semicolon
	EXPECT_THROW({ parseString("int x"); }, parser::ParserError);

	// Unmatched parenthesis
	EXPECT_THROW({ parseString("int f() { return (1 + 2; }"); }, parser::ParserError);

	// Invalid declaration
	EXPECT_THROW({ parseString("x = 10;"); }, parser::ParserError);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}