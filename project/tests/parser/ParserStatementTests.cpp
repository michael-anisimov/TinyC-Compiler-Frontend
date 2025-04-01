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
	// Helper function to cast a node to a specific type
	template<typename T>
	T* as(const ast::ASTNode* node) {
		return dynamic_cast<T*>(const_cast<ast::ASTNode*>(node));
	}

	// Helper function to parse a statement within a function
	ast::ASTNodePtr parseStatement(const std::string &stmt) {
		std::string source = "void test() { " + stmt + " }";
		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);
		return parser.parseProgram();
	}

	// Helper function to get the statement node from a parsed AST
	const ast::ASTNode* getStatementNode(const ast::ASTNodePtr& ast) {
		const auto* program = as<ast::ProgramNode>(ast.get());
		EXPECT_NE(program, nullptr);
		if (!program) return nullptr;

		const auto* func = as<ast::FunctionDeclarationNode>(program->getDeclarations()[0].get());
		EXPECT_NE(func, nullptr);
		if (!func) return nullptr;

		const auto* body = as<ast::BlockStatementNode>(func->getBody().get());
		EXPECT_NE(body, nullptr);
		if (!body) return nullptr;

		return body->getStatements()[0].get();
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
		auto ast = parseStatement("x = 42;");
		const auto* stmt = getStatementNode(ast);
		const auto* exprStmt = as<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		const auto* expr = as<ast::BinaryExpressionNode>(exprStmt->getExpression().get());
		ASSERT_NE(expr, nullptr);
		EXPECT_EQ(expr->getOperator(), ast::BinaryExpressionNode::Operator::ASSIGN);
	}

	// Test function call statement
	{
		auto ast = parseStatement("foo(1, 2, 3);");
		const auto* stmt = getStatementNode(ast);
		const auto* exprStmt = as<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		const auto* expr = as<ast::CallExpressionNode>(exprStmt->getExpression().get());
		ASSERT_NE(expr, nullptr);
		EXPECT_EQ(expr->getArguments().size(), 3);
	}

	// Test post-increment statement
	{
		auto ast = parseStatement("x++;");
		const auto* stmt = getStatementNode(ast);
		const auto* exprStmt = as<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		const auto* expr = as<ast::UnaryExpressionNode>(exprStmt->getExpression().get());
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
		auto ast = parseStatement("int x;");
		const auto* stmt = getStatementNode(ast);
		const auto* exprStmt = as<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		const auto* varDecl = as<ast::VariableNode>(exprStmt->getExpression().get());
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "x");

		const auto* type = as<ast::PrimitiveTypeNode>(varDecl->getType().get());
		ASSERT_NE(type, nullptr);
		EXPECT_EQ(type->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		EXPECT_FALSE(varDecl->hasInitializer());
	}

	// Test variable declaration with initializer
	{
		auto ast = parseStatement("double pi = 3.14159;");
		const auto* stmt = getStatementNode(ast);
		const auto* exprStmt = as<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		const auto* varDecl = as<ast::VariableNode>(exprStmt->getExpression().get());
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

	// Test array declaration
	{
		auto ast = parseStatement("int numbers[10];");
		const auto* stmt = getStatementNode(ast);
		const auto* exprStmt = as<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		const auto* varDecl = as<ast::VariableNode>(exprStmt->getExpression().get());
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

	// Test multiple variable declarations
	{
		auto ast = parseStatement("int a = 1, int b = 2, int c = 3;");
		const auto* stmt = getStatementNode(ast);
		const auto* exprStmt = as<ast::ExpressionStatementNode>(stmt);
		ASSERT_NE(exprStmt, nullptr);

		const auto* multiDecl = as<ast::MultipleDeclarationNode>(exprStmt->getExpression().get());
		ASSERT_NE(multiDecl, nullptr);

		// Get the declarations from the multiple declaration node
		const auto& declarations = multiDecl->getDeclarations();
		ASSERT_EQ(declarations.size(), 3);

		// Check the first variable
		const auto* firstVar = as<ast::VariableNode>(declarations[0].get());
		ASSERT_NE(firstVar, nullptr);
		EXPECT_EQ(firstVar->getIdentifier(), "a");

		const auto* firstType = as<ast::PrimitiveTypeNode>(firstVar->getType().get());
		ASSERT_NE(firstType, nullptr);
		EXPECT_EQ(firstType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		EXPECT_TRUE(firstVar->hasInitializer());
		const auto* firstInit = as<ast::LiteralNode>(firstVar->getInitializer().get());
		ASSERT_NE(firstInit, nullptr);
		EXPECT_EQ(firstInit->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(firstInit->getValue(), "1");

		// Check the second variable
		const auto* secondVar = as<ast::VariableNode>(declarations[1].get());
		ASSERT_NE(secondVar, nullptr);
		EXPECT_EQ(secondVar->getIdentifier(), "b");
		EXPECT_TRUE(secondVar->hasInitializer());

		// Check the third variable
		const auto* thirdVar = as<ast::VariableNode>(declarations[2].get());
		ASSERT_NE(thirdVar, nullptr);
		EXPECT_EQ(thirdVar->getIdentifier(), "c");
		EXPECT_TRUE(thirdVar->hasInitializer());
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
		auto ast = parseStatement("{}");
		const auto* stmt = getStatementNode(ast);
		const auto* block = as<ast::BlockStatementNode>(stmt);
		ASSERT_NE(block, nullptr);
		EXPECT_EQ(block->getStatements().size(), 0);
	}

	// Test block with statements
	{
		auto ast = parseStatement("{ int x = 10; x = x + 1; foo(x); }");
		const auto* stmt = getStatementNode(ast);
		const auto* block = as<ast::BlockStatementNode>(stmt);
		ASSERT_NE(block, nullptr);
		ASSERT_EQ(block->getStatements().size(), 3);

		const auto* exprStmt0 = as<ast::ExpressionStatementNode>(block->getStatements()[0].get());
		ASSERT_NE(exprStmt0, nullptr);

		const auto* varDecl = as<ast::VariableNode>(exprStmt0->getExpression().get());
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "x");

		// Second statement is an assignment expression
		const auto* exprStmt1 = as<ast::ExpressionStatementNode>(block->getStatements()[1].get());
		ASSERT_NE(exprStmt1, nullptr);

		// Third statement is a function call
		const auto* exprStmt2 = as<ast::ExpressionStatementNode>(block->getStatements()[2].get());
		ASSERT_NE(exprStmt2, nullptr);
	}

	// Test nested blocks
	{
		auto ast = parseStatement("{ int x = 10; { int y = 20; } }");
		const auto* stmt = getStatementNode(ast);
		const auto* block = as<ast::BlockStatementNode>(stmt);
		ASSERT_NE(block, nullptr);
		ASSERT_EQ(block->getStatements().size(), 2);

		const auto* innerBlock = as<ast::BlockStatementNode>(block->getStatements()[1].get());
		ASSERT_NE(innerBlock, nullptr);
		ASSERT_EQ(innerBlock->getStatements().size(), 1);

		const auto* exprStmnt = as<ast::ExpressionStatementNode>(innerBlock->getStatements()[0].get());
		ASSERT_NE(exprStmnt, nullptr);

		const auto* varDecl = as<ast::VariableNode>(exprStmnt->getExpression().get());
		ASSERT_NE(varDecl, nullptr);
		EXPECT_EQ(varDecl->getIdentifier(), "y");
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
		auto ast = parseStatement("if (x > 0) x = x - 1;");
		const auto* stmt = getStatementNode(ast);
		const auto* ifStmt = as<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		const auto* condition = as<ast::BinaryExpressionNode>(ifStmt->getCondition().get());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		const auto* thenBranch = as<ast::ExpressionStatementNode>(ifStmt->getThenBranch().get());
		ASSERT_NE(thenBranch, nullptr);

		EXPECT_FALSE(ifStmt->hasElseBranch());
		EXPECT_EQ(ifStmt->getElseBranch(), nullptr);
	}

	// Test if with else
	{
		auto ast = parseStatement("if (x > 0) x = x - 1; else x = 0;");
		const auto* stmt = getStatementNode(ast);
		const auto* ifStmt = as<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		const auto* condition = as<ast::BinaryExpressionNode>(ifStmt->getCondition().get());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		const auto* thenBranch = as<ast::ExpressionStatementNode>(ifStmt->getThenBranch().get());
		ASSERT_NE(thenBranch, nullptr);

		EXPECT_TRUE(ifStmt->hasElseBranch());
		const auto* elseBranch = as<ast::ExpressionStatementNode>(ifStmt->getElseBranch().get());
		ASSERT_NE(elseBranch, nullptr);
	}

	// Test if with block
	{
		auto ast = parseStatement("if (x > 0) { x = x - 1; y = y + 1; }");
		const auto* stmt = getStatementNode(ast);
		const auto* ifStmt = as<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		const auto* thenBranch = as<ast::BlockStatementNode>(ifStmt->getThenBranch().get());
		ASSERT_NE(thenBranch, nullptr);
		EXPECT_EQ(thenBranch->getStatements().size(), 2);
	}

	// Test if-else with blocks
	{
		auto ast = parseStatement("if (x > 0) { x = x - 1; } else { x = 0; }");
		const auto* stmt = getStatementNode(ast);
		const auto* ifStmt = as<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		const auto* thenBranch = as<ast::BlockStatementNode>(ifStmt->getThenBranch().get());
		ASSERT_NE(thenBranch, nullptr);
		EXPECT_EQ(thenBranch->getStatements().size(), 1);

		EXPECT_TRUE(ifStmt->hasElseBranch());
		const auto* elseBranch = as<ast::BlockStatementNode>(ifStmt->getElseBranch().get());
		ASSERT_NE(elseBranch, nullptr);
		EXPECT_EQ(elseBranch->getStatements().size(), 1);
	}

	// Test nested if-else
	{
		auto ast = parseStatement("if (x > 0) if (y > 0) z = 1; else z = 2;");
		const auto* stmt = getStatementNode(ast);
		const auto* outerIf = as<ast::IfStatementNode>(stmt);
		ASSERT_NE(outerIf, nullptr);

		const auto* innerIf = as<ast::IfStatementNode>(outerIf->getThenBranch().get());
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
		auto ast = parseStatement("while (x > 0) x = x - 1;");
		const auto* stmt = getStatementNode(ast);
		const auto* whileStmt = as<ast::WhileStatementNode>(stmt);
		ASSERT_NE(whileStmt, nullptr);

		const auto* condition = as<ast::BinaryExpressionNode>(whileStmt->getCondition().get());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		const auto* body = as<ast::ExpressionStatementNode>(whileStmt->getBody().get());
		ASSERT_NE(body, nullptr);
	}

	// Test while loop with block
	{
		auto ast = parseStatement("while (x > 0) { x = x - 1; sum = sum + x; }");
		const auto* stmt = getStatementNode(ast);
		const auto* whileStmt = as<ast::WhileStatementNode>(stmt);
		ASSERT_NE(whileStmt, nullptr);

		const auto* body = as<ast::BlockStatementNode>(whileStmt->getBody().get());
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
		auto ast = parseStatement("do x = x - 1; while (x > 0);");
		const auto* stmt = getStatementNode(ast);
		const auto* doWhileStmt = as<ast::DoWhileStatementNode>(stmt);
		ASSERT_NE(doWhileStmt, nullptr);

		const auto* condition = as<ast::BinaryExpressionNode>(doWhileStmt->getCondition().get());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		const auto* body = as<ast::ExpressionStatementNode>(doWhileStmt->getBody().get());
		ASSERT_NE(body, nullptr);
	}

	// Test do-while loop with block
	{
		auto ast = parseStatement("do { x = x - 1; sum = sum + x; } while (x > 0);");
		const auto* stmt = getStatementNode(ast);
		const auto* doWhileStmt = as<ast::DoWhileStatementNode>(stmt);
		ASSERT_NE(doWhileStmt, nullptr);

		const auto* body = as<ast::BlockStatementNode>(doWhileStmt->getBody().get());
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
		auto ast = parseStatement("for (int i = 0; i < 10; i = i + 1) sum = sum + i;");
		const auto* stmt = getStatementNode(ast);
		const auto* forStmt = as<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		const auto* init = as<ast::VariableNode>(forStmt->getInitialization().get());
		ASSERT_NE(init, nullptr);
		EXPECT_EQ(init->getIdentifier(), "i");

		const auto* condition = as<ast::BinaryExpressionNode>(forStmt->getCondition().get());
		ASSERT_NE(condition, nullptr);
		EXPECT_EQ(condition->getOperator(), ast::BinaryExpressionNode::Operator::LESS);

		const auto* update = as<ast::BinaryExpressionNode>(forStmt->getUpdate().get());
		ASSERT_NE(update, nullptr);
		EXPECT_EQ(update->getOperator(), ast::BinaryExpressionNode::Operator::ASSIGN);

		const auto* body = as<ast::ExpressionStatementNode>(forStmt->getBody().get());
		ASSERT_NE(body, nullptr);
	}

	// Test for loop with missing parts
	{
		auto ast = parseStatement("for (;;) sum = sum + 1;");
		const auto* stmt = getStatementNode(ast);
		const auto* forStmt = as<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		EXPECT_EQ(forStmt->getInitialization(), nullptr);
		EXPECT_EQ(forStmt->getCondition(), nullptr);
		EXPECT_EQ(forStmt->getUpdate(), nullptr);

		const auto* body = as<ast::ExpressionStatementNode>(forStmt->getBody().get());
		ASSERT_NE(body, nullptr);
	}

	// Test for loop with expression initialization
	{
		auto ast = parseStatement("for (i = 0; i < 10; i = i + 1) sum = sum + i;");
		const auto* stmt = getStatementNode(ast);
		const auto* forStmt = as<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		const auto* init = as<ast::BinaryExpressionNode>(forStmt->getInitialization().get());
		ASSERT_NE(init, nullptr);
		EXPECT_EQ(init->getOperator(), ast::BinaryExpressionNode::Operator::ASSIGN);
	}

	// Test for loop with block
	{
		auto ast = parseStatement("for (int i = 0; i < 10; i++) { sum = sum + i; }");
		const auto* stmt = getStatementNode(ast);
		const auto* forStmt = as<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		const auto* body = as<ast::BlockStatementNode>(forStmt->getBody().get());
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
		auto ast = parseStatement("switch (x) { case 1: y = 10; break; case 2: y = 20; break; }");
		const auto* stmt = getStatementNode(ast);
		const auto* switchStmt = as<ast::SwitchStatementNode>(stmt);
		ASSERT_NE(switchStmt, nullptr);

		const auto* expr = as<ast::IdentifierNode>(switchStmt->getExpression().get());
		ASSERT_NE(expr, nullptr);
		EXPECT_EQ(expr->getIdentifier(), "x");

		ASSERT_EQ(switchStmt->getCases().size(), 2);

		// Check first case - using reference to avoid copying
		const auto& case1 = switchStmt->getCases()[0];
		EXPECT_FALSE(case1.isDefault);
		EXPECT_EQ(case1.value, 1);
		ASSERT_EQ(case1.body.size(), 2);

		// Check second case - using reference to avoid copying
		const auto& case2 = switchStmt->getCases()[1];
		EXPECT_FALSE(case2.isDefault);
		EXPECT_EQ(case2.value, 2);
		ASSERT_EQ(case2.body.size(), 2);
	}

	// Test switch with default
	{
		auto ast = parseStatement("switch (x) { case 1: y = 10; break; default: y = 0; }");
		const auto* stmt = getStatementNode(ast);
		const auto* switchStmt = as<ast::SwitchStatementNode>(stmt);
		ASSERT_NE(switchStmt, nullptr);

		ASSERT_EQ(switchStmt->getCases().size(), 2);

		// Check case - using reference to avoid copying
		const auto& case1 = switchStmt->getCases()[0];
		EXPECT_FALSE(case1.isDefault);
		EXPECT_EQ(case1.value, 1);
		ASSERT_EQ(case1.body.size(), 2);

		// Check default - using reference to avoid copying
		const auto& defaultCase = switchStmt->getCases()[1];
		EXPECT_TRUE(defaultCase.isDefault);
		ASSERT_EQ(defaultCase.body.size(), 1);
	}

	// Test switch with multiple statements in case
	{
		auto ast = parseStatement("switch (x) { case 1: y = 10; z = 20; break; }");
		const auto* stmt = getStatementNode(ast);
		const auto* switchStmt = as<ast::SwitchStatementNode>(stmt);
		ASSERT_NE(switchStmt, nullptr);

		ASSERT_EQ(switchStmt->getCases().size(), 1);

		// Check case - using reference to avoid copying
		const auto& case1 = switchStmt->getCases()[0];
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
		auto ast = parseStatement("break;");
		const auto* stmt = getStatementNode(ast);
		const auto* breakStmt = as<ast::BreakStatementNode>(stmt);
		ASSERT_NE(breakStmt, nullptr);
	}

	// Test continue statement
	{
		auto ast = parseStatement("continue;");
		const auto* stmt = getStatementNode(ast);
		const auto* continueStmt = as<ast::ContinueStatementNode>(stmt);
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
		auto ast = parseStatement("return;");
		const auto* stmt = getStatementNode(ast);
		const auto* returnStmt = as<ast::ReturnStatementNode>(stmt);
		ASSERT_NE(returnStmt, nullptr);
		EXPECT_FALSE(returnStmt->hasValue());
	}

	// Test return with expression
	{
		auto ast = parseStatement("return x + y;");
		const auto* stmt = getStatementNode(ast);
		const auto* returnStmt = as<ast::ReturnStatementNode>(stmt);
		ASSERT_NE(returnStmt, nullptr);
		EXPECT_TRUE(returnStmt->hasValue());

		const auto* expr = as<ast::BinaryExpressionNode>(returnStmt->getExpression().get());
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
		auto ast = parseStatement(R"(
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

		const auto* stmt = getStatementNode(ast);
		const auto* ifStmt = as<ast::IfStatementNode>(stmt);
		ASSERT_NE(ifStmt, nullptr);

		const auto* thenBranch = as<ast::BlockStatementNode>(ifStmt->getThenBranch().get());
		ASSERT_NE(thenBranch, nullptr);
		EXPECT_EQ(thenBranch->getStatements().size(), 2);

		const auto* elseBranch = as<ast::BlockStatementNode>(ifStmt->getElseBranch().get());
		ASSERT_NE(elseBranch, nullptr);
		EXPECT_EQ(elseBranch->getStatements().size(), 1);

		const auto* innerIf = as<ast::IfStatementNode>(thenBranch->getStatements()[1].get());
		ASSERT_NE(innerIf, nullptr);
		EXPECT_TRUE(innerIf->hasElseBranch());
	}

	// Test complex loop with multiple control statements
	{
		auto ast = parseStatement(R"(
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

		const auto* stmt = getStatementNode(ast);
		const auto* forStmt = as<ast::ForStatementNode>(stmt);
		ASSERT_NE(forStmt, nullptr);

		const auto* body = as<ast::BlockStatementNode>(forStmt->getBody().get());
		ASSERT_NE(body, nullptr);
		EXPECT_EQ(body->getStatements().size(), 3);
	}
}