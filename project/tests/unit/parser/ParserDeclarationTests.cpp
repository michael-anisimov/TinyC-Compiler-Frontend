#include <iostream>
#include "tinyc/parser/Parser.h"
#include "tinyc/lexer/Lexer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <memory>

using namespace tinyc;

/**
 * @file ParserDeclarationTests.cpp
 * @brief Tests for TinyC declaration parsing
 */

namespace {
	// Helper function to parse a declaration
//	ast::ASTNodePtr parseDeclaration(const std::string &source) {
//		lexer::Lexer lexer(source);
//		parser::Parser parser(lexer);
//		auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
//		EXPECT_NE(program, nullptr);
//		EXPECT_EQ(program->getDeclarations().size(), 1);
//		return program->getDeclarations()[0];
//	}

	// Function to print token-by-token debug info
	void debugSource(const std::string &source) {
		std::cout << "Debug source: '" << source << "'" << std::endl;
		lexer::Lexer lexer(source);

		try {
			lexer::TokenPtr token;
			while ((token = lexer.nextToken())->getType() != lexer::TokenType::END_OF_FILE) {
				std::cout << "Token: " << token->getLexeme()
						  << " (Type: " << static_cast<int>(token->getType()) << ")"
						  << " at " << token->getLocation() << std::endl;
			}
		} catch (const std::exception &e) {
			std::cout << "Exception during lexing: " << e.what() << std::endl;
		}
	}
}

/**
 * @test StructDeclarations
 * @brief Test struct declarations
 */
TEST(ParserDeclarationTest, StructDeclarations) {
	// Helper function to parse struct declaration with debugging
	auto parseStructDecl = [](const std::string &source) {
		std::cout << "Parsing struct source: '" << source << "'" << std::endl;

		// Debug token stream
		debugSource(source);

		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);

		try {
			auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
			if (!program) {
				std::cout << "Failed to parse program" << std::endl;
				return static_cast<ast::StructDeclarationNode *>(nullptr);
			}

			std::cout << "Program declarations: " << program->getDeclarations().size() << std::endl;

			if (program->getDeclarations().empty()) {
				std::cout << "No declarations found in program" << std::endl;
				return static_cast<ast::StructDeclarationNode *>(nullptr);
			}

			return dynamic_cast<ast::StructDeclarationNode *>(program->getDeclarations()[0].get());
		} catch (const std::exception &e) {
			std::cout << "Exception during parsing: " << e.what() << std::endl;
			return static_cast<ast::StructDeclarationNode *>(nullptr);
		}
	};

	// Test struct forward declaration first (simpler case)
	{
		std::string source = "struct Node;";
		auto structDecl = parseStructDecl(source);
		ASSERT_NE(structDecl, nullptr) << "Failed to parse: " << source;

		EXPECT_EQ(structDecl->getName(), "Node");
		EXPECT_FALSE(structDecl->isDefinition());
		EXPECT_EQ(structDecl->getFields().size(), 0);
	}

	// Test struct with one field
	{
		std::string source = "struct Point { int x; };";
		auto structDecl = parseStructDecl(source);
		ASSERT_NE(structDecl, nullptr) << "Failed to parse: " << source;

		EXPECT_EQ(structDecl->getName(), "Point");
		EXPECT_TRUE(structDecl->isDefinition());
		ASSERT_EQ(structDecl->getFields().size(), 1);

		// Check first field
		auto field = dynamic_cast<ast::VariableDeclarationNode *>(structDecl->getFields()[0].get());
		ASSERT_NE(field, nullptr);
		EXPECT_EQ(field->getName(), "x");
	}

	// Test struct with multiple fields (this is where it's failing)
	{
		std::string source = "struct Point { int x; int y; };";
		auto structDecl = parseStructDecl(source);
		ASSERT_NE(structDecl, nullptr) << "Failed to parse: " << source;

		EXPECT_EQ(structDecl->getName(), "Point");
		EXPECT_TRUE(structDecl->isDefinition());
		ASSERT_EQ(structDecl->getFields().size(), 2);

		// Only test further if we got enough fields
		if (structDecl->getFields().size() >= 2) {
			// Check first field (x)
			auto field1 = dynamic_cast<ast::VariableDeclarationNode *>(structDecl->getFields()[0].get());
			ASSERT_NE(field1, nullptr);
			EXPECT_EQ(field1->getName(), "x");

			// Check second field (y)
			auto field2 = dynamic_cast<ast::VariableDeclarationNode *>(structDecl->getFields()[1].get());
			ASSERT_NE(field2, nullptr);
			EXPECT_EQ(field2->getName(), "y");
		}
	}
}