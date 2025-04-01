#include "tinyc/parser/Parser.h"
#include "tinyc/lexer/Lexer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <memory>

using namespace tinyc;

/**
 * @file ParserTypeTests.cpp
 * @brief Tests for TinyC type parsing
 *
 * This file contains tests for type declarations in TinyC, organized from simple
 * to complex. These tests verify the parser correctly builds AST nodes for all
 * TinyC types including primitive types, pointers, and arrays.
 */

namespace {
	// Helper function to cast a node to a specific type
	template<typename T>
	T* as(const ast::ASTNode* node) {
		return dynamic_cast<T*>(const_cast<ast::ASTNode*>(node));
	}

	// Helper function to parse a variable declaration with a type
	ast::ASTNodePtr parseTypeDeclaration(const std::string &source) {
		std::string fullSource = source + ";"; // Add semicolon
		lexer::Lexer lexer(fullSource);
		parser::Parser parser(lexer);
		return parser.parseProgram();
	}

	// Helper function to get the first declaration from a program
	const ast::ASTNode* getFirstDeclaration(const ast::ASTNodePtr& ast) {
		const auto* program = as<ast::ProgramNode>(ast.get());
		EXPECT_NE(program, nullptr);
		EXPECT_EQ(program->getDeclarations().size(), 1);
		return program->getDeclarations()[0].get();
	}

	// Helper function to check a primitive type
	void checkPrimitiveType(const ast::ASTNode* typeNode, ast::PrimitiveTypeNode::Kind expectedKind) {
		const auto* primitiveType = as<ast::PrimitiveTypeNode>(typeNode);
		ASSERT_NE(primitiveType, nullptr);
		EXPECT_EQ(primitiveType->getKind(), expectedKind);
	}
}

/**
 * @test PrimitiveTypes
 * @brief Test primitive type declarations (int, double, char)
 *
 * Grammar rules tested:
 * - TYPE -> BASE_TYPE STAR_SEQ
 * - BASE_TYPE -> int | double | char
 */
TEST(ParserTypeTest, PrimitiveTypes) {
	// Test int
	{
		auto ast = parseTypeDeclaration("int x");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "x");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test double
	{
		auto ast = parseTypeDeclaration("double y");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "y");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::DOUBLE);
	}

	// Test char
	{
		auto ast = parseTypeDeclaration("char z");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "z");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::CHAR);
	}
}

/**
 * @test PointerTypes
 * @brief Test pointer type declarations
 *
 * Grammar rules tested:
 * - TYPE -> BASE_TYPE STAR_SEQ
 * - STAR_SEQ -> * STAR_SEQ | ε
 */
TEST(ParserTypeTest, PointerTypes) {
	// Test pointer to int
	{
		auto ast = parseTypeDeclaration("int* ptr");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "ptr");

		const auto* pointerType = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		const auto* baseType = pointerType->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test pointer to double
	{
		auto ast = parseTypeDeclaration("double* dptr");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "dptr");

		const auto* pointerType = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		const auto* baseType = pointerType->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::DOUBLE);
	}

	// Test pointer to char
	{
		auto ast = parseTypeDeclaration("char* str");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "str");

		const auto* pointerType = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		const auto* baseType = pointerType->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::CHAR);
	}
}

/**
 * @test MultiLevelPointers
 * @brief Test multi-level pointer type declarations
 *
 * Grammar rules tested:
 * - TYPE -> BASE_TYPE STAR_SEQ
 * - STAR_SEQ -> * STAR_SEQ
 */
TEST(ParserTypeTest, MultiLevelPointers) {
	// Test pointer to pointer to int
	{
		auto ast = parseTypeDeclaration("int** ppint");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "ppint");

		const auto* pointer1 = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(pointer1, nullptr);

		const auto* pointer2 = as<ast::PointerTypeNode>(pointer1->getBaseType().get());
		ASSERT_NE(pointer2, nullptr);

		const auto* baseType = pointer2->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test pointer to pointer to pointer
	{
		auto ast = parseTypeDeclaration("double*** pppd");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "pppd");

		const auto* pointer1 = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(pointer1, nullptr);

		const auto* pointer2 = as<ast::PointerTypeNode>(pointer1->getBaseType().get());
		ASSERT_NE(pointer2, nullptr);

		const auto* pointer3 = as<ast::PointerTypeNode>(pointer2->getBaseType().get());
		ASSERT_NE(pointer3, nullptr);

		const auto* baseType = pointer3->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::DOUBLE);
	}
}

/**
 * @test VoidPointers
 * @brief Test void pointer type declarations
 *
 * Grammar rules tested:
 * - TYPE -> void STAR_PLUS
 * - STAR_PLUS -> * STAR_SEQ
 */
TEST(ParserTypeTest, VoidPointers) {
	// Test void pointer
	{
		auto ast = parseTypeDeclaration("void* vptr");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "vptr");

		const auto* pointerType = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		const auto* baseType = pointerType->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::VOID);
	}

	// Test pointer to void pointer
	{
		auto ast = parseTypeDeclaration("void** pvptr");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "pvptr");

		const auto* pointer1 = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(pointer1, nullptr);

		const auto* pointer2 = as<ast::PointerTypeNode>(pointer1->getBaseType().get());
		ASSERT_NE(pointer2, nullptr);

		const auto* baseType = pointer2->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::VOID);
	}
}

/**
 * @test ArrayTypes
 * @brief Test array type declarations
 *
 * Grammar rules tested:
 * - VAR_DECL -> TYPE identifier OPT_ARRAY_SIZE OPT_INIT
 * - OPT_ARRAY_SIZE -> [ E9 ] | ε
 */
TEST(ParserTypeTest, ArrayTypes) {
	// Test array of int
	{
		auto ast = parseTypeDeclaration("int numbers[10]");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "numbers");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::INT);
		ASSERT_NE(varDecl->getArraySize(), nullptr);
		// Check that array size is a literal with value 10
		const auto* sizeNode = as<ast::LiteralNode>(varDecl->getArraySize().get());
		ASSERT_NE(sizeNode, nullptr);
		EXPECT_EQ(sizeNode->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(sizeNode->getValue(), "10");
	}

	// Test array of char
	{
		auto ast = parseTypeDeclaration("char str[100]");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "str");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::CHAR);
		ASSERT_NE(varDecl->getArraySize(), nullptr);
	}

	// Test array of pointers
	{
		auto ast = parseTypeDeclaration("int* ptrs[5]");
		const auto* decl = getFirstDeclaration(ast);
		const auto* varDecl = as<ast::VariableNode>(decl);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "ptrs");

		const auto* pointerType = as<ast::PointerTypeNode>(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		const auto* baseType = pointerType->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::INT);

		ASSERT_NE(varDecl->getArraySize(), nullptr);
	}
}

/**
 * @test StructTypes
 * @brief Test parsing of struct types
 *
 * Grammar rules tested:
 * - STRUCT_DECL -> struct identifier [ '{' { TYPE identifier ';' } '}' ] ';'
 */
TEST(ParserTypeTest, StructTypes) {
	// Test struct declaration
	{
		lexer::Lexer lexer("struct Point { int x; int y; };");
		parser::Parser parser(lexer);
		auto ast = parser.parseProgram();
		const auto* decl = getFirstDeclaration(ast);
		const auto* structDecl = as<ast::StructDeclarationNode>(decl);
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getIdentifier(), "Point");
		EXPECT_TRUE(structDecl->isDefinition());
		ASSERT_EQ(structDecl->getFields().size(), 2);

		// Check first field (x)
		const auto* field1 = as<ast::VariableNode>(structDecl->getFields()[0].get());
		ASSERT_NE(field1, nullptr);
		EXPECT_EQ(field1->getIdentifier(), "x");
		checkPrimitiveType(field1->getType().get(), ast::PrimitiveTypeNode::Kind::INT);

		// Check second field (y)
		const auto* field2 = as<ast::VariableNode>(structDecl->getFields()[1].get());
		ASSERT_NE(field2, nullptr);
		EXPECT_EQ(field2->getIdentifier(), "y");
		checkPrimitiveType(field2->getType().get(), ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test struct forward declaration
	{
		lexer::Lexer lexer("struct Node;");
		parser::Parser parser(lexer);
		auto ast = parser.parseProgram();
		const auto* decl = getFirstDeclaration(ast);
		const auto* structDecl = as<ast::StructDeclarationNode>(decl);
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getIdentifier(), "Node");
		EXPECT_FALSE(structDecl->isDefinition());
		EXPECT_EQ(structDecl->getFields().size(), 0);
	}
}

/**
 * @test FunctionPointerTypes
 * @brief Test parsing of function pointer types
 *
 * Grammar rules tested:
 * - FUNPTR_DECL -> typedef TYPE_FUN_RET '(' '*' identifier ')' '(' [ TYPE { ',' TYPE } ] ')' ';'
 */
TEST(ParserTypeTest, FunctionPointerTypes) {
	// Test function pointer with no parameters
	{
		lexer::Lexer lexer("typedef void (*Callback)();");
		parser::Parser parser(lexer);
		auto ast = parser.parseProgram();
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcPtrDecl = as<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getIdentifier(), "Callback");
		checkPrimitiveType(funcPtrDecl->getReturnType().get(), ast::PrimitiveTypeNode::Kind::VOID);
		EXPECT_EQ(funcPtrDecl->getParameterTypes().size(), 0);
	}

	// Test function pointer with parameters
	{
		lexer::Lexer lexer("typedef int (*Comparator)(int, int);");
		parser::Parser parser(lexer);
		auto ast = parser.parseProgram();
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcPtrDecl = as<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getIdentifier(), "Comparator");
		checkPrimitiveType(funcPtrDecl->getReturnType().get(), ast::PrimitiveTypeNode::Kind::INT);
		ASSERT_EQ(funcPtrDecl->getParameterTypes().size(), 2);

		// Check parameter types
		checkPrimitiveType(funcPtrDecl->getParameterTypes()[0].get(), ast::PrimitiveTypeNode::Kind::INT);
		checkPrimitiveType(funcPtrDecl->getParameterTypes()[1].get(), ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test function pointer with complex types
	{
		lexer::Lexer lexer("typedef void* (*Allocator)(int, void*);");
		parser::Parser parser(lexer);
		auto ast = parser.parseProgram();
		const auto* decl = getFirstDeclaration(ast);
		const auto* funcPtrDecl = as<ast::FunctionPointerDeclarationNode>(decl);
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getIdentifier(), "Allocator");

		// Check return type (void*)
		const auto* returnPointer = as<ast::PointerTypeNode>(funcPtrDecl->getReturnType().get());
		ASSERT_NE(returnPointer, nullptr);

		const auto* returnBaseType = returnPointer->getBaseType().get();
		checkPrimitiveType(returnBaseType, ast::PrimitiveTypeNode::Kind::VOID);

		// Check parameter types
		ASSERT_EQ(funcPtrDecl->getParameterTypes().size(), 2);
		checkPrimitiveType(funcPtrDecl->getParameterTypes()[0].get(), ast::PrimitiveTypeNode::Kind::INT);

		const auto* paramPointer = as<ast::PointerTypeNode>(funcPtrDecl->getParameterTypes()[1].get());
		ASSERT_NE(paramPointer, nullptr);

		const auto* paramBaseType = paramPointer->getBaseType().get();
		checkPrimitiveType(paramBaseType, ast::PrimitiveTypeNode::Kind::VOID);
	}
}