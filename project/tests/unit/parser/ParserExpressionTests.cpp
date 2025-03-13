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
	// Helper function to parse an expression statement within a function
	ast::ASTNodePtr parseExpression(const std::string &expr) {
		std::string source = "void test() { " + expr + "; }";
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

		auto exprStmt = std::dynamic_pointer_cast<ast::ExpressionStatementNode>(body->getStatements()[0]);
		EXPECT_NE(exprStmt, nullptr);

		return exprStmt->getExpression();
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
		auto expr = parseExpression("42");
		auto literal = std::dynamic_pointer_cast<ast::LiteralNode>(expr);
		ASSERT_NE(literal, nullptr);
		EXPECT_EQ(literal->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(literal->getValue(), "42");
	}

	// Test double literal
	{
		auto expr = parseExpression("3.14");
		auto literal = std::dynamic_pointer_cast<ast::LiteralNode>(expr);
		ASSERT_NE(literal, nullptr);
		EXPECT_EQ(literal->getKind(), ast::LiteralNode::Kind::DOUBLE);
		// Value might be represented with different precision
		EXPECT_TRUE(literal->getValue().find("3.14") != std::string::npos);
	}

	// Test char literal
	{
		auto expr = parseExpression("'a'");
		auto literal = std::dynamic_pointer_cast<ast::LiteralNode>(expr);
		ASSERT_NE(literal, nullptr);
		EXPECT_EQ(literal->getKind(), ast::LiteralNode::Kind::CHAR);
		EXPECT_EQ(literal->getValue(), "a");
	}

	// Test string literal
	{
		auto expr = parseExpression("\"hello\"");
		auto literal = std::dynamic_pointer_cast<ast::LiteralNode>(expr);
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
	auto expr = parseExpression("variable");
	auto identifier = std::dynamic_pointer_cast<ast::IdentifierNode>(expr);
	ASSERT_NE(identifier, nullptr);
	EXPECT_EQ(identifier->getName(), "variable");
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
		auto expr = parseExpression("+x");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::POSITIVE);
		auto operand = std::dynamic_pointer_cast<ast::IdentifierNode>(unary->getOperand());
		ASSERT_NE(operand, nullptr);
		EXPECT_EQ(operand->getName(), "x");
	}

	// Test unary minus
	{
		auto expr = parseExpression("-x");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::NEGATIVE);
	}

	// Test logical not
	{
		auto expr = parseExpression("!x");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::LOGICAL_NOT);
	}

	// Test bitwise not
	{
		auto expr = parseExpression("~x");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::BITWISE_NOT);
	}

	// Test pre-increment
	{
		auto expr = parseExpression("++x");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::PRE_INCREMENT);
	}

	// Test pre-decrement
	{
		auto expr = parseExpression("--x");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::PRE_DECREMENT);
	}

	// Test dereference
	{
		auto expr = parseExpression("*ptr");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::DEREFERENCE);
	}

	// Test address-of
	{
		auto expr = parseExpression("&x");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::ADDRESS_OF);
	}

	// Test post-increment
	{
		auto expr = parseExpression("x++");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
		ASSERT_NE(unary, nullptr);
		EXPECT_EQ(unary->getOperator(), ast::UnaryExpressionNode::Operator::POST_INCREMENT);
	}

	// Test post-decrement
	{
		auto expr = parseExpression("x--");
		auto unary = std::dynamic_pointer_cast<ast::UnaryExpressionNode>(expr);
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
		auto expr = parseExpression("a * b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);

		auto left = std::dynamic_pointer_cast<ast::IdentifierNode>(binary->getLeft());
		ASSERT_NE(left, nullptr);
		EXPECT_EQ(left->getName(), "a");

		auto right = std::dynamic_pointer_cast<ast::IdentifierNode>(binary->getRight());
		ASSERT_NE(right, nullptr);
		EXPECT_EQ(right->getName(), "b");
	}

	// Test division
	{
		auto expr = parseExpression("a / b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::DIVIDE);
	}

	// Test modulo
	{
		auto expr = parseExpression("a % b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::MODULO);
	}

	// Test addition
	{
		auto expr = parseExpression("a + b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::ADD);
	}

	// Test subtraction
	{
		auto expr = parseExpression("a - b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::SUBTRACT);
	}

	// Test left shift
	{
		auto expr = parseExpression("a << b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LEFT_SHIFT);
	}

	// Test right shift
	{
		auto expr = parseExpression("a >> b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::RIGHT_SHIFT);
	}

	// Test less than
	{
		auto expr = parseExpression("a < b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LESS);
	}

	// Test less than or equal
	{
		auto expr = parseExpression("a <= b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LESS_EQUAL);
	}

	// Test greater than
	{
		auto expr = parseExpression("a > b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);
	}

	// Test greater than or equal
	{
		auto expr = parseExpression("a >= b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::GREATER_EQUAL);
	}

	// Test equality
	{
		auto expr = parseExpression("a == b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::EQUAL);
	}

	// Test inequality
	{
		auto expr = parseExpression("a != b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::NOT_EQUAL);
	}

	// Test bitwise AND
	{
		auto expr = parseExpression("a & b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::BITWISE_AND);
	}

	// Test bitwise OR
	{
		auto expr = parseExpression("a | b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::BITWISE_OR);
	}

	// Test logical AND
	{
		auto expr = parseExpression("a && b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LOGICAL_AND);
	}

	// Test logical OR
	{
		auto expr = parseExpression("a || b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::LOGICAL_OR);
	}

	// Test assignment
	{
		auto expr = parseExpression("a = b");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
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
		auto expr = parseExpression("a + b * c");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::ADD);

		auto left = std::dynamic_pointer_cast<ast::IdentifierNode>(binary->getLeft());
		ASSERT_NE(left, nullptr);
		EXPECT_EQ(left->getName(), "a");

		auto right = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(binary->getRight());
		ASSERT_NE(right, nullptr);
		EXPECT_EQ(right->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);
	}

	// Test parenthesized expression
	{
		auto expr = parseExpression("(a + b) * c");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);

		auto left = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(binary->getLeft());
		ASSERT_NE(left, nullptr);
		EXPECT_EQ(left->getOperator(), ast::BinaryExpressionNode::Operator::ADD);

		auto right = std::dynamic_pointer_cast<ast::IdentifierNode>(binary->getRight());
		ASSERT_NE(right, nullptr);
		EXPECT_EQ(right->getName(), "c");
	}

	// Test complex expression with mixed operators
	{
		auto expr = parseExpression("a * b + c * d");
		auto binary = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(binary, nullptr);
		EXPECT_EQ(binary->getOperator(), ast::BinaryExpressionNode::Operator::ADD);

		auto left = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(binary->getLeft());
		ASSERT_NE(left, nullptr);
		EXPECT_EQ(left->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);

		auto right = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(binary->getRight());
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
		auto expr = parseExpression("point.x");
		auto member = std::dynamic_pointer_cast<ast::MemberExpressionNode>(expr);
		ASSERT_NE(member, nullptr);
		EXPECT_EQ(member->getKind(), ast::MemberExpressionNode::Kind::DOT);
		EXPECT_EQ(member->getMember(), "x");

		auto object = std::dynamic_pointer_cast<ast::IdentifierNode>(member->getObject());
		ASSERT_NE(object, nullptr);
		EXPECT_EQ(object->getName(), "point");
	}

	// Test arrow operator
	{
		auto expr = parseExpression("ptr->x");
		auto member = std::dynamic_pointer_cast<ast::MemberExpressionNode>(expr);
		ASSERT_NE(member, nullptr);
		EXPECT_EQ(member->getKind(), ast::MemberExpressionNode::Kind::ARROW);
		EXPECT_EQ(member->getMember(), "x");

		auto object = std::dynamic_pointer_cast<ast::IdentifierNode>(member->getObject());
		ASSERT_NE(object, nullptr);
		EXPECT_EQ(object->getName(), "ptr");
	}

	// Test chained member access
	{
		auto expr = parseExpression("obj.inner.value");
		auto member1 = std::dynamic_pointer_cast<ast::MemberExpressionNode>(expr);
		ASSERT_NE(member1, nullptr);
		EXPECT_EQ(member1->getKind(), ast::MemberExpressionNode::Kind::DOT);
		EXPECT_EQ(member1->getMember(), "value");

		auto member2 = std::dynamic_pointer_cast<ast::MemberExpressionNode>(member1->getObject());
		ASSERT_NE(member2, nullptr);
		EXPECT_EQ(member2->getKind(), ast::MemberExpressionNode::Kind::DOT);
		EXPECT_EQ(member2->getMember(), "inner");

		auto object = std::dynamic_pointer_cast<ast::IdentifierNode>(member2->getObject());
		ASSERT_NE(object, nullptr);
		EXPECT_EQ(object->getName(), "obj");
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
		auto expr = parseExpression("arr[0]");
		auto index = std::dynamic_pointer_cast<ast::IndexExpressionNode>(expr);
		ASSERT_NE(index, nullptr);

		auto array = std::dynamic_pointer_cast<ast::IdentifierNode>(index->getArray());
		ASSERT_NE(array, nullptr);
		EXPECT_EQ(array->getName(), "arr");

		auto indexExpr = std::dynamic_pointer_cast<ast::LiteralNode>(index->getIndex());
		ASSERT_NE(indexExpr, nullptr);
		EXPECT_EQ(indexExpr->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(indexExpr->getValue(), "0");
	}

	// Test indexing with expression
	{
		auto expr = parseExpression("arr[i + 1]");
		auto index = std::dynamic_pointer_cast<ast::IndexExpressionNode>(expr);
		ASSERT_NE(index, nullptr);

		auto array = std::dynamic_pointer_cast<ast::IdentifierNode>(index->getArray());
		ASSERT_NE(array, nullptr);
		EXPECT_EQ(array->getName(), "arr");

		auto indexExpr = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(index->getIndex());
		ASSERT_NE(indexExpr, nullptr);
		EXPECT_EQ(indexExpr->getOperator(), ast::BinaryExpressionNode::Operator::ADD);
	}

	// Test multi-dimensional indexing
	{
		auto expr = parseExpression("matrix[i][j]");
		auto index1 = std::dynamic_pointer_cast<ast::IndexExpressionNode>(expr);
		ASSERT_NE(index1, nullptr);

		auto index2 = std::dynamic_pointer_cast<ast::IndexExpressionNode>(index1->getArray());
		ASSERT_NE(index2, nullptr);

		auto array = std::dynamic_pointer_cast<ast::IdentifierNode>(index2->getArray());
		ASSERT_NE(array, nullptr);
		EXPECT_EQ(array->getName(), "matrix");
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
		auto expr = parseExpression("foo()");
		auto call = std::dynamic_pointer_cast<ast::CallExpressionNode>(expr);
		ASSERT_NE(call, nullptr);

		auto callee = std::dynamic_pointer_cast<ast::IdentifierNode>(call->getCallee());
		ASSERT_NE(callee, nullptr);
		EXPECT_EQ(callee->getName(), "foo");

		EXPECT_EQ(call->getArguments().size(), 0);
	}

	// Test function call with arguments
	{
		auto expr = parseExpression("bar(1, 2)");
		auto call = std::dynamic_pointer_cast<ast::CallExpressionNode>(expr);
		ASSERT_NE(call, nullptr);

		auto callee = std::dynamic_pointer_cast<ast::IdentifierNode>(call->getCallee());
		ASSERT_NE(callee, nullptr);
		EXPECT_EQ(callee->getName(), "bar");

		ASSERT_EQ(call->getArguments().size(), 2);

		auto arg1 = std::dynamic_pointer_cast<ast::LiteralNode>(call->getArguments()[0]);
		ASSERT_NE(arg1, nullptr);
		EXPECT_EQ(arg1->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(arg1->getValue(), "1");

		auto arg2 = std::dynamic_pointer_cast<ast::LiteralNode>(call->getArguments()[1]);
		ASSERT_NE(arg2, nullptr);
		EXPECT_EQ(arg2->getKind(), ast::LiteralNode::Kind::INTEGER);
		EXPECT_EQ(arg2->getValue(), "2");
	}

	// Test function call with expressions as arguments
	{
		auto expr = parseExpression("compute(a + b, c * d)");
		auto call = std::dynamic_pointer_cast<ast::CallExpressionNode>(expr);
		ASSERT_NE(call, nullptr);

		auto callee = std::dynamic_pointer_cast<ast::IdentifierNode>(call->getCallee());
		ASSERT_NE(callee, nullptr);
		EXPECT_EQ(callee->getName(), "compute");

		ASSERT_EQ(call->getArguments().size(), 2);

		auto arg1 = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(call->getArguments()[0]);
		ASSERT_NE(arg1, nullptr);
		EXPECT_EQ(arg1->getOperator(), ast::BinaryExpressionNode::Operator::ADD);

		auto arg2 = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(call->getArguments()[1]);
		ASSERT_NE(arg2, nullptr);
		EXPECT_EQ(arg2->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);
	}

	// Test nested function calls
	{
		auto expr = parseExpression("outer(inner(x))");
		auto outerCall = std::dynamic_pointer_cast<ast::CallExpressionNode>(expr);
		ASSERT_NE(outerCall, nullptr);

		auto outerCallee = std::dynamic_pointer_cast<ast::IdentifierNode>(outerCall->getCallee());
		ASSERT_NE(outerCallee, nullptr);
		EXPECT_EQ(outerCallee->getName(), "outer");

		ASSERT_EQ(outerCall->getArguments().size(), 1);

		auto innerCall = std::dynamic_pointer_cast<ast::CallExpressionNode>(outerCall->getArguments()[0]);
		ASSERT_NE(innerCall, nullptr);

		auto innerCallee = std::dynamic_pointer_cast<ast::IdentifierNode>(innerCall->getCallee());
		ASSERT_NE(innerCallee, nullptr);
		EXPECT_EQ(innerCallee->getName(), "inner");
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
		auto expr = parseExpression("cast<int>(3.14)");
		auto cast = std::dynamic_pointer_cast<ast::CastExpressionNode>(expr);
		ASSERT_NE(cast, nullptr);

		auto targetType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(cast->getTargetType());
		ASSERT_NE(targetType, nullptr);
		EXPECT_EQ(targetType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		auto expression = std::dynamic_pointer_cast<ast::LiteralNode>(cast->getExpression());
		ASSERT_NE(expression, nullptr);
		EXPECT_EQ(expression->getKind(), ast::LiteralNode::Kind::DOUBLE);
	}

	// Test cast to pointer
	{
		auto expr = parseExpression("cast<void*>(ptr)");
		auto cast = std::dynamic_pointer_cast<ast::CastExpressionNode>(expr);
		ASSERT_NE(cast, nullptr);

		auto targetType = std::dynamic_pointer_cast<ast::PointerTypeNode>(cast->getTargetType());
		ASSERT_NE(targetType, nullptr);

		auto baseType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(targetType->getBaseType());
		ASSERT_NE(baseType, nullptr);
		EXPECT_EQ(baseType->getKind(), ast::PrimitiveTypeNode::Kind::VOID);

		auto expression = std::dynamic_pointer_cast<ast::IdentifierNode>(cast->getExpression());
		ASSERT_NE(expression, nullptr);
		EXPECT_EQ(expression->getName(), "ptr");
	}

	// Test cast with complex expression
	{
		auto expr = parseExpression("cast<int>(a + b)");
		auto cast = std::dynamic_pointer_cast<ast::CastExpressionNode>(expr);
		ASSERT_NE(cast, nullptr);

		auto targetType = std::dynamic_pointer_cast<ast::PrimitiveTypeNode>(cast->getTargetType());
		ASSERT_NE(targetType, nullptr);
		EXPECT_EQ(targetType->getKind(), ast::PrimitiveTypeNode::Kind::INT);

		auto expression = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(cast->getExpression());
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
	auto expr = parseExpression("a = 1, b = 2, c = 3");
	auto comma = std::dynamic_pointer_cast<ast::CommaExpressionNode>(expr);
	ASSERT_NE(comma, nullptr);
	EXPECT_EQ(comma->getExpressions().size(), 3);

	for (size_t i = 0; i < 3; ++i) {
		auto assignExpr = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(comma->getExpressions()[i]);
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
		auto expr = parseExpression("getData()->items[getIndex()].value");

		// Should be a member access with DOT operator
		auto member = std::dynamic_pointer_cast<ast::MemberExpressionNode>(expr);
		ASSERT_NE(member, nullptr);
		EXPECT_EQ(member->getKind(), ast::MemberExpressionNode::Kind::DOT);
		EXPECT_EQ(member->getMember(), "value");

		// Object should be an array indexing
		auto index = std::dynamic_pointer_cast<ast::IndexExpressionNode>(member->getObject());
		ASSERT_NE(index, nullptr);

		// Index expression should be a function call
		auto indexExpr = std::dynamic_pointer_cast<ast::CallExpressionNode>(index->getIndex());
		ASSERT_NE(indexExpr, nullptr);

		auto indexCallee = std::dynamic_pointer_cast<ast::IdentifierNode>(indexExpr->getCallee());
		ASSERT_NE(indexCallee, nullptr);
		EXPECT_EQ(indexCallee->getName(), "getIndex");

		// Array should be a member access with ARROW operator
		auto innerMember = std::dynamic_pointer_cast<ast::MemberExpressionNode>(index->getArray());
		ASSERT_NE(innerMember, nullptr);
		EXPECT_EQ(innerMember->getKind(), ast::MemberExpressionNode::Kind::ARROW);
		EXPECT_EQ(innerMember->getMember(), "items");

		// Object of arrow operator should be a function call
		auto call = std::dynamic_pointer_cast<ast::CallExpressionNode>(innerMember->getObject());
		ASSERT_NE(call, nullptr);

		auto callee = std::dynamic_pointer_cast<ast::IdentifierNode>(call->getCallee());
		ASSERT_NE(callee, nullptr);
		EXPECT_EQ(callee->getName(), "getData");
	}

	// Test complex expression with arithmetic, logical, and comparison operators
	{
		auto expr = parseExpression("(a + b) * c > d && e || f == g");

		// Should be a logical OR expression
		auto logicalOr = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(expr);
		ASSERT_NE(logicalOr, nullptr);
		EXPECT_EQ(logicalOr->getOperator(), ast::BinaryExpressionNode::Operator::LOGICAL_OR);

		// Left side should be a logical AND expression
		auto logicalAnd = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(logicalOr->getLeft());
		ASSERT_NE(logicalAnd, nullptr);
		EXPECT_EQ(logicalAnd->getOperator(), ast::BinaryExpressionNode::Operator::LOGICAL_AND);

		// Right side should be an equality comparison
		auto equality = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(logicalOr->getRight());
		ASSERT_NE(equality, nullptr);
		EXPECT_EQ(equality->getOperator(), ast::BinaryExpressionNode::Operator::EQUAL);

		// Left side of logical AND should be a greater-than comparison
		auto greaterThan = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(logicalAnd->getLeft());
		ASSERT_NE(greaterThan, nullptr);
		EXPECT_EQ(greaterThan->getOperator(), ast::BinaryExpressionNode::Operator::GREATER);

		// Left side of greater-than should be a multiplication
		auto multiply = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(greaterThan->getLeft());
		ASSERT_NE(multiply, nullptr);
		EXPECT_EQ(multiply->getOperator(), ast::BinaryExpressionNode::Operator::MULTIPLY);

		// Left side of multiplication should be an addition in parentheses
		auto addition = std::dynamic_pointer_cast<ast::BinaryExpressionNode>(multiply->getLeft());
		ASSERT_NE(addition, nullptr);
		EXPECT_EQ(addition->getOperator(), ast::BinaryExpressionNode::Operator::ADD);
	}
}