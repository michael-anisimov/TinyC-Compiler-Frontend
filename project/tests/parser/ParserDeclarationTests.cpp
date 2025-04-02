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
	// Helper function to cast a node to a specific type
	template<typename T>
	T* as(const ast::ASTNode* node) {
		return dynamic_cast<T*>(const_cast<ast::ASTNode*>(node));
	}

	// Helper function to parse a declaration
	ast::ASTNodePtr parseDeclaration(const std::string &source) {
		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);
		return parser.parseProgram();
	}

	// Helper to get the first declaration from a program
	const ast::ASTNode* getFirstDeclaration(const ast::ASTNodePtr& ast) {
		const auto* program = as<ast::ProgramNode>(ast.get());
		EXPECT_NE(program, nullptr);
		EXPECT_EQ(program->getDeclarations().size(), 1);
		return program->getDeclarations()[0].get();
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
		auto ast = parseDeclaration("int counter;");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "counter");

		const auto* type = as<ast::PrimitiveTypeNode>(varDecl->getType().get());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		EXPECT_FALSE(varDecl->hasInitializer());
	}

	// Test variable declaration with initializer
	{
		auto ast = parseDeclaration("double pi = 3.14159;");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "pi");

		const auto* type = as<ast::PrimitiveTypeNode>(varDecl->getType().get());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::DOUBLE);

		EXPECT_TRUE(varDecl->hasInitializer());
		const auto* initializer = as<ast::LiteralNode>(varDecl->getInitializer().get());
		ASSERT_NE(initializer, nullptr);
		EXPECT_EQ(initializer->getKind(), ast::LiteralNode::Kind::DOUBLE);
	}

	// Test pointer variable declaration
	{
		auto ast = parseDeclaration("char* str;");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "str");

		const auto* type = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(type, nullptr);

		const auto* baseType = as<ast::PrimitiveTypeNode>(type->getBaseType().get());
		ASSERT_NE(baseType, nullptr);
		EXPECT_EQ(baseType->getKind(), ast::PrimitiveTypeNode::Kind::CHAR);
	}

	// Test array variable declaration
	{
		auto ast = parseDeclaration("int numbers[10];");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "numbers");

		const auto* type = as<ast::PrimitiveTypeNode>(varDecl->getType().get());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_NE(varDecl->getArraySize(), nullptr);
		const auto* arraySize = as<ast::LiteralNode>(varDecl->getArraySize().get());
		ASSERT_NE(arraySize, nullptr);
		EXPECT_EQ(arraySize->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(arraySize->getValue(), "10");
	}

	// Test void pointer declaration
	{
		auto ast = parseDeclaration("void* ptr;");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "ptr");

		const auto* type = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(type, nullptr);

		const auto* baseType = as<ast::PrimitiveTypeNode>(type->getBaseType().get());
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
		auto ast = parseDeclaration("int getNumber();");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcDecl = as<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getIdentifier(), "getNumber");

		const auto* returnType = as<ast::PrimitiveTypeNode>(funcDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		EXPECT_EQ(funcDecl->getParameters().size(), 0);
		EXPECT_FALSE(funcDecl->isDefinition());
	}

	// Test function declaration with parameters
	{
		auto ast = parseDeclaration("int add(int a, int b);");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcDecl = as<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getIdentifier(), "add");

		const auto* returnType = as<ast::PrimitiveTypeNode>(funcDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_EQ(funcDecl->getParameters().size(), 2);

		// Check first parameter
		const auto* param1 = as<ast::ParameterNode>(funcDecl->getParameters()[0].get());
		ASSERT_NE(param1, nullptr);
		EXPECT_EQ(param1->getIdentifier(), "a");

		const auto* param1Type = as<ast::PrimitiveTypeNode>(param1->getType().get());
		ASSERT_NE(param1Type, nullptr);
		EXPECT_EQ(param1Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		// Check second parameter
		const auto* param2 = as<ast::ParameterNode>(funcDecl->getParameters()[1].get());
		ASSERT_NE(param2, nullptr);
		EXPECT_EQ(param2->getIdentifier(), "b");
	}

	// Test void function declaration
	{
		auto ast = parseDeclaration("void logMessage(char* msg);");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcDecl = as<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getIdentifier(), "logMessage");

		const auto* returnType = as<ast::PrimitiveTypeNode>(funcDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		ASSERT_EQ(funcDecl->getParameters().size(), 1);

		// Check parameter
		const auto* param = as<ast::ParameterNode>(funcDecl->getParameters()[0].get());
		ASSERT_NE(param, nullptr);
		EXPECT_EQ(param->getIdentifier(), "msg");

		const auto* paramType = as<ast::PointerTypeNode>(param->getType().get());
		ASSERT_NE(paramType, nullptr);

		const auto* baseType = as<ast::PrimitiveTypeNode>(paramType->getBaseType().get());
		ASSERT_NE(baseType, nullptr);
		EXPECT_EQ(baseType->getKind(), ast::PrimitiveTypeNode::Kind::CHAR);
	}

	// Test function declaration with complex return type
	{
		auto ast = parseDeclaration("int* createArray(int size);");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcDecl = as<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getIdentifier(), "createArray");

		const auto* returnType = as<ast::PointerTypeNode>(funcDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);

		const auto* baseType = as<ast::PrimitiveTypeNode>(returnType->getBaseType().get());
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
		auto ast = parseDeclaration("int identity(int x) { return x; }");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcDecl = as<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getIdentifier(), "identity");

		const auto* returnType = as<ast::PrimitiveTypeNode>(funcDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_EQ(funcDecl->getParameters().size(), 1);
		EXPECT_TRUE(funcDecl->isDefinition());

		const auto* body = as<ast::BlockStatementNode>(funcDecl->getBody().get());
		ASSERT_NE(body, nullptr);
		ASSERT_EQ(body->getStatements().size(), 1);

		const auto* returnStmt = as<ast::ReturnStatementNode>(body->getStatements()[0].get());
		ASSERT_NE(returnStmt, nullptr);
		EXPECT_TRUE(returnStmt->hasValue());
	}

	// Test function definition with multiple statements
	{
		auto ast = parseDeclaration(R"(
            int max(int a, int b) {
                if (a > b) {
                    return a;
                } else {
                    return b;
                }
            }
        )");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcDecl = as<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getIdentifier(), "max");

		ASSERT_EQ(funcDecl->getParameters().size(), 2);
		EXPECT_TRUE(funcDecl->isDefinition());

		const auto* body = as<ast::BlockStatementNode>(funcDecl->getBody().get());
		ASSERT_NE(body, nullptr);
		ASSERT_EQ(body->getStatements().size(), 1);

		const auto* ifStmt = as<ast::IfStatementNode>(body->getStatements()[0].get());
		ASSERT_NE(ifStmt, nullptr);
		EXPECT_TRUE(ifStmt->hasElseBranch());
	}

	// Test void function definition
	{
		auto ast = parseDeclaration(R"(
            void swap(int* a, int* b) {
                int temp = *a;
                *a = *b;
                *b = temp;
            }
        )");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcDecl = as<ast::FunctionDeclarationNode>(decl);
		ASSERT_NE(funcDecl, nullptr);
		EXPECT_EQ(funcDecl->getIdentifier(), "swap");

		const auto* returnType = as<ast::PrimitiveTypeNode>(funcDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		ASSERT_EQ(funcDecl->getParameters().size(), 2);
		EXPECT_TRUE(funcDecl->isDefinition());

		const auto* body = as<ast::BlockStatementNode>(funcDecl->getBody().get());
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
		auto ast = parseDeclaration("struct Point { int x; int y; };");
		const auto* decl = getFirstDeclaration(ast);
		const auto* structDecl = as<ast::StructDeclarationNode>(decl);
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getIdentifier(), "Point");
		EXPECT_TRUE(structDecl->isDefinition());
		ASSERT_EQ(structDecl->getFields().size(), 2);

		// Check first field
		const auto* field1 = as<ast::VariableNode>(structDecl->getFields()[0].get());
		ASSERT_NE(field1, nullptr);
		EXPECT_EQ(field1->getIdentifier(), "x");

		const auto* field1Type = as<ast::PrimitiveTypeNode>(field1->getType().get());
		ASSERT_NE(field1Type, nullptr);
		EXPECT_EQ(field1Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		// Check second field
		const auto* field2 = as<ast::VariableNode>(structDecl->getFields()[1].get());
		ASSERT_NE(field2, nullptr);
		EXPECT_EQ(field2->getIdentifier(), "y");
	}

	// Test struct forward declaration
	{
		auto ast = parseDeclaration("struct Node;");
		const auto* decl = getFirstDeclaration(ast);
		const auto* structDecl = as<ast::StructDeclarationNode>(decl);
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getIdentifier(), "Node");
		EXPECT_FALSE(structDecl->isDefinition());
		EXPECT_EQ(structDecl->getFields().size(), 0);
	}

	// Test struct with complex fields
	{
		auto ast = parseDeclaration("struct LinkedList { int value; LinkedList* next; };");
		const auto* decl = getFirstDeclaration(ast);
		const auto* structDecl = as<ast::StructDeclarationNode>(decl);
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getIdentifier(), "LinkedList");
		EXPECT_TRUE(structDecl->isDefinition());
		ASSERT_EQ(structDecl->getFields().size(), 2);

		// Check first field
		const auto* field1 = as<ast::VariableNode>(structDecl->getFields()[0].get());
		ASSERT_NE(field1, nullptr);
		EXPECT_EQ(field1->getIdentifier(), "value");

		// Check second field (pointer)
		const auto* field2 = as<ast::VariableNode>(structDecl->getFields()[1].get());
		ASSERT_NE(field2, nullptr);
		EXPECT_EQ(field2->getIdentifier(), "next");

		const auto* field2Type = as<ast::PointerTypeNode>(field2->getType().get());
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
		auto ast = parseDeclaration("typedef void (*Callback)();");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcPtrDecl = as<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getIdentifier(), "Callback");

		const auto* returnType = as<ast::PrimitiveTypeNode>(funcPtrDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		EXPECT_EQ(funcPtrDecl->getParameterTypes().size(), 0);
	}

	// Test function pointer with parameters
	{
		auto ast = parseDeclaration("typedef int (*Comparator)(int, int);");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcPtrDecl = as<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getIdentifier(), "Comparator");

		const auto* returnType = as<ast::PrimitiveTypeNode>(funcPtrDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);
		EXPECT_EQ(returnType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_EQ(funcPtrDecl->getParameterTypes().size(), 2);

		// Check parameter types
		const auto* param1Type = as<ast::PrimitiveTypeNode>(funcPtrDecl->getParameterTypes()[0].get());
		ASSERT_NE(param1Type, nullptr);
		EXPECT_EQ(param1Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		const auto* param2Type = as<ast::PrimitiveTypeNode>(funcPtrDecl->getParameterTypes()[1].get());
		ASSERT_NE(param2Type, nullptr);
		EXPECT_EQ(param2Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test function pointer with complex types
	{
		auto ast = parseDeclaration("typedef void* (*Allocator)(int, void*);");
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcPtrDecl = as<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getIdentifier(), "Allocator");

		// Check return type (void*)
		const auto* returnType = as<ast::PointerTypeNode>(funcPtrDecl->getReturnType().get());
		ASSERT_NE(returnType, nullptr);

		const auto* returnBaseType = as<ast::PrimitiveTypeNode>(returnType->getBaseType().get());
		ASSERT_NE(returnBaseType, nullptr);
		EXPECT_EQ(returnBaseType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		ASSERT_EQ(funcPtrDecl->getParameterTypes().size(), 2);

		// Check first parameter type (int)
		const auto* param1Type = as<ast::PrimitiveTypeNode>(funcPtrDecl->getParameterTypes()[0].get());
		ASSERT_NE(param1Type, nullptr);
		EXPECT_EQ(param1Type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		// Check second parameter type (void*)
		const auto* param2Type = as<ast::PointerTypeNode>(funcPtrDecl->getParameterTypes()[1].get());
		ASSERT_NE(param2Type, nullptr);

		const auto* param2BaseType = as<ast::PrimitiveTypeNode>(param2Type->getBaseType().get());
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
	auto ast = parser.parseProgram();
	const auto* program = as<ast::ProgramNode>(ast.get());
	ASSERT_NE(program, nullptr);
	ASSERT_EQ(program->getDeclarations().size(), 4);

	// Check struct declaration
	const auto* structDecl = as<ast::StructDeclarationNode>(program->getDeclarations()[0].get());
	ASSERT_NE(structDecl, nullptr);
	EXPECT_EQ(structDecl->getIdentifier(), "Point");

	// Check function pointer declaration
	const auto* funcPtrDecl = as<ast::FunctionPointerDeclarationNode>(program->getDeclarations()[1].get());
	ASSERT_NE(funcPtrDecl, nullptr);
	EXPECT_EQ(funcPtrDecl->getIdentifier(), "Comparator");

	// Check function declaration
	const auto* funcDecl = as<ast::FunctionDeclarationNode>(program->getDeclarations()[2].get());
	ASSERT_NE(funcDecl, nullptr);
	EXPECT_EQ(funcDecl->getIdentifier(), "compare");
	EXPECT_FALSE(funcDecl->isDefinition());

	// Check function definition
	const auto* mainDecl = as<ast::FunctionDeclarationNode>(program->getDeclarations()[3].get());
	ASSERT_NE(mainDecl, nullptr);
	EXPECT_EQ(mainDecl->getIdentifier(), "main");
	EXPECT_TRUE(mainDecl->isDefinition());
}