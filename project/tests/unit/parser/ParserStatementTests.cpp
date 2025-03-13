#include "tinyc/parser/Parser.h"
#include "tinyc/lexer/Lexer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <memory>

using namespace tinyc;

/**
 * @file ParserStatementTests.cpp
 * @brief Tests for TinyC statement parsing
 *
 * This file contains tests for statement parsing in TinyC, organized from simple
 * to complex. These tests verify the parser correctly builds AST nodes for all
 * TinyC statement types.
 */

namespace {
	// Helper function to parse a statement within a function
	ast::ASTNodePtr parseStatement(const std::string &stmt) {
		std::string source = "void test() { " + stmt + " }";
		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);
		auto program = std::dynamic_pointer_cast<ast::ProgramNode>(parser.parseProgram());
		EXPECT_NE(program, nullptr);
		EXPECT_EQ(program->getDeclarations().size(), 1);

		auto func = std::dynamic_pointer_cast<ast::FunctionDeclarationNode>(program->getDeclarations()[0]);
		EXPECT_NE(func, nullptr);

		auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(func->getBody());
		EXPECT_NE(body, nullptr);
		EXPECT_EQ(body->getStatements().size(), 1);

		return body->getStatements()[0];
	}
}

/**
 * @test ExpressionStatement
 * @brief Test expression statements
 *
 * Grammar rules tested:
 * - EXPR_STMT -> EXPR_OR_VAR_DECL ;
 */
TEST(ParserStatementTest, ExpressionStatement) {
	// Test simple expression statement
	{
		auto stmt = parseStatement("x = 42;");
		auto exprStmt = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		auto expr = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(exprStmt->getExpression());
		ASSERT_NE(expr, nullptr);
		EXPECT_EQ(expr->getOperator(), ast::BinaryExpressionNode::Operator::ASSIGN);
	}

	// Test function call statement
	{
		auto stmt = parseStatement("foo(1, 2, 3);");
		auto exprStmt = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		auto expr = std::dynamic_pointer_cast<ast::CallExpressionNode>(exprStmt->getExpression());
		ASSERT_NE(expr, nullptr);
		EXPECT_EQ(expr->getArguments().size(), 3);
	}

	// Test post-increment statement
	{
		auto stmt = parseStatement("x++;");
		auto exprStmt = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		auto expr = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(exprStmt->getExpression());
		ASSERT_NE(expr, nullptr);
		EXPECT_EQ(expr->getOperator(), ast::UnaryExpressionNode::Operator::POST_INCREMENT);
	}
}

/**
 * @test VariableDeclaration
 * @brief Test variable declaration statements
 *
 * Grammar rules tested:
 * - EXPR_OR_VAR_DECL -> VAR_DECLS
 * - VAR_DECLS -> VAR_DECL VAR_DECLS_TAIL
 * - VAR_DECL -> TYPE identifier OPT_ARRAY_SIZE OPT_INIT
 */
TEST(ParserStatementTest, VariableDeclaration) {
	// Test simple variable declaration
	{
		auto stmt = parseStatement("int x;");
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(stmt);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "x");

		auto type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(varDecl->getType());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		EXPECT_FALSE(varDecl->hasInitializer());
	}

	// Test variable declaration with initializer
	{
		auto stmt = parseStatement("double pi = 3.14159;");
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(stmt);
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

	// Test array declaration
	{
		auto stmt = parseStatement("int numbers[10];");
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(stmt);
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

	// Test multiple variable declarations
	{
		auto stmt = parseStatement("int a = 1, b = 2, c = 3;");

		// In the current implementation, the first variable is returned
		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(stmt);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "a");

		auto type = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(varDecl->getType());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		EXPECT_TRUE(varDecl->hasInitializer());
		auto initializer = std::dynamic_pointer_cast<ast::LiteralNode>(varDecl->getInitializer());
		ASSERT_NE(initializer, nullptr);
		EXPECT_EQ(initializer->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(initializer->getValue(), "1");
	}
}

/**
 * @test BlockStatement
 * @brief Test block statements
 *
 * Grammar rules tested:
 * - BLOCK_STMT -> { STATEMENT_STAR }
 * - STATEMENT_STAR -> STATEMENT STATEMENT_STAR | ε
 */
TEST(ParserStatementTest, BlockStatement) {
	// Test empty block
	{
		auto stmt = parseStatement("{}");
		auto block = std::dynamic_pointer_cast<ast::BlockStatementNode>(stmt);
		ASSERT_NE(block, nullptr);
		EXPECT_EQ(block->getStatements().size(), 0);
	}

	// Test block with statements
	{
		auto stmt = parseStatement("{ int x = 10; x = x + 1; foo(x); }");
		auto block = std::dynamic_pointer_cast<ast::BlockStatementNode>(stmt);
		ASSERT_NE(block, nullptr);
		ASSERT_EQ(block->getStatements().size(), 3);

		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(block->getStatements()[0]);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "x");

		auto exprStmt1 = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(block->getStatements()[1]);
		ASSERT_NE(exprStmt1, nullptr);

		auto exprStmt2 = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(block->getStatements()[2]);
		ASSERT_NE(exprStmt2, nullptr);
	}

	// Test nested blocks
	{
		auto stmt = parseStatement("{ int x = 10; { int y = 20; } }");
		auto block = std::dynamic_pointer_cast<ast::BlockStatementNode>(stmt);
		ASSERT_NE(block, nullptr);
		ASSERT_EQ(block->getStatements().size(), 2);

		auto innerBlock = std::dynamic_pointer_cast<ast::BlockStatementNode>(block->getStatements()[1]);
		ASSERT_NE(innerBlock, nullptr);
		ASSERT_EQ(innerBlock->getStatements().size(), 1);

		auto varDecl = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(innerBlock->getStatements()[0]);
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getName(), "y");
	}
}

/**
 * @test IfStatement
 * @brief Test if statements
 *
 * Grammar rules tested:
 * - IF_STMT -> if ( EXPR ) STATEMENT
 * - IF_STMT -> if ( EXPR ) STATEMENT else STATEMENT
 */
TEST(ParserStatementTest, IfStatement) {
	// Test if without else
	{
		auto stmt = parseStatement("if (x > 0) x = x - 1;");
		auto ifStmt = std::dynamic_pointer_cast<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		auto condition = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(ifStmt->getCondition());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		auto thenBranch = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(ifStmt->getThenBranch());
		ASSERT_NE(thenBranch, nullptr);

		EXPECT_FALSE(ifStmt->hasElseBranch());
		EXPECT_EQ(ifStmt->getElseBranch(), nullptr);
	}

	// Test if with else
	{
		auto stmt = parseStatement("if (x > 0) x = x - 1; else x = 0;");
		auto ifStmt = std::dynamic_pointer_cast<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		auto condition = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(ifStmt->getCondition());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		auto thenBranch = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(ifStmt->getThenBranch());
		ASSERT_NE(thenBranch, nullptr);

		EXPECT_TRUE(ifStmt->hasElseBranch());
		auto elseBranch = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(ifStmt->getElseBranch());
		ASSERT_NE(elseBranch, nullptr);
	}

	// Test if with block
	{
		auto stmt = parseStatement("if (x > 0) { x = x - 1; y = y + 1; }");
		auto ifStmt = std::dynamic_pointer_cast<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		auto thenBranch = std::dynamic_pointer_cast<ast::BlockStatementNode>(ifStmt->getThenBranch());
		ASSERT_NE(thenBranch, nullptr);
		EXPECT_EQ(thenBranch->getStatements().size(), 2);
	}

	// Test if-else with blocks
	{
		auto stmt = parseStatement("if (x > 0) { x = x - 1; } else { x = 0; }");
		auto ifStmt = std::dynamic_pointer_cast<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		auto thenBranch = std::dynamic_pointer_cast<ast::BlockStatementNode>(ifStmt->getThenBranch());
		ASSERT_NE(thenBranch, nullptr);
		EXPECT_EQ(thenBranch->getStatements().size(), 1);

		EXPECT_TRUE(ifStmt->hasElseBranch());
		auto elseBranch = std::dynamic_pointer_cast<ast::BlockStatementNode>(ifStmt->getElseBranch());
		ASSERT_NE(elseBranch, nullptr);
		EXPECT_EQ(elseBranch->getStatements().size(), 1);
	}

	// Test nested if-else
	{
		auto stmt = parseStatement("if (x > 0) if (y > 0) z = 1; else z = 2;");
		auto outerIf = std::dynamic_pointer_cast<ast::IfStatementNode>(stmt);
		ASSERT_NE(outerIf, nullptr);

		auto innerIf = std::dynamic_pointer_cast<ast::IfStatementNode>(outerIf->getThenBranch());
		ASSERT_NE(innerIf, nullptr);
		EXPECT_TRUE(innerIf->hasElseBranch());
	}
}

/**
 * @test WhileStatement
 * @brief Test while statements
 *
 * Grammar rules tested:
 * - WHILE_STMT -> while ( EXPR ) STATEMENT
 */
TEST(ParserStatementTest, WhileStatement) {
	// Test simple while loop
	{
		auto stmt = parseStatement("while (x > 0) x = x - 1;");
		auto whileStmt = std::dynamic_pointer_cast<ast::WhileStatementNode>(stmt);
		ASSERT_NE(whileStmt, nullptr);

		auto condition = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(whileStmt->getCondition());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		auto body = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(whileStmt->getBody());
		ASSERT_NE(body, nullptr);
	}

	// Test while loop with block
	{
		auto stmt = parseStatement("while (x > 0) { x = x - 1; sum = sum + x; }");
		auto whileStmt = std::dynamic_pointer_cast<ast::WhileStatementNode>(stmt);
		ASSERT_NE(whileStmt, nullptr);

		auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(whileStmt->getBody());
		ASSERT_NE(body, nullptr);
		EXPECT_EQ(body->getStatements().size(), 2);
	}
}

/**
 * @test DoWhileStatement
 * @brief Test do-while statements
 *
 * Grammar rules tested:
 * - DO_WHILE_STMT -> do STATEMENT while ( EXPR ) ;
 */
TEST(ParserStatementTest, DoWhileStatement) {
	// Test simple do-while loop
	{
		auto stmt = parseStatement("do x = x - 1; while (x > 0);");
		auto doWhileStmt = std::dynamic_pointer_cast<ast::DoWhileStatementNode>(stmt);
		ASSERT_NE(doWhileStmt, nullptr);

		auto condition = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(doWhileStmt->getCondition());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		auto body = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(doWhileStmt->getBody());
		ASSERT_NE(body, nullptr);
	}

	// Test do-while loop with block
	{
		auto stmt = parseStatement("do { x = x - 1; sum = sum + x; } while (x > 0);");
		auto doWhileStmt = std::dynamic_pointer_cast<ast::DoWhileStatementNode>(stmt);
		ASSERT_NE(doWhileStmt, nullptr);

		auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(doWhileStmt->getBody());
		ASSERT_NE(body, nullptr);
		EXPECT_EQ(body->getStatements().size(), 2);
	}
}

/**
 * @test ForStatement
 * @brief Test for statements
 *
 * Grammar rules tested:
 * - FOR_STMT -> for ( OPT_EXPR_OR_VAR_DECL ; OPT_EXPR ; OPT_EXPR ) STATEMENT
 */
TEST(ParserStatementTest, ForStatement) {
	// Test for loop with all parts
	{
		auto stmt = parseStatement("for (int i = 0; i < 10; i = i + 1) sum = sum + i;");
		auto forStmt = std::dynamic_pointer_cast<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		auto init = std::dynamic_pointer_cast<ast::VariableDeclarationNode>(forStmt->getInitialization());
		ASSERT_NE(init, nullptr);
		EXPECT_EQ(init->getName(), "i");

		auto condition = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(forStmt->getCondition());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::LESS);

		auto update = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(forStmt->getUpdate());
		ASSERT_NE(update, nullptr);
		EXPECT_EQ(update->getOperator(), ast::BinaryExpressionNode::Operator::ASSIGN);

		auto body = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(forStmt->getBody());
		ASSERT_NE(body, nullptr);
	}

	// Test for loop with missing parts
	{
		auto stmt = parseStatement("for (;;) sum = sum + 1;");
		auto forStmt = std::dynamic_pointer_cast<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		EXPECT_EQ(forStmt->getInitialization(), nullptr);
		EXPECT_EQ(forStmt->getCondition(), nullptr);
		EXPECT_EQ(forStmt->getUpdate(), nullptr);

		auto body = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(forStmt->getBody());
		ASSERT_NE(body, nullptr);
	}

	// Test for loop with expression initialization
	{
		auto stmt = parseStatement("for (i = 0; i < 10; i = i + 1) sum = sum + i;");
		auto forStmt = std::dynamic_pointer_cast<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		auto init = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(forStmt->getInitialization());
		ASSERT_NE(init, nullptr);
		EXPECT_EQ(init->getOperator(), ast::BinaryExpressionNode::Operator::ASSIGN);
	}

	// Test for loop with block
	{
		auto stmt = parseStatement("for (int i = 0; i < 10; i++) { sum = sum + i; }");
		auto forStmt = std::dynamic_pointer_cast<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(forStmt->getBody());
		ASSERT_NE(body, nullptr);
		EXPECT_EQ(body->getStatements().size(), 1);
	}
}

/**
 * @test SwitchStatement
 * @brief Test switch statements
 *
 * Grammar rules tested:
 * - SWITCH_STMT -> switch ( EXPR ) { CASE_WITH_DEFAULT_STMT_STAR }
 * - CASE_STMT -> case integer_literal : CASE_BODY
 * - DEFAULT_CASE -> default : CASE_BODY
 */
TEST(ParserStatementTest, SwitchStatement) {
	// Test switch with cases
	{
		auto stmt = parseStatement("switch (x) { case 1: y = 10; break; case 2: y = 20; break; }");
		auto switchStmt = std::dynamic_pointer_cast<ast::SwitchStatementNode>(stmt);
		ASSERT_NE(switchStmt, nullptr);

		auto expr = std::dynamic_pointer_cast<ast::IdentifierNode>(switchStmt->getExpression());
		ASSERT_NE(expr, nullptr);
		EXPECT_EQ(expr->getName(), "x");

		ASSERT_EQ(switchStmt->getCases().size(), 2);

		// Check first case
		auto case1 = switchStmt->getCases()[0];
		EXPECT_FALSE(case1.isDefault);
		EXPECT_EQ(case1.value, 1);
		ASSERT_EQ(case1.body.size(), 2);

		// Check second case
		auto case2 = switchStmt->getCases()[1];
		EXPECT_FALSE(case2.isDefault);
		EXPECT_EQ(case2.value, 2);
		ASSERT_EQ(case2.body.size(), 2);
	}

	// Test switch with default
	{
		auto stmt = parseStatement("switch (x) { case 1: y = 10; break; default: y = 0; }");
		auto switchStmt = std::dynamic_pointer_cast<ast::SwitchStatementNode>(stmt);
		ASSERT_NE(switchStmt, nullptr);

		ASSERT_EQ(switchStmt->getCases().size(), 2);

		// Check case
		auto case1 = switchStmt->getCases()[0];
		EXPECT_FALSE(case1.isDefault);
		EXPECT_EQ(case1.value, 1);
		ASSERT_EQ(case1.body.size(), 2);

		// Check default
		auto defaultCase = switchStmt->getCases()[1];
		EXPECT_TRUE(defaultCase.isDefault);
		ASSERT_EQ(defaultCase.body.size(), 1);
	}

	// Test switch with multiple statements in case
	{
		auto stmt = parseStatement("switch (x) { case 1: y = 10; z = 20; break; }");
		auto switchStmt = std::dynamic_pointer_cast<ast::SwitchStatementNode>(stmt);
		ASSERT_NE(switchStmt, nullptr);

		ASSERT_EQ(switchStmt->getCases().size(), 1);

		// Check case
		auto case1 = switchStmt->getCases()[0];
		ASSERT_EQ(case1.body.size(), 3);
	}
}

/**
 * @test BreakContinueStatement
 * @brief Test break and continue statements
 *
 * Grammar rules tested:
 * - BREAK_STMT -> break ;
 * - CONTINUE_STMT -> continue ;
 */
TEST(ParserStatementTest, BreakContinueStatement) {
	// Test break statement
	{
		auto stmt = parseStatement("break;");
		auto breakStmt = std::dynamic_pointer_cast<ast::BreakStatementNode>(stmt);
		ASSERT_NE(breakStmt, nullptr);
	}

	// Test continue statement
	{
		auto stmt = parseStatement("continue;");
		auto continueStmt = std::dynamic_pointer_cast<ast::ContinueStatementNode>(stmt);
		ASSERT_NE(continueStmt, nullptr);
	}
}

/**
 * @test ReturnStatement
 * @brief Test return statements
 *
 * Grammar rules tested:
 * - RETURN_STMT -> return OPT_EXPR ;
 */
TEST(ParserStatementTest, ReturnStatement) {
	// Test return without expression
	{
		auto stmt = parseStatement("return;");
		auto returnStmt = std::dynamic_pointer_cast<ast::ReturnStatementNode>(stmt);
		ASSERT_NE(returnStmt, nullptr);
		EXPECT_FALSE(returnStmt->hasValue());
	}

	// Test return with expression
	{
		auto stmt = parseStatement("return x + y;");
		auto returnStmt = std::dynamic_pointer_cast<ast::ReturnStatementNode>(stmt);
		ASSERT_NE(returnStmt, nullptr);
		EXPECT_TRUE(returnStmt->hasValue());

		auto expr = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(returnStmt->getExpression());
		ASSERT_NE(expr, nullptr);
		EXPECT_EQ(expr->getOperator(), ast::BinaryExpressionNode::Operator::ADD);
	}
}

/**
 * @test ComplexStatements
 * @brief Test complex statements combining multiple features
 */
TEST(ParserStatementTest, ComplexStatements) {
	// Test complex if-else with nested statements
	{
		auto stmt = parseStatement(R"(
            if (x > 0) {
                int y = x * 2;
                if (y > 10) {
                    return y;
                } else {
                    y = y + 1;
                }
            } else {
                return 0;
            }
        )");

		auto ifStmt = std::dynamic_pointer_cast<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		auto thenBranch = std::dynamic_pointer_cast<ast::BlockStatementNode>(ifStmt->getThenBranch());
		ASSERT_NE(thenBranch, nullptr);
		EXPECT_EQ(thenBranch->getStatements().size(), 2);

		auto elseBranch = std::dynamic_pointer_cast<ast::BlockStatementNode>(ifStmt->getElseBranch());
		ASSERT_NE(elseBranch, nullptr);
		EXPECT_EQ(elseBranch->getStatements().size(), 1);

		auto innerIf = std::dynamic_pointer_cast<ast::IfStatementNode>(thenBranch->getStatements()[1]);
		ASSERT_NE(innerIf, nullptr);
		EXPECT_TRUE(innerIf->hasElseBranch());
	}

	// Test complex loop with multiple control statements
	{
		auto stmt = parseStatement(R"(
            for (int i = 0; i < 10; i++) {
                if (i % 2 == 0) {
                    continue;
                }
                sum = sum + i;
                if (sum > 20) {
                    break;
                }
            }
        )");

		auto forStmt = std::dynamic_pointer_cast<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		auto body = std::dynamic_pointer_cast<ast::BlockStatementNode>(forStmt->getBody());
		ASSERT_NE(body, nullptr);
		EXPECT_EQ(body->getStatements().size(), 3);
	}
}