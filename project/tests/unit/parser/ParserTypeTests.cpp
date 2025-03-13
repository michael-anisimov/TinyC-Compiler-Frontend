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
	// Helper function to parse a variable declaration with a type
	ast::VariableDeclarationNode *parseTypeDeclaration(const std::string &source) {
		std::string fullSource = source + ";"; // Add semicolon
		lexer::Lexer lexer(fullSource);
		parser::Parser parser(lexer);
		auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
		EXPECT_NE(program, nullptr);
		EXPECT_EQ(program->getDeclarations().size(), 1);
		return dynamic_cast<ast::VariableDeclarationNode *>(program->getDeclarations()[0].get());
	}

	// Helper function to cast to a primitive type
	ast::PrimitiveTypeNode *castToPrimitiveType(ast::ASTNode *typeNode) {
		return dynamic_cast<ast::PrimitiveTypeNode *>(typeNode);
	}

	// Helper function to cast to a pointer type
	ast::PointerTypeNode *castToPointerType(ast::ASTNode *typeNode) {
		return dynamic_cast<ast::PointerTypeNode *>(typeNode);
	}

	// Helper function to check a primitive type
	void checkPrimitiveType(ast::ASTNode *typeNode, ast::PrimitiveTypeNode::Kind expectedKind) {
		auto primitiveType = castToPrimitiveType(typeNode);
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
		auto varDecl = parseTypeDeclaration("int x");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "x");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test double
	{
		auto varDecl = parseTypeDeclaration("double y");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "y");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::DOUBLE);
	}

	// Test char
	{
		auto varDecl = parseTypeDeclaration("char z");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "z");
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
		auto varDecl = parseTypeDeclaration("int* ptr");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "ptr");

		auto pointerType = castToPointerType(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		auto baseType = pointerType->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test pointer to double
	{
		auto varDecl = parseTypeDeclaration("double* dptr");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "dptr");

		auto pointerType = castToPointerType(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		auto baseType = pointerType->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::DOUBLE);
	}

	// Test pointer to char
	{
		auto varDecl = parseTypeDeclaration("char* str");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "str");

		auto pointerType = castToPointerType(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		auto baseType = pointerType->getBaseType().get();
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
		auto varDecl = parseTypeDeclaration("int** ppint");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "ppint");

		auto pointer1 = castToPointerType(varDecl->getType().get());
		ASSERT_NE(pointer1, nullptr);

		auto pointer2 = castToPointerType(pointer1->getBaseType().get());
		ASSERT_NE(pointer2, nullptr);

		auto baseType = pointer2->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test pointer to pointer to pointer
	{
		auto varDecl = parseTypeDeclaration("double*** pppd");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "pppd");

		auto pointer1 = castToPointerType(varDecl->getType().get());
		ASSERT_NE(pointer1, nullptr);

		auto pointer2 = castToPointerType(pointer1->getBaseType().get());
		ASSERT_NE(pointer2, nullptr);

		auto pointer3 = castToPointerType(pointer2->getBaseType().get());
		ASSERT_NE(pointer3, nullptr);

		auto baseType = pointer3->getBaseType().get();
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
		auto varDecl = parseTypeDeclaration("void* vptr");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "vptr");

		auto pointerType = castToPointerType(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		auto baseType = pointerType->getBaseType().get();
		checkPrimitiveType(baseType, ast::PrimitiveTypeNode::Kind::VOID);
	}

	// Test pointer to void pointer
	{
		auto varDecl = parseTypeDeclaration("void** pvptr");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "pvptr");

		auto pointer1 = castToPointerType(varDecl->getType().get());
		ASSERT_NE(pointer1, nullptr);

		auto pointer2 = castToPointerType(pointer1->getBaseType().get());
		ASSERT_NE(pointer2, nullptr);

		auto baseType = pointer2->getBaseType().get();
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
	// Helper function to parse array declaration
	auto parseArrayDecl = [](const std::string &source) {
		std::string fullSource = source + ";";
		lexer::Lexer lexer(fullSource);
		parser::Parser parser(lexer);
		auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
		EXPECT_NE(program, nullptr);
		EXPECT_EQ(program->getDeclarations().size(), 1);
		return dynamic_cast<ast::VariableDeclarationNode *>(program->getDeclarations()[0].get());
	};

	// Test array of int
	{
		auto varDecl = parseArrayDecl("int numbers[10]");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "numbers");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::INT);
		ASSERT_NE(varDecl->getArraySize(), nullptr);
		// Check that array size is a literal with value 10
		auto sizeNode = dynamic_cast<ast::LiteralNode *>(varDecl->getArraySize().get());
		ASSERT_NE(sizeNode, nullptr);
		EXPECT_EQ(sizeNode->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(sizeNode->getValue(), "10");
	}

	// Test array of char
	{
		auto varDecl = parseArrayDecl("char str[100]");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "str");
		checkPrimitiveType(varDecl->getType().get(), ast::PrimitiveTypeNode::Kind::CHAR);
		ASSERT_NE(varDecl->getArraySize(), nullptr);
	}

	// Test array of pointers
	{
		auto varDecl = parseArrayDecl("int* ptrs[5]");
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "ptrs");

		auto pointerType = castToPointerType(varDecl->getType().get());
		ASSERT_NE(pointerType, nullptr);

		auto baseType = pointerType->getBaseType().get();
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
	// Helper function to parse struct declaration
	auto parseStructDecl = [](const std::string &source) {
		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);
		auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
		EXPECT_NE(program, nullptr);
		EXPECT_EQ(program->getDeclarations().size(), 1);
		return dynamic_cast<ast::StructDeclarationNode *>(program->getDeclarations()[0].get());
	};

	// Test struct declaration
	{
		auto structDecl = parseStructDecl("struct Point { int x; int y; };");
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getName(), "Point");
		EXPECT_TRUE(structDecl->isDefinition());
		ASSERT_EQ(structDecl->getFields().size(), 2);

		// Check first field (x)
		auto field1 = dynamic_cast<ast::VariableDeclarationNode *>(structDecl->getFields()[0].get());
		ASSERT_NE(field1, nullptr);
		EXPECT_EQ(field1->getName(), "x");
		checkPrimitiveType(field1->getType().get(), ast::PrimitiveTypeNode::Kind::INT);

		// Check second field (y)
		auto field2 = dynamic_cast<ast::VariableDeclarationNode *>(structDecl->getFields()[1].get());
		ASSERT_NE(field2, nullptr);
		EXPECT_EQ(field2->getName(), "y");
		checkPrimitiveType(field2->getType().get(), ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test struct forward declaration
	{
		auto structDecl = parseStructDecl("struct Node;");
		ASSERT_NE(structDecl, nullptr);
		EXPECT_EQ(structDecl->getName(), "Node");
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
	// Helper function to parse function pointer declaration
	auto parseFuncPtrDecl = [](const std::string &source) {
		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);
		auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
		EXPECT_NE(program, nullptr);
		EXPECT_EQ(program->getDeclarations().size(), 1);
		return dynamic_cast<ast::FunctionPointerDeclarationNode *>(program->getDeclarations()[0].get());
	};

	// Test function pointer with no parameters
	{
		auto funcPtrDecl = parseFuncPtrDecl("typedef void (*Callback)();");
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getName(), "Callback");
		checkPrimitiveType(funcPtrDecl->getReturnType().get(), ast::PrimitiveTypeNode::Kind::VOID);
		EXPECT_EQ(funcPtrDecl->getParameterTypes().size(), 0);
	}

	// Test function pointer with parameters
	{
		auto funcPtrDecl = parseFuncPtrDecl("typedef int (*Comparator)(int, int);");
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getName(), "Comparator");
		checkPrimitiveType(funcPtrDecl->getReturnType().get(), ast::PrimitiveTypeNode::Kind::INT);
		ASSERT_EQ(funcPtrDecl->getParameterTypes().size(), 2);

		// Check parameter types
		checkPrimitiveType(funcPtrDecl->getParameterTypes()[0].get(), ast::PrimitiveTypeNode::Kind::INT);
		checkPrimitiveType(funcPtrDecl->getParameterTypes()[1].get(), ast::PrimitiveTypeNode::Kind::INT);
	}

	// Test function pointer with complex types
	{
		auto funcPtrDecl = parseFuncPtrDecl("typedef void* (*Allocator)(int, void*);");
		ASSERT_NE(funcPtrDecl, nullptr);
		EXPECT_EQ(funcPtrDecl->getName(), "Allocator");

		// Check return type (void*)
		auto returnPointer = castToPointerType(funcPtrDecl->getReturnType().get());
		ASSERT_NE(returnPointer, nullptr);

		auto returnBaseType = returnPointer->getBaseType().get();
		checkPrimitiveType(returnBaseType, ast::PrimitiveTypeNode::Kind::VOID);

		// Check parameter types
		ASSERT_EQ(funcPtrDecl->getParameterTypes().size(), 2);
		checkPrimitiveType(funcPtrDecl->getParameterTypes()[0].get(), ast::PrimitiveTypeNode::Kind::INT);

		auto paramPointer = castToPointerType(funcPtrDecl->getParameterTypes()[1].get());
		ASSERT_NE(paramPointer, nullptr);

		auto paramBaseType = paramPointer->getBaseType().get();
		checkPrimitiveType(paramBaseType, ast::PrimitiveTypeNode::Kind::VOID);
	}
}