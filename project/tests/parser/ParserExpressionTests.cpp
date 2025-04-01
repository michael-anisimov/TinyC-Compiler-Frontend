#include "tinyc/parser/Parser.h"
#include "tinyc/lexer/Lexer.h"
#include <gtest/gtest.h>
#include <sstream>
#include <memory>

using namespace tinyc;

/**
 * @file ParserExpressionTests.cpp
 * @brief Tests for TinyC expression parsing
 *
 * This file contains tests for expression parsing in TinyC, organized from simple
 * to complex. These tests verify the parser correctly builds AST nodes for all
 * TinyC expression types.
 */

namespace {
	/**
	 * @brief Helper function to cast a node to a specific type
	 * @tparam T The target node type
	 * @param node The node to cast
	 * @return Pointer to the node as type T, or nullptr if cast fails
	 */
	template<typename T>
	T* as(const ast::ASTNode* node) {
		return dynamic_cast<T*>(const_cast<ast::ASTNode*>(node));
	}

	/**
	 * @brief Helper function to parse an expression statement within a function
	 * @param expr The expression text to parse
	 * @return The parsed AST
	 */
	ast::ASTNodePtr parseExpression(const std::string &expr) {
		std::string source = "void test() { " + expr + "; }";
		lexer::Lexer lexer(source);
		parser::Parser parser(lexer);
		return parser.parseProgram();
	}

	/**
	 * @brief Helper function to get the expression from a parsed AST
	 * @param ast The AST returned by parseExpression
	 * @return Pointer to the expression node
	 */
	const ast::ASTNode* getExpressionNode(const ast::ASTNodePtr& ast) {
		const auto* program = as<ast::ProgramNode>(ast.get());
		EXPECT_NE(program, nullptr);
		if (!program) return nullptr;

		const auto* func = as<ast::FunctionDeclarationNode>(program->getDeclarations()[0].get());
		EXPECT_NE(func, nullptr);
		if (!func) return nullptr;

		const auto* body = as<ast::BlockStatementNode>(func->getBody().get());
		EXPECT_NE(body, nullptr);
		if (!body) return nullptr;

		const auto* exprStmt = as<ast::ExpressionStatementNode>(body->getStatements()[0].get());
		EXPECT_NE(exprStmt, nullptr);
		if (!exprStmt) return nullptr;

		return exprStmt->getExpression().get();
	}
}


/**
 * @test LiteralExpressions
 * @brief Test literal expressions (int, double, char, string)
 *
 * Grammar rules tested:
 * - F -> integer_literal | double_literal | char_literal | string_literal
 */
TEST(ParserExpressionTest, LiteralExpressions) {
	// Test integer literal
	{
		auto ast = parseExpression("42");
		const auto* expr = getExpressionNode(ast);
		const auto* literal = as<ast::LiteralNode>(expr);
		ASSERT_NE(literal, nullptr);
		EXPECT_EQ(literal->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(literal->getValue(), "42");
	}

	// Test double literal
	{
		auto ast = parseExpression("3.14");
		const auto* expr = getExpressionNode(ast);
		const auto* literal = as<ast::LiteralNode>(expr);
		ASSERT_NE(literal, nullptr);
		EXPECT_EQ(literal->getKind(), ast::LiteralNode::Kind::DOUBLE);
		// Value might be represented with different precision
		EXPECT_TRUE(literal->getValue().find("3.14") != std::string::npos);
	}

	// Test char literal
	{
		auto ast = parseExpression("'a'");
		const auto* expr = getExpressionNode(ast);
		const auto* literal = as<ast::LiteralNode>(expr);
		ASSERT_NE(literal, nullptr);
		EXPECT_EQ(literal->getKind(), ast::LiteralNode::Kind::CHAR);
		EXPECT_EQ(literal->getValue(), "a");
	}

	// Test string literal
	{
		auto ast = parseExpression("\"hello\"");
		const auto* expr = getExpressionNode(ast);
		const auto* literal = as<ast::LiteralNode>(expr);
		ASSERT_NE(literal, nullptr);
		EXPECT_EQ(literal->getKind(), ast::LiteralNode::Kind::STRING);
		EXPECT_EQ(literal->getValue(), "\"hello\"");
	}
}

/**
 * @test IdentifierExpressions
 * @brief Test identifier expressions
 *
 * Grammar rules tested:
 * - F -> identifier
 */
TEST(ParserExpressionTest, IdentifierExpressions) {
	auto ast = parseExpression("variable");
	const auto* expr = getExpressionNode(ast);
	const auto* identifier = as<ast::IdentifierNode>(expr);
	ASSERT_NE(identifier, nullptr);
	EXPECT_EQ(identifier->getIdentifier(), "variable");
}

/**
 * @test UnaryExpressions
 * @brief Test unary expressions
 *
 * Grammar rules tested:
 * - E_UNARY_PRE -> + E_UNARY_PRE | - E_UNARY_PRE | ! E_UNARY_PRE | ~ E_UNARY_PRE
 * - E_UNARY_PRE -> ++ E_UNARY_PRE | -- E_UNARY_PRE | * E_UNARY_PRE | & E_UNARY_PRE
 * - E_CALL_INDEX_MEMBER_POST_Prime -> E_POST E_CALL_INDEX_MEMBER_POST_Prime
 * - E_POST -> ++ | --
 */
TEST(ParserExpressionTest, UnaryExpressions) {
	// Test unary plus
	{
		auto ast = parseExpression("+x");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::POSITIVE);
		const auto* operand = as<ast::IdentifierNode>(unary->getOperand().get());
		ASSERT_NE(operand, nullptr);
		EXPECT_EQ(operand->getIdentifier(), "x");
	}

	// Test unary minus
	{
		auto ast = parseExpression("-x");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::NEGATIVE);
	}

	// Test logical not
	{
		auto ast = parseExpression("!x");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::LOGICAL_NOT);
	}

	// Test bitwise not
	{
		auto ast = parseExpression("~x");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::BITWISE_NOT);
	}

	// Test pre-increment
	{
		auto ast = parseExpression("++x");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::PRE_INCREMENT);
	}

	// Test pre-decrement
	{
		auto ast = parseExpression("--x");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::PRE_DECREMENT);
	}

	// Test dereference
	{
		auto ast = parseExpression("*ptr");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::DEREFERENCE);
	}

	// Test address-of
	{
		auto ast = parseExpression("&x");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::ADDRESS_OF);
	}

	// Test post-increment
	{
		auto ast = parseExpression("x++");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::POST_INCREMENT);
	}

	// Test post-decrement
	{
		auto ast = parseExpression("x--");
		const auto* expr = getExpressionNode(ast);
		const auto* unary = as<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::POST_DECREMENT);
	}
}

/**
 * @test BinaryExpressions
 * @brief Test binary expressions
 *
 * Grammar rules tested:
 * - E1 -> E_UNARY_PRE { ('*' | '/' | '%' ) E_UNARY_PRE }
 * - E2 -> E1 { ('+' | '-') E1 }
 * - E3 -> E2 { ('<<' | '>>') E2 }
 * - E4 -> E3 { ('<' | '<=' | '>' | '>=') E3 }
 * - E5 -> E4 { ('==' | '!=') E4 }
 * - E6 -> E5 { '&' E5 }
 * - E7 -> E6 { '|' E6 }
 * - E8 -> E7 { '&&' E7 }
 * - E9 -> E8 { '||' E8 }
 * - EXPR -> E9 [ '=' EXPR ]
 */
TEST(ParserExpressionTest, BinaryExpressions) {
	// Test multiplication
	{
		auto ast = parseExpression("a * b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);

		const auto* left = as<ast::IdentifierNode>(binary->getLeft().get());
		ASSERT_NE(left, nullptr);
		EXPECT_EQ(left->getIdentifier(), "a");

		const auto* right = as<ast::IdentifierNode>(binary->getRight().get());
		ASSERT_NE(right, nullptr);
		EXPECT_EQ(right->getIdentifier(), "b");
	}

	// Test division
	{
		auto ast = parseExpression("a / b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::DIVIDE);
	}

	// Test modulo
	{
		auto ast = parseExpression("a % b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::MODULO);
	}

	// Test addition
	{
		auto ast = parseExpression("a + b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::ADD);
	}

	// Test subtraction
	{
		auto ast = parseExpression("a - b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::SUBTRACT);
	}

	// Test left shift
	{
		auto ast = parseExpression("a << b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LEFT_SHIFT);
	}

	// Test right shift
	{
		auto ast = parseExpression("a >> b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::RIGHT_SHIFT);
	}

	// Test less than
	{
		auto ast = parseExpression("a < b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LESS);
	}

	// Test less than or equal
	{
		auto ast = parseExpression("a <= b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LESS_EQUAL);
	}

	// Test greater than
	{
		auto ast = parseExpression("a > b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);
	}

	// Test greater than or equal
	{
		auto ast = parseExpression("a >= b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::GREATER_EQUAL);
	}

	// Test equality
	{
		auto ast = parseExpression("a == b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::EQUAL);
	}

	// Test inequality
	{
		auto ast = parseExpression("a != b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::NOT_EQUAL);
	}

	// Test bitwise AND
	{
		auto ast = parseExpression("a & b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::BITWISE_AND);
	}

	// Test bitwise OR
	{
		auto ast = parseExpression("a | b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::BITWISE_OR);
	}

	// Test logical AND
	{
		auto ast = parseExpression("a && b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LOGICAL_AND);
	}

	// Test logical OR
	{
		auto ast = parseExpression("a || b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LOGICAL_OR);
	}

	// Test assignment
	{
		auto ast = parseExpression("a = b");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::ASSIGN);
	}
}

/**
 * @test OperatorPrecedence
 * @brief Test operator precedence in expressions
 */
TEST(ParserExpressionTest, OperatorPrecedence) {
	// Test multiplication before addition
	{
		auto ast = parseExpression("a + b * c");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::ADD);

		const auto* left = as<ast::IdentifierNode>(binary->getLeft().get());
		ASSERT_NE(left, nullptr);
		EXPECT_EQ(left->getIdentifier(), "a");

		const auto* right = as<ast::BinaryExpressionNode>(binary->getRight().get());
		ASSERT_NE(right, nullptr);
		EXPECT_EQ(right->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);
	}

	// Test parenthesized expression
	{
		auto ast = parseExpression("(a + b) * c");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);

		const auto* left = as<ast::BinaryExpressionNode>(binary->getLeft().get());
		ASSERT_NE(left, nullptr);
		EXPECT_EQ(left->getOperator(), ast::BinaryExpressionNode::Operator::ADD);

		const auto* right = as<ast::IdentifierNode>(binary->getRight().get());
		ASSERT_NE(right, nullptr);
		EXPECT_EQ(right->getIdentifier(), "c");
	}

	// Test complex expression with mixed operators
	{
		auto ast = parseExpression("a * b + c * d");
		const auto* expr = getExpressionNode(ast);
		const auto* binary = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::ADD);

		const auto* left = as<ast::BinaryExpressionNode>(binary->getLeft().get());
		ASSERT_NE(left, nullptr);
		EXPECT_EQ(left->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);

		const auto* right = as<ast::BinaryExpressionNode>(binary->getRight().get());
		ASSERT_NE(right, nullptr);
		EXPECT_EQ(right->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);
	}
}

/**
 * @test MemberExpressions
 * @brief Test member access expressions
 *
 * Grammar rules tested:
 * - E_CALL_INDEX_MEMBER_POST_Prime -> E_MEMBER E_CALL_INDEX_MEMBER_POST_Prime
 * - E_MEMBER -> . identifier | -> identifier
 */
TEST(ParserExpressionTest, MemberExpressions) {
	// Test dot operator
	{
		auto ast = parseExpression("point.x");
		const auto* expr = getExpressionNode(ast);
		const auto* member = as<ast::MemberExpressionNode>(expr);
		ASSERT_NE(member, nullptr);
		EXPECT_EQ(member->getKind(), ast::MemberExpressionNode::Kind::DOT);
		EXPECT_EQ(member->getMember(), "x");

		const auto* object = as<ast::IdentifierNode>(member->getObject().get());
		ASSERT_NE(object, nullptr);
		EXPECT_EQ(object->getIdentifier(), "point");
	}

	// Test arrow operator
	{
		auto ast = parseExpression("ptr->x");
		const auto* expr = getExpressionNode(ast);
		const auto* member = as<ast::MemberExpressionNode>(expr);
		ASSERT_NE(member, nullptr);
		EXPECT_EQ(member->getKind(), ast::MemberExpressionNode::Kind::ARROW);
		EXPECT_EQ(member->getMember(), "x");
	}

	// Test chained member access
	{
		auto ast = parseExpression("obj.inner.value");
		const auto* expr = getExpressionNode(ast);
		const auto* member1 = as<ast::MemberExpressionNode>(expr);
		ASSERT_NE(member1, nullptr);
		EXPECT_EQ(member1->getKind(), ast::MemberExpressionNode::Kind::DOT);
		EXPECT_EQ(member1->getMember(), "value");

		const auto* member2 = as<ast::MemberExpressionNode>(member1->getObject().get());
		ASSERT_NE(member2, nullptr);
		EXPECT_EQ(member2->getKind(), ast::MemberExpressionNode::Kind::DOT);
		EXPECT_EQ(member2->getMember(), "inner");

		const auto* object = as<ast::IdentifierNode>(member2->getObject().get());
		ASSERT_NE(object, nullptr);
		EXPECT_EQ(object->getIdentifier(), "obj");
	}
}

/**
 * @test IndexExpressions
 * @brief Test array index expressions
 *
 * Grammar rules tested:
 * - E_CALL_INDEX_MEMBER_POST_Prime -> E_INDEX E_CALL_INDEX_MEMBER_POST_Prime
 * - E_INDEX -> [ EXPR ]
 */
TEST(ParserExpressionTest, IndexExpressions) {
	// Test simple indexing
	{
		auto ast = parseExpression("arr[0]");
		const auto* expr = getExpressionNode(ast);
		const auto* index = as<ast::IndexExpressionNode>(expr);
		ASSERT_NE(index, nullptr);

		const auto* array = as<ast::IdentifierNode>(index->getArray().get());
		ASSERT_NE(array, nullptr);
		EXPECT_EQ(array->getIdentifier(), "arr");

		const auto* indexExpr = as<ast::LiteralNode>(index->getIndex().get());
		ASSERT_NE(indexExpr, nullptr);
		EXPECT_EQ(indexExpr->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(indexExpr->getValue(), "0");
	}

	// Test indexing with expression
	{
		auto ast = parseExpression("arr[i + 1]");
		const auto* expr = getExpressionNode(ast);
		const auto* index = as<ast::IndexExpressionNode>(expr);
		ASSERT_NE(index, nullptr);

		const auto* array = as<ast::IdentifierNode>(index->getArray().get());
		ASSERT_NE(array, nullptr);
		EXPECT_EQ(array->getIdentifier(), "arr");

		const auto* indexExpr = as<ast::BinaryExpressionNode>(index->getIndex().get());
		ASSERT_NE(indexExpr, nullptr);
		EXPECT_EQ(indexExpr->getOperator(), ast::BinaryExpressionNode::Operator::ADD);
	}

	// Test multi-dimensional indexing
	{
		auto ast = parseExpression("matrix[i][j]");
		const auto* expr = getExpressionNode(ast);
		const auto* index1 = as<ast::IndexExpressionNode>(expr);
		ASSERT_NE(index1, nullptr);

		const auto* index2 = as<ast::IndexExpressionNode>(index1->getArray().get());
		ASSERT_NE(index2, nullptr);

		const auto* array = as<ast::IdentifierNode>(index2->getArray().get());
		ASSERT_NE(array, nullptr);
		EXPECT_EQ(array->getIdentifier(), "matrix");
	}
}

/**
 * @test CallExpressions
 * @brief Test function call expressions
 *
 * Grammar rules tested:
 * - E_CALL_INDEX_MEMBER_POST_Prime -> E_CALL E_CALL_INDEX_MEMBER_POST_Prime
 * - E_CALL -> ( OPT_EXPR_LIST )
 */
TEST(ParserExpressionTest, CallExpressions) {
	// Test function call with no arguments
	{
		auto ast = parseExpression("foo()");
		const auto* expr = getExpressionNode(ast);
		const auto* call = as<ast::CallExpressionNode>(expr);
		ASSERT_NE(call, nullptr);

		const auto* callee = as<ast::IdentifierNode>(call->getCallee().get());
		ASSERT_NE(callee, nullptr);
		EXPECT_EQ(callee->getIdentifier(), "foo");

		EXPECT_EQ(call->getArguments().size(), 0);
	}

	// Test function call with arguments
	{
		auto ast = parseExpression("bar(1, 2)");
		const auto* expr = getExpressionNode(ast);
		const auto* call = as<ast::CallExpressionNode>(expr);
		ASSERT_NE(call, nullptr);

		const auto* callee = as<ast::IdentifierNode>(call->getCallee().get());
		ASSERT_NE(callee, nullptr);
		EXPECT_EQ(callee->getIdentifier(), "bar");

		ASSERT_EQ(call->getArguments().size(), 2);

		auto arg1 = as<ast::LiteralNode>(call->getArguments()[0].get());
		ASSERT_NE(arg1, nullptr);
		EXPECT_EQ(arg1->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(arg1->getValue(), "1");

		auto arg2 = as<ast::LiteralNode>(call->getArguments()[1].get());
		ASSERT_NE(arg2, nullptr);
		EXPECT_EQ(arg2->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(arg2->getValue(), "2");
	}

	// Test function call with expressions as arguments
	{
		auto ast = parseExpression("compute(a + b, c * d)");
		const auto* expr = getExpressionNode(ast);
		const auto* call = as<ast::CallExpressionNode>(expr);
		ASSERT_NE(call, nullptr);

		const auto* callee = as<ast::IdentifierNode>(call->getCallee().get());
		ASSERT_NE(callee, nullptr);
		EXPECT_EQ(callee->getIdentifier(), "compute");

		ASSERT_EQ(call->getArguments().size(), 2);

		const auto* arg1 = as<ast::BinaryExpressionNode>(call->getArguments()[0].get());
		ASSERT_NE(arg1, nullptr);
		EXPECT_EQ(arg1->getOperator(), ast::BinaryExpressionNode::Operator::ADD);

		const auto* arg2 = as<ast::BinaryExpressionNode>(call->getArguments()[1].get());
		ASSERT_NE(arg2, nullptr);
		EXPECT_EQ(arg2->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);
	}

	// Test nested function calls
	{
		auto ast = parseExpression("outer(inner(x))");
		const auto* expr = getExpressionNode(ast);
		const auto* outerCall = as<ast::CallExpressionNode>(expr);
		ASSERT_NE(outerCall, nullptr);

		const auto* outerCallee = as<ast::IdentifierNode>(outerCall->getCallee().get());
		ASSERT_NE(outerCallee, nullptr);
		EXPECT_EQ(outerCallee->getIdentifier(), "outer");

		ASSERT_EQ(outerCall->getArguments().size(), 1);

		const auto* innerCall = as<ast::CallExpressionNode>(outerCall->getArguments()[0].get());
		ASSERT_NE(innerCall, nullptr);

		const auto* innerCallee = as<ast::IdentifierNode>(innerCall->getCallee().get());
		ASSERT_NE(innerCallee, nullptr);
		EXPECT_EQ(innerCallee->getIdentifier(), "inner");
	}
}

/**
 * @test CastExpressions
 * @brief Test cast expressions
 *
 * Grammar rules tested:
 * - F -> E_CAST
 * - E_CAST -> cast < TYPE > ( EXPR )
 */
TEST(ParserExpressionTest, CastExpressions) {
	// Test cast to int
	{
		auto ast = parseExpression("cast<int>(3.14)");
		const auto* expr = getExpressionNode(ast);
		const auto* cast = as<ast::CastExpressionNode>(expr);
		ASSERT_NE(cast, nullptr);

		const auto* targetType = as<ast::PrimitiveTypeNode>(cast->getTargetType().get());
		ASSERT_NE(targetType, nullptr);
		EXPECT_EQ(targetType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		const auto* expression = as<ast::LiteralNode>(cast->getExpression().get());
		ASSERT_NE(expression, nullptr);
		EXPECT_EQ(expression->getKind(), ast::LiteralNode::Kind::DOUBLE);
	}

	// Test cast to pointer
	{
		auto ast = parseExpression("cast<void*>(ptr)");
		const auto* expr = getExpressionNode(ast);
		const auto* cast = as<ast::CastExpressionNode>(expr);
		ASSERT_NE(cast, nullptr);

		const auto* targetType = as<ast::PointerTypeNode>(cast->getTargetType().get());
		ASSERT_NE(targetType, nullptr);

		const auto* baseType = as<ast::PrimitiveTypeNode>(targetType->getBaseType().get());
		ASSERT_NE(baseType, nullptr);
		EXPECT_EQ(baseType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		const auto* expression = as<ast::IdentifierNode>(cast->getExpression().get());
		ASSERT_NE(expression, nullptr);
		EXPECT_EQ(expression->getIdentifier(), "ptr");
	}

	// Test cast with complex expression
	{
		auto ast = parseExpression("cast<int>(a + b)");
		const auto* expr = getExpressionNode(ast);
		const auto* cast = as<ast::CastExpressionNode>(expr);
		ASSERT_NE(cast, nullptr);

		const auto* targetType = as<ast::PrimitiveTypeNode>(cast->getTargetType().get());
		ASSERT_NE(targetType, nullptr);
		EXPECT_EQ(targetType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		const auto* expression = as<ast::BinaryExpressionNode>(cast->getExpression().get());
		ASSERT_NE(expression, nullptr);
		EXPECT_EQ(expression->getOperator(), ast::BinaryExpressionNode::Operator::ADD);
	}
}

/**
 * @test CommaExpressions
 * @brief Test comma expressions
 *
 * Grammar rules tested:
 * - EXPRS -> EXPR EXPRS_TAIL
 * - EXPRS_TAIL -> , EXPR EXPRS_TAIL | ε
 */
TEST(ParserExpressionTest, CommaExpressions) {
	auto ast = parseExpression("a = 1, b = 2, c = 3");
	const auto* expr = getExpressionNode(ast);
	const auto* comma = as<ast::CommaExpressionNode>(expr);
	ASSERT_NE(comma, nullptr);
	EXPECT_EQ(comma->getExpressions().size(), 3);

	for (size_t i = 0; i < 3; ++i) {
		const auto* assignExpr = as<ast::BinaryExpressionNode>(comma->getExpressions()[i].get());
		ASSERT_NE(assignExpr, nullptr);
		EXPECT_EQ(assignExpr->getOperator(), ast::BinaryExpressionNode::Operator::ASSIGN);
	}
}

/**
 * @test ComplexExpressions
 * @brief Test complex expressions combining multiple features
 */
TEST(ParserExpressionTest, ComplexExpressions) {
	// Test complex expression with function call, member access, and indexing
	{
		auto ast = parseExpression("getData()->items[getIndex()].value");
		const auto* expr = getExpressionNode(ast);

		// Should be a member access with DOT operator
		const auto* member = as<ast::MemberExpressionNode>(expr);
		ASSERT_NE(member, nullptr);
		EXPECT_EQ(member->getKind(), ast::MemberExpressionNode::Kind::DOT);
		EXPECT_EQ(member->getMember(), "value");

		// Object should be an array indexing
		const auto* index = as<ast::IndexExpressionNode>(member->getObject().get());
		ASSERT_NE(index, nullptr);

		// Index expression should be a function call
		const auto* indexExpr = as<ast::CallExpressionNode>(index->getIndex().get());
		ASSERT_NE(indexExpr, nullptr);

		const auto* indexCallee = as<ast::IdentifierNode>(indexExpr->getCallee().get());
		ASSERT_NE(indexCallee, nullptr);
		EXPECT_EQ(indexCallee->getIdentifier(), "getIndex");

		// Array should be a member access with ARROW operator
		const auto* innerMember = as<ast::MemberExpressionNode>(index->getArray().get());
		ASSERT_NE(innerMember, nullptr);
		EXPECT_EQ(innerMember->getKind(), ast::MemberExpressionNode::Kind::ARROW);
		EXPECT_EQ(innerMember->getMember(), "items");

		// Object of arrow operator should be a function call
		const auto* call = as<ast::CallExpressionNode>(innerMember->getObject().get());
		ASSERT_NE(call, nullptr);

		const auto* callee = as<ast::IdentifierNode>(call->getCallee().get());
		ASSERT_NE(callee, nullptr);
		EXPECT_EQ(callee->getIdentifier(), "getData");
	}

	// Test complex expression with arithmetic, logical, and comparison operators
	{
		auto ast = parseExpression("(a + b) * c > d && e || f == g");
		const auto* expr = getExpressionNode(ast);

		// Should be a logical OR expression
		const auto* logicalOr = as<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(logicalOr, nullptr);
		EXPECT_EQ(logicalOr->getOperator(), ast::BinaryExpressionNode::Operator::LOGICAL_OR);

		// Left side should be a logical AND expression
		const auto* logicalAnd = as<ast::BinaryExpressionNode>(logicalOr->getLeft().get());
		ASSERT_NE(logicalAnd, nullptr);
		EXPECT_EQ(logicalAnd->getOperator(), ast::BinaryExpressionNode::Operator::LOGICAL_AND);

		// Right side should be an equality comparison
		const auto* equality = as<ast::BinaryExpressionNode>(logicalOr->getRight().get());
		ASSERT_NE(equality, nullptr);
		EXPECT_EQ(equality->getOperator(), ast::BinaryExpressionNode::Operator::EQUAL);

		// Left side of logical AND should be a greater-than comparison
		const auto* greaterThan = as<ast::BinaryExpressionNode>(logicalAnd->getLeft().get());
		ASSERT_NE(greaterThan, nullptr);
		EXPECT_EQ(greaterThan->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		// Left side of greater-than should be a multiplication
		const auto* multiply = as<ast::BinaryExpressionNode>(greaterThan->getLeft().get());
		ASSERT_NE(multiply, nullptr);
		EXPECT_EQ(multiply->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);

		// Left side of multiplication should be an addition in parentheses
		const auto* addition = as<ast::BinaryExpressionNode>(multiply->getLeft().get());
		ASSERT_NE(addition, nullptr);
		EXPECT_EQ(addition->getOperator(), ast::BinaryExpressionNode::Operator::ADD);
	}
}