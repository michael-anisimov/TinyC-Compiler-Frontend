#include "tinyc/parser/Parser.h"
#include "tinyc/lexer/Lexer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <memory>

using namespace tinyc;

/**
 * @file ParserDeclarationTests.cpp
 * @brief Tests for TinyC declaration parsing
 *
 * This file contains tests for declaration parsing in TinyC, organized by declaration
 * type. These tests verify the parser correctly builds AST nodes for all TinyC
 * declaration types.
 */

namespace {
	// Helper function to parse a declaration
	ast::ASTNodePtr parseDeclaration(const std::string &source) {
		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);
		auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
		EXPECT_NE(program, nullptr);
		EXPECT_EQ(program->getDeclarations().size(), 1);
		return program->getDeclarations()[0];
	}
}

/**
 * @test VariableDeclarations
 * @brief Test global variable declarations
 *
 * Grammar rules tested:
 * - PROGRAM_ITEM -> NON_VOID_TYPE identifier VARIABLE_TAIL
 * - VARIABLE_TAIL -> OPT_ARRAY_SIZE OPT_INIT MORE_GLOBAL_VARS ;
 */
TEST(ParserDeclarationTest, VariableDeclarations) {
	// Test simple variable declaration
	{
		auto decl = parseDeclaration("int counter;");
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "counter");

		auto type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(varDecl->getType());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		EXPECT_FALSE(varDecl->hasInitializer());
	}

	// Test variable declaration with initializer
	{
		auto decl = parseDeclaration("double pi = 3.14159;");
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "pi");

		auto type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(varDecl->getType());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::DOUBLE);

		EXPECT_TRUE(varDecl->hasInitializer());
		auto initializer = std::dynamic_pointer_cast<ast::LiteralNode>(varDecl->getInitializer());
		ASSERT_NE(initializer, nullptr);
		EXPECT_EQ(initializer->getKind(), ast::LiteralNode::Kind::DOUBLE);
	}

	// Test pointer variable declaration
	{
		auto decl = parseDeclaration("char* str;");
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "str");

		auto type = std::dynamic_pointer_cast<ast::PointerTypeNode>(varDecl->getType());
		ASSERT_NE(type, nullptr);

		auto baseType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(type->getBaseType());
		ASSERT_NE(baseType, nullptr);
		EXPECT_EQ(baseType->getKind(), ast::PrimitiveTypeNode::Kind::CHAR);
	}

	// Test array variable declaration
	{
		auto decl = parseDeclaration("int numbers[10];");
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "numbers");

		auto type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(varDecl->getType());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_NE(varDecl->getArraySize(), nullptr);
		auto arraySize = std::dynamic_pointer_cast<ast::LiteralNode>(varDecl->getArraySize());
		ASSERT_NE(arraySize, nullptr);
		EXPECT_EQ(arraySize->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(arraySize->getValue(), "10");
	}

	// Test void pointer declaration
	{
		auto decl = parseDeclaration("void* ptr;");
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "ptr");

		auto type = std::dynamic_pointer_cast<ast::PointerTypeNode>(varDecl->getType());
		ASSERT_NE(type, nullptr);

		auto baseType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(type->getBaseType());
		ASSERT_NE(baseType, nullptr);
		EXPECT_EQ(baseType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);
	}
}

/**
 * @test FunctionDeclarations
 * @brief Test function declarations
 *
 * Grammar rules tested:
 * - PROGRAM_ITEM -> NON_VOID_TYPE identifier FUNCTION_DECLARATION_TAIL
 * - PROGRAM_ITEM -> void VOID_DECL_TAIL
 * - FUNCTION_DECLARATION_TAIL -> ( OPT_FUN_ARGS ) FUNC_TAIL
 */
TEST(ParserDeclarationTest, FunctionDeclarations) {
	// Test function declaration with no parameters
	{
		auto decl = parseDeclaration("int getNumber();");
		auto funcDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getName(), "getNumber");

		auto returnType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		EXPECT_EQ(funcDecl->getParameters().size(), 0);
		EXPECT_FALSE(funcDecl->isDefinition());
	}

	// Test function declaration with parameters
	{
		auto decl = parseDeclaration("int add(int a, int b);");
		auto funcDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getName(), "add");

		auto returnType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_EQ(funcDecl->getParameters().size(), 2);

		// Check first parameter
		auto param1 = std::dynamic_pointer_cast<ast::ParameterNode>(funcDecl->getParameters()[0]);
		ASSERT_NE(param1, nullptr);
		EXPECT_EQ(param1->getName(), "a");

		auto param1Type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(param1->getType());
		ASSERT_NE(param1Type, nullptr);
		EXPECT_EQ(param1Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		// Check second parameter
		auto param2 = std::dynamic_pointer_cast<ast::ParameterNode>(funcDecl->getParameters()[1]);
		ASSERT_NE(param2, nullptr);
		EXPECT_EQ(param2->getName(), "b");
	}

	// Test void function declaration
	{
		auto decl = parseDeclaration("void logMessage(char* msg);");
		auto funcDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getName(), "logMessage");

		auto returnType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		ASSERT_EQ(funcDecl->getParameters().size(), 1);

		// Check parameter
		auto param = std::dynamic_pointer_cast<ast::ParameterNode>(funcDecl->getParameters()[0]);
		ASSERT_NE(param, nullptr);
		EXPECT_EQ(param->getName(), "msg");

		auto paramType = std::dynamic_pointer_cast<ast::PointerTypeNode>(param->getType());
		ASSERT_NE(paramType, nullptr);

		auto baseType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(paramType->getBaseType());
		ASSERT_NE(baseType, nullptr);
		EXPECT_EQ(baseType->getKind(), ast::PrimitiveTypeNode::Kind::CHAR);
	}

	// Test function declaration with complex return type
	{
		auto decl = parseDeclaration("int* createArray(int size);");
		auto funcDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getName(), "createArray");

		auto returnType = std::dynamic_pointer_cast<ast::PointerTypeNode>(funcDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);

		auto baseType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(returnType->getBaseType());
		ASSERT_NE(baseType, nullptr);
		EXPECT_EQ(baseType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_EQ(funcDecl->getParameters().size(), 1);
	}
}

/**
 * @test FunctionDefinitions
 * @brief Test function definitions
 *
 * Grammar rules tested:
 * - FUNCTION_DECLARATION_TAIL -> ( OPT_FUN_ARGS ) BLOCK_STMT
 */
TEST(ParserDeclarationTest, FunctionDefinitions) {
	// Test simple function definition
	{
		auto decl = parseDeclaration("int identity(int x) { return x; }");
		auto funcDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getName(), "identity");

		auto returnType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_EQ(funcDecl->getParameters().size(), 1);
		EXPECT_TRUE(funcDecl->isDefinition());

		auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(funcDecl->getBody());
		ASSERT_NE(body, nullptr);
		ASSERT_EQ(body->getStatements().size(), 1);

		auto returnStmt = std::dynamic_pointer_cast<ast::ReturnStatementNode>(body->getStatements()[0]);
		ASSERT_NE(returnStmt, nullptr);
		EXPECT_TRUE(returnStmt->hasValue());
	}

	// Test function definition with multiple statements
	{
		auto decl = parseDeclaration(R"(
            int max(int a, int b) {
                if (a > b) {
                    return a;
                } else {
                    return b;
                }
            }
        )");
		auto funcDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getName(), "max");

		ASSERT_EQ(funcDecl->getParameters().size(), 2);
		EXPECT_TRUE(funcDecl->isDefinition());

		auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(funcDecl->getBody());
		ASSERT_NE(body, nullptr);
		ASSERT_EQ(body->getStatements().size(), 1);

		auto ifStmt = std::dynamic_pointer_cast<ast::IfStatementNode>(body->getStatements()[0]);
		ASSERT_NE(ifStmt, nullptr);
		EXPECT_TRUE(ifStmt->hasElseBranch());
	}

	// Test void function definition
	{
		auto decl = parseDeclaration(R"(
            void swap(int* a, int* b) {
                int temp = *a;
                *a = *b;
                *b = temp;
            }
        )");
		auto funcDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getName(), "swap");

		auto returnType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		ASSERT_EQ(funcDecl->getParameters().size(), 2);
		EXPECT_TRUE(funcDecl->isDefinition());

		auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(funcDecl->getBody());
		ASSERT_NE(body, nullptr);
		ASSERT_EQ(body->getStatements().size(), 3);
	}
}

/**
 * @test StructDeclarations
 * @brief Test struct declarations
 *
 * Grammar rules tested:
 * - PROGRAM_ITEM -> STRUCT_DECL
 * - STRUCT_DECL -> struct identifier [ '{' { TYPE identifier ';' } '}' ] ';'
 */
TEST(ParserDeclarationTest, StructDeclarations) {
	// Test struct declaration
	{
		auto decl = parseDeclaration("struct Point { int x; int y; };");
		auto structDecl = std::dynamic_pointer_cast<ast::StructDeclarationNode>(decl);
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getName(), "Point");
		EXPECT_TRUE(structDecl->isDefinition());
		ASSERT_EQ(structDecl->getFields().size(), 2);

		// Check first field
		auto field1 = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(structDecl->getFields()[0]);
		ASSERT_NE(field1, nullptr);
		EXPECT_EQ(field1->getName(), "x");

		auto field1Type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(field1->getType());
		ASSERT_NE(field1Type, nullptr);
		EXPECT_EQ(field1Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		// Check second field
		auto field2 = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(structDecl->getFields()[1]);
		ASSERT_NE(field2, nullptr);
		EXPECT_EQ(field2->getName(), "y");
	}

	// Test struct forward declaration
	{
		auto decl = parseDeclaration("struct Node;");
		auto structDecl = std::dynamic_pointer_cast<ast::StructDeclarationNode>(decl);
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getName(), "Node");
		EXPECT_FALSE(structDecl->isDefinition());
		EXPECT_EQ(structDecl->getFields().size(), 0);
	}

	// Test struct with complex fields
	{
		auto decl = parseDeclaration("struct LinkedList { int value; struct LinkedList* next; };");
		auto structDecl = std::dynamic_pointer_cast<ast::StructDeclarationNode>(decl);
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getName(), "LinkedList");
		EXPECT_TRUE(structDecl->isDefinition());
		ASSERT_EQ(structDecl->getFields().size(), 2);

		// Check first field
		auto field1 = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(structDecl->getFields()[0]);
		ASSERT_NE(field1, nullptr);
		EXPECT_EQ(field1->getName(), "value");

		// Check second field (pointer)
		auto field2 = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(structDecl->getFields()[1]);
		ASSERT_NE(field2, nullptr);
		EXPECT_EQ(field2->getName(), "next");

		auto field2Type = std::dynamic_pointer_cast<ast::PointerTypeNode>(field2->getType());
		ASSERT_NE(field2Type, nullptr);
	}
}

/**
 * @test FunctionPointerDeclarations
 * @brief Test function pointer declarations
 *
 * Grammar rules tested:
 * - PROGRAM_ITEM -> FUNPTR_DECL
 * - FUNPTR_DECL -> typedef TYPE_FUN_RET '(' '*' identifier ')' '(' [ TYPE { ',' TYPE } ] ')' ';'
 */
TEST(ParserDeclarationTest, FunctionPointerDeclarations) {
	// Test function pointer with no parameters
	{
		auto decl = parseDeclaration("typedef void (*Callback)();");
		auto funcPtrDecl = std::dynamic_pointer_cast<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getName(), "Callback");

		auto returnType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcPtrDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		EXPECT_EQ(funcPtrDecl->getParameterTypes().size(), 0);
	}

	// Test function pointer with parameters
	{
		auto decl = parseDeclaration("typedef int (*Comparator)(int, int);");
		auto funcPtrDecl = std::dynamic_pointer_cast<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getName(), "Comparator");

		auto returnType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcPtrDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_EQ(funcPtrDecl->getParameterTypes().size(), 2);

		// Check parameter types
		auto param1Type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcPtrDecl->getParameterTypes()[0]);
		ASSERT_NE(param1Type, nullptr);
		EXPECT_EQ(param1Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		auto param2Type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcPtrDecl->getParameterTypes()[1]);
		ASSERT_NE(param2Type, nullptr);
		EXPECT_EQ(param2Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test function pointer with complex types
	{
		auto decl = parseDeclaration("typedef void* (*Allocator)(int, void*);");
		auto funcPtrDecl = std::dynamic_pointer_cast<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getName(), "Allocator");

		// Check return type (void*)
		auto returnType = std::dynamic_pointer_cast<ast::PointerTypeNode>(funcPtrDecl->getReturnType());
		ASSERT_NE(returnType, nullptr);

		auto returnBaseType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(returnType->getBaseType());
		ASSERT_NE(returnBaseType, nullptr);
		EXPECT_EQ(returnBaseType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		ASSERT_EQ(funcPtrDecl->getParameterTypes().size(), 2);

		// Check first parameter type (int)
		auto param1Type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(funcPtrDecl->getParameterTypes()[0]);
		ASSERT_NE(param1Type, nullptr);
		EXPECT_EQ(param1Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		// Check second parameter type (void*)
		auto param2Type = std::dynamic_pointer_cast<ast::PointerTypeNode>(funcPtrDecl->getParameterTypes()[1]);
		ASSERT_NE(param2Type, nullptr);

		auto param2BaseType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(param2Type->getBaseType());
		ASSERT_NE(param2BaseType, nullptr);
		EXPECT_EQ(param2BaseType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);
	}
}

/**
 * @test MultipleDeclarations
 * @brief Test multiple declarations in a single program
 */
TEST(ParserDeclarationTest, MultipleDeclarations) {
	// Test multiple declarations
	std::string source = R"(
        struct Point {
            int x;
            int y;
        };

        typedef int (*Comparator)(int, int);

        int compare(int a, int b);

        int main() {
            int result = compare(10, 20);
            return result;
        }
    )";

	lexer::Lexer lexer(source);
	parser::Parser parser(lexer);
	auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
	ASSERT_NE(program, nullptr);
	ASSERT_EQ(program->getDeclarations().size(), 4);

	// Check struct declaration
	auto structDecl = std::dynamic_pointer_cast<ast::StructDeclarationNode>(program->getDeclarations()[0]);
	ASSERT_NE(structDecl, nullptr);
	EXPECT_EQ(structDecl->getName(), "Point");

	// Check function pointer declaration
	auto funcPtrDecl = std::dynamic_pointer_cast<ast::FunctionPointerDeclarationNode>(program->getDeclarations()[1]);
	ASSERT_NE(funcPtrDecl, nullptr);
	EXPECT_EQ(funcPtrDecl->getName(), "Comparator");

	// Check function declaration
	auto funcDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(program->getDeclarations()[2]);
	ASSERT_NE(funcDecl, nullptr);
	EXPECT_EQ(funcDecl->getName(), "compare");
	EXPECT_FALSE(funcDecl->isDefinition());

	// Check function definition
	auto mainDecl = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(program->getDeclarations()[3]);
	ASSERT_NE(mainDecl, nullptr);
	EXPECT_EQ(mainDecl->getName(), "main");
	EXPECT_TRUE(mainDecl->isDefinition());
}