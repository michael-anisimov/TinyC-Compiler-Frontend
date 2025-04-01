#include "tinyc/parser/Parser.h"


namespace tinyc::parser {

	// Expression parsing methods

	ast::ASTNodePtr Parser::parseExprs() {
		// Rule 70: EXPRS -> EXPR EXPRS_TAIL
		ast::ASTNodePtr expr = parseExpr();

		// Parse comma-separated expressions if any
		std::vector<ast::ASTNodePtr> expressions;
		expressions.push_back(std::move(expr));
		expressions = parseExprsTail(std::move(expressions));

		// If only one expression, return it directly
		if (expressions.size() == 1) {
			return std::move(expressions[0]);
		}

		// Create comma expression node
		return std::make_unique<ast::CommaExpressionNode>(
				std::move(expressions),
				expressions[0]->getLocation());
	}

	std::vector<ast::ASTNodePtr> Parser::parseExprsTail(std::vector<ast::ASTNodePtr> expressions) {
		// Rule 71: EXPRS_TAIL -> , EXPR EXPRS_TAIL
		// Rule 72: EXPRS_TAIL -> ε
		if (match(lexer::TokenType::COMMA)) {
			ast::ASTNodePtr expr = parseExpr();
			expressions.push_back(std::move(expr));
			return parseExprsTail(std::move(expressions));
		}

		// No more expressions
		return expressions;
	}

	ast::ASTNodePtr Parser::parseExpr() {
		// Rule 100: EXPR -> E9 EXPR_TAIL
		ast::ASTNodePtr left = parseE9();
		return parseExprTail(std::move(left));
	}

	ast::ASTNodePtr Parser::parseExprTail(ast::ASTNodePtr left) {
		// Rule 101: EXPR_TAIL -> = EXPR
		// Rule 102: EXPR_TAIL -> ε
		if (match(lexer::TokenType::OP_ASSIGN)) {
			ast::ASTNodePtr right = parseExpr();

			// Create assignment expression
			return std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::ASSIGN,
					std::move(left),
					std::move(right),
					left->getLocation());
		}

		// No assignment
		return left;
	}

	ast::ASTNodePtr Parser::parseE9() {
		// Rule 103: E9 -> E8 E9_Prime
		ast::ASTNodePtr left = parseE8();
		return parseE9Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE9Prime(ast::ASTNodePtr left) {
		// Rule 104: E9_Prime -> || E8 E9_Prime
		// Rule 105: E9_Prime -> ε
		if (match(lexer::TokenType::OP_LOGICAL_OR)) {
			ast::ASTNodePtr right = parseE8();

			// Create logical OR expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::LOGICAL_OR,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE9Prime(std::move(newLeft));
		}

		// No logical OR
		return left;
	}

	ast::ASTNodePtr Parser::parseE8() {
		// Rule 106: E8 -> E7 E8_Prime
		ast::ASTNodePtr left = parseE7();
		return parseE8Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE8Prime(ast::ASTNodePtr left) {
		// Rule 107: E8_Prime -> && E7 E8_Prime
		// Rule 108: E8_Prime -> ε
		if (match(lexer::TokenType::OP_LOGICAL_AND)) {
			ast::ASTNodePtr right = parseE7();

			// Create logical AND expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::LOGICAL_AND,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE8Prime(std::move(newLeft));
		}

		// No logical AND
		return left;
	}

	ast::ASTNodePtr Parser::parseE7() {
		// Rule 109: E7 -> E6 E7_Prime
		ast::ASTNodePtr left = parseE6();
		return parseE7Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE7Prime(ast::ASTNodePtr left) {
		// Rule 110: E7_Prime -> | E6 E7_Prime
		// Rule 111: E7_Prime -> ε
		if (match(lexer::TokenType::OP_OR)) {
			ast::ASTNodePtr right = parseE6();

			// Create bitwise OR expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::BITWISE_OR,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE7Prime(std::move(newLeft));
		}

		// No bitwise OR
		return left;
	}

	ast::ASTNodePtr Parser::parseE6() {
		// Rule 112: E6 -> E5 E6_Prime
		ast::ASTNodePtr left = parseE5();
		return parseE6Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE6Prime(ast::ASTNodePtr left) {
		// Rule 113: E6_Prime -> & E5 E6_Prime
		// Rule 114: E6_Prime -> ε
		if (match(lexer::TokenType::OP_AND)) {
			ast::ASTNodePtr right = parseE5();

			// Create bitwise AND expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::BITWISE_AND,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE6Prime(std::move(newLeft));
		}

		// No bitwise AND
		return left;
	}

	ast::ASTNodePtr Parser::parseE5() {
		// Rule 115: E5 -> E4 E5_Prime
		ast::ASTNodePtr left = parseE4();
		return parseE5Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE5Prime(ast::ASTNodePtr left) {
		// Rule 116: E5_Prime -> == E4 E5_Prime
		// Rule 117: E5_Prime -> != E4 E5_Prime
		// Rule 118: E5_Prime -> ε
		if (match(lexer::TokenType::OP_EQUAL)) {
			ast::ASTNodePtr right = parseE4();

			// Create equality expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::EQUAL,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE5Prime(std::move(newLeft));
		} else if (match(lexer::TokenType::OP_NOT_EQUAL)) {
			ast::ASTNodePtr right = parseE4();

			// Create inequality expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::NOT_EQUAL,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE5Prime(std::move(newLeft));
		}

		// No equality/inequality
		return left;
	}

	ast::ASTNodePtr Parser::parseE4() {
		// Rule 119: E4 -> E3 E4_Prime
		ast::ASTNodePtr left = parseE3();
		return parseE4Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE4Prime(ast::ASTNodePtr left) {
		// Rule 120: E4_Prime -> < E3 E4_Prime
		// Rule 121: E4_Prime -> <= E3 E4_Prime
		// Rule 122: E4_Prime -> > E3 E4_Prime
		// Rule 123: E4_Prime -> >= E3 E4_Prime
		// Rule 124: E4_Prime -> ε

		if (match(lexer::TokenType::OP_LESS)) {
			ast::ASTNodePtr right = parseE3();

			// Create less-than expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::LESS,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE4Prime(std::move(newLeft));
		} else if (match(lexer::TokenType::OP_LESS_EQUAL)) {
			ast::ASTNodePtr right = parseE3();

			// Create less-than-or-equal expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::LESS_EQUAL,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE4Prime(std::move(newLeft));
		} else if (match(lexer::TokenType::OP_GREATER)) {
			ast::ASTNodePtr right = parseE3();

			// Create greater-than expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::GREATER,
					std::move(left),
				  	std::move(right),
					left->getLocation());

			return parseE4Prime(std::move(newLeft));
		} else if (match(lexer::TokenType::OP_GREATER_EQUAL)) {
			ast::ASTNodePtr right = parseE3();

			// Create greater-than-or-equal expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::GREATER_EQUAL,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE4Prime(std::move(newLeft));
		}

		// No comparison
		return left;
	}

	ast::ASTNodePtr Parser::parseE3() {
		// Rule 125: E3 -> E2 E3_Prime
		ast::ASTNodePtr left = parseE2();
		return parseE3Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE3Prime(ast::ASTNodePtr left) {
		// Rule 126: E3_Prime -> << E2 E3_Prime
		// Rule 127: E3_Prime -> >> E2 E3_Prime
		// Rule 128: E3_Prime -> ε

		if (match(lexer::TokenType::OP_LEFT_SHIFT)) {
			ast::ASTNodePtr right = parseE2();

			// Create left shift expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::LEFT_SHIFT,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE3Prime(std::move(newLeft));
		} else if (match(lexer::TokenType::OP_RIGHT_SHIFT)) {
			ast::ASTNodePtr right = parseE2();

			// Create right shift expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::RIGHT_SHIFT,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE3Prime(std::move(newLeft));
		}

		// No shift
		return left;
	}

	ast::ASTNodePtr Parser::parseE2() {
		// Rule 129: E2 -> E1 E2_Prime
		ast::ASTNodePtr left = parseE1();
		return parseE2Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE2Prime(ast::ASTNodePtr left) {
		// Rule 130: E2_Prime -> + E1 E2_Prime
		// Rule 131: E2_Prime -> - E1 E2_Prime
		// Rule 132: E2_Prime -> ε

		if (match(lexer::TokenType::OP_PLUS)) {
			ast::ASTNodePtr right = parseE1();

			// Create addition expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::ADD,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE2Prime(std::move(newLeft));
		} else if (match(lexer::TokenType::OP_MINUS)) {
			ast::ASTNodePtr right = parseE1();

			// Create subtraction expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::SUBTRACT,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE2Prime(std::move(newLeft));
		}

		// No addition/subtraction
		return left;
	}

	ast::ASTNodePtr Parser::parseE1() {
		// Rule 133: E1 -> E_UNARY_PRE E1_Prime
		ast::ASTNodePtr left = parseEUnaryPre();
		return parseE1Prime(std::move(left));
	}

	ast::ASTNodePtr Parser::parseE1Prime(ast::ASTNodePtr left) {
		// Rule 134: E1_Prime -> * E_UNARY_PRE E1_Prime
		// Rule 135: E1_Prime -> / E_UNARY_PRE E1_Prime
		// Rule 136: E1_Prime -> % E_UNARY_PRE E1_Prime
		// Rule 137: E1_Prime -> ε

		if (match(lexer::TokenType::OP_MULTIPLY)) {
			ast::ASTNodePtr right = parseEUnaryPre();

			// Create multiplication expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::MULTIPLY,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE1Prime(std::move(newLeft));
		} else if (match(lexer::TokenType::OP_DIVIDE)) {
			ast::ASTNodePtr right = parseEUnaryPre();

			// Create division expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::DIVIDE,
					std::move(left),
					std::move(right),
					left->getLocation());

			return parseE1Prime(std::move(newLeft));
		} else if (match(lexer::TokenType::OP_MODULO)) {
			ast::ASTNodePtr right = parseEUnaryPre();

			// Create modulo expression
			ast::ASTNodePtr newLeft = std::make_unique<ast::BinaryExpressionNode>(
					ast::BinaryExpressionNode::Operator::MODULO,
					std::move(left),
				  	std::move(right),
					left->getLocation());

			return parseE1Prime(std::move(newLeft));
		}

		// No multiplication/division/modulo
		return left;
	}

	ast::ASTNodePtr Parser::parseEUnaryPre() {
		// Rules 138-146: E_UNARY_PRE -> ...
		lexer::TokenPtr token = currentToken;

		switch (token->getType()) {
			case lexer::TokenType::OP_PLUS: {
				// Rule 138: E_UNARY_PRE -> + E_UNARY_PRE
				consume();
				ast::ASTNodePtr operand = parseEUnaryPre();

				// Create unary plus expression
				return std::make_unique<ast::UnaryExpressionNode>(
						ast::UnaryExpressionNode::Operator::POSITIVE,
						std::move(operand),
						token->getLocation());
			}

			case lexer::TokenType::OP_MINUS: {
				// Rule 139: E_UNARY_PRE -> - E_UNARY_PRE
				consume();
				ast::ASTNodePtr operand = parseEUnaryPre();

				// Create unary minus expression
				return std::make_unique<ast::UnaryExpressionNode>(
						ast::UnaryExpressionNode::Operator::NEGATIVE,
						std::move(operand),
						token->getLocation());
			}

			case lexer::TokenType::OP_NOT: {
				// Rule 140: E_UNARY_PRE -> ! E_UNARY_PRE
				consume();
				ast::ASTNodePtr operand = parseEUnaryPre();

				// Create logical not expression
				return std::make_unique<ast::UnaryExpressionNode>(
						ast::UnaryExpressionNode::Operator::LOGICAL_NOT,
						std::move(operand),
						token->getLocation());
			}

			case lexer::TokenType::OP_BITWISE_NOT: {
				// Rule 141: E_UNARY_PRE -> ~ E_UNARY_PRE
				consume();
				ast::ASTNodePtr operand = parseEUnaryPre();

				// Create bitwise not expression
				return std::make_unique<ast::UnaryExpressionNode>(
						ast::UnaryExpressionNode::Operator::BITWISE_NOT,
						std::move(operand),
						token->getLocation());
			}

			case lexer::TokenType::OP_INCREMENT: {
				// Rule 142: E_UNARY_PRE -> ++ E_UNARY_PRE
				consume();
				ast::ASTNodePtr operand = parseEUnaryPre();

				// Create pre-increment expression
				return std::make_unique<ast::UnaryExpressionNode>(
						ast::UnaryExpressionNode::Operator::PRE_INCREMENT,
						std::move(operand),
						token->getLocation());
			}

			case lexer::TokenType::OP_DECREMENT: {
				// Rule 143: E_UNARY_PRE -> -- E_UNARY_PRE
				consume();
				ast::ASTNodePtr operand = parseEUnaryPre();

				// Create pre-decrement expression
				return std::make_unique<ast::UnaryExpressionNode>(
						ast::UnaryExpressionNode::Operator::PRE_DECREMENT,
						std::move(operand),
						token->getLocation());
			}

			case lexer::TokenType::OP_MULTIPLY: {
				// Rule 144: E_UNARY_PRE -> * E_UNARY_PRE
				consume();
				ast::ASTNodePtr operand = parseEUnaryPre();

				// Create dereference expression
				return std::make_unique<ast::UnaryExpressionNode>(
						ast::UnaryExpressionNode::Operator::DEREFERENCE,
						std::move(operand),
						token->getLocation());
			}

			case lexer::TokenType::OP_AND: {
				// Rule 145: E_UNARY_PRE -> & E_UNARY_PRE
				consume();
				ast::ASTNodePtr operand = parseEUnaryPre();

				// Create address-of expression
				return std::make_unique<ast::UnaryExpressionNode>(
						ast::UnaryExpressionNode::Operator::ADDRESS_OF,
						std::move(operand),
						token->getLocation());
			}

			default:
				// Rule 146: E_UNARY_PRE -> E_CALL_INDEX_MEMBER_POST
				return parseECallIndexMemberPost();
		}
	}

	ast::ASTNodePtr Parser::parseECallIndexMemberPost() {
		// Rule 147: E_CALL_INDEX_MEMBER_POST -> F E_CALL_INDEX_MEMBER_POST_Prime
		ast::ASTNodePtr expr = parseF();
		return parseECallIndexMemberPostPrime(std::move(expr));
	}

	ast::ASTNodePtr Parser::parseECallIndexMemberPostPrime(ast::ASTNodePtr expr) {
		// Rules 148-152: E_CALL_INDEX_MEMBER_POST_Prime -> ...
		switch (currentToken->getType()) {
			case lexer::TokenType::LPAREN:
				// Rule 148: E_CALL_INDEX_MEMBER_POST_Prime -> E_CALL E_CALL_INDEX_MEMBER_POST_Prime
				expr = parseECall(std::move(expr));
				return parseECallIndexMemberPostPrime(std::move(expr));

			case lexer::TokenType::LBRACKET:
				// Rule 149: E_CALL_INDEX_MEMBER_POST_Prime -> E_INDEX E_CALL_INDEX_MEMBER_POST_Prime
				expr = parseEIndex(std::move(expr));
				return parseECallIndexMemberPostPrime(std::move(expr));

			case lexer::TokenType::OP_DOT:
			case lexer::TokenType::OP_ARROW:
				// Rule 150: E_CALL_INDEX_MEMBER_POST_Prime -> E_MEMBER E_CALL_INDEX_MEMBER_POST_Prime
				expr = parseEMember(std::move(expr));
				return parseECallIndexMemberPostPrime(std::move(expr));

			case lexer::TokenType::OP_INCREMENT:
			case lexer::TokenType::OP_DECREMENT:
				// Rule 151: E_CALL_INDEX_MEMBER_POST_Prime -> E_POST E_CALL_INDEX_MEMBER_POST_Prime
				expr = parseEPost(std::move(expr));
				return parseECallIndexMemberPostPrime(std::move(expr));

			default:
				// Rule 152: E_CALL_INDEX_MEMBER_POST_Prime -> ε
				return expr;
		}
	}

	ast::ASTNodePtr Parser::parseECall(ast::ASTNodePtr callee) {
		// Rule 153: E_CALL -> ( OPT_EXPR_LIST )
		expect(lexer::TokenType::LPAREN, "Expected '(' for function call");

		// Parse optional argument list
		std::vector<ast::ASTNodePtr> arguments = parseOptExprList();

		expect(lexer::TokenType::RPAREN, "Expected ')' after function arguments");

		// Create call expression
		return std::make_unique<ast::CallExpressionNode>(
				std::move(callee),
				std::move(arguments),
				callee->getLocation());
	}

	std::vector<ast::ASTNodePtr> Parser::parseOptExprList() {
		// Rule 154: OPT_EXPR_LIST -> EXPR EXPR_TAIL_LIST
		// Rule 155: OPT_EXPR_LIST -> ε
		switch (currentToken->getType()) {
			case lexer::TokenType::OP_PLUS:
			case lexer::TokenType::OP_MINUS:
			case lexer::TokenType::OP_NOT:
			case lexer::TokenType::OP_BITWISE_NOT:
			case lexer::TokenType::OP_INCREMENT:
			case lexer::TokenType::OP_DECREMENT:
			case lexer::TokenType::OP_MULTIPLY:
			case lexer::TokenType::OP_AND:
			case lexer::TokenType::INTEGER_LITERAL:
			case lexer::TokenType::DOUBLE_LITERAL:
			case lexer::TokenType::CHAR_LITERAL:
			case lexer::TokenType::STRING_LITERAL:
			case lexer::TokenType::IDENTIFIER:
			case lexer::TokenType::LPAREN:
			case lexer::TokenType::KW_CAST: {
				ast::ASTNodePtr expr = parseExpr();
				std::vector<ast::ASTNodePtr> expressions;
				expressions.push_back(std::move(expr));
				return parseExprTailList(std::move(expressions));
			}

			default:
				// Empty argument list
				return std::vector<ast::ASTNodePtr>();
		}
	}

	std::vector<ast::ASTNodePtr> Parser::parseExprTailList(std::vector<ast::ASTNodePtr> expressions) {
		// Rule 156: EXPR_TAIL_LIST -> , EXPR EXPR_TAIL_LIST
		// Rule 157: EXPR_TAIL_LIST -> ε
		if (match(lexer::TokenType::COMMA)) {
			ast::ASTNodePtr expr = parseExpr();
			expressions.push_back(std::move(expr));
			return parseExprTailList(std::move(expressions));
		}

		// No more expressions
		return expressions;
	}

	ast::ASTNodePtr Parser::parseEIndex(ast::ASTNodePtr array) {
		// Rule 158: E_INDEX -> [ EXPR ]
		expect(lexer::TokenType::LBRACKET, "Expected '[' for array indexing");

		ast::ASTNodePtr index = parseExpr();

		expect(lexer::TokenType::RBRACKET, "Expected ']' after array index");

		// Create index expression
		return std::make_unique<ast::IndexExpressionNode>(
				std::move(array),
				std::move(index),
				array->getLocation());
	}

	ast::ASTNodePtr Parser::parseEMember(ast::ASTNodePtr object) {
		// Rule 159: E_MEMBER -> . identifier
		// Rule 160: E_MEMBER -> -> identifier

		ast::MemberExpressionNode::Kind kind;

		if (match(lexer::TokenType::OP_DOT)) {
			kind = ast::MemberExpressionNode::Kind::DOT;
		} else if (match(lexer::TokenType::OP_ARROW)) {
			kind = ast::MemberExpressionNode::Kind::ARROW;
		} else {
			error("Expected '.' or '->' for member access");
		}

		auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected member name");
		std::string memberName = identifierToken->getLexeme();

		// Create member expression
		return std::make_unique<ast::MemberExpressionNode>(
				kind,
				std::move(object),
				memberName,
				object->getLocation());
	}

	ast::ASTNodePtr Parser::parseEPost(ast::ASTNodePtr operand) {
		// Rule 161: E_POST -> ++
		// Rule 162: E_POST -> --

		ast::UnaryExpressionNode::Operator op;

		if (match(lexer::TokenType::OP_INCREMENT)) {
			op = ast::UnaryExpressionNode::Operator::POST_INCREMENT;
		} else if (match(lexer::TokenType::OP_DECREMENT)) {
			op = ast::UnaryExpressionNode::Operator::POST_DECREMENT;
		} else {
			error("Expected '++' or '--' for postfix operation");
		}

		// Create postfix expression
		return std::make_unique<ast::UnaryExpressionNode>(
				op,
				std::move(operand),
				operand->getLocation());
	}

	ast::ASTNodePtr Parser::parseF() {
		// Rules 163-169: F -> ...
		lexer::TokenPtr token = currentToken;

		switch (token->getType()) {
			case lexer::TokenType::INTEGER_LITERAL: {
				// Rule 163: F -> integer_literal
				consume();

				// Create integer literal
				return std::make_unique<ast::LiteralNode>(
						std::to_string(token->getIntValue()),
						ast::LiteralNode::Kind::INTEGER,
						token->getLocation());
			}

			case lexer::TokenType::DOUBLE_LITERAL: {
				// Rule 164: F -> double_literal
				consume();

				// Create double literal
				return std::make_unique<ast::LiteralNode>(
						std::to_string(token->getDoubleValue()),
						ast::LiteralNode::Kind::DOUBLE,
						token->getLocation());
			}

			case lexer::TokenType::CHAR_LITERAL: {
				// Rule 165: F -> char_literal
				consume();

				// Create char literal
				std::string value;
				value.push_back(token->getCharValue());

				return std::make_unique<ast::LiteralNode>(
						value,
						ast::LiteralNode::Kind::CHAR,
						token->getLocation());
			}

			case lexer::TokenType::STRING_LITERAL: {
				// Rule 166: F -> string_literal
				consume();

				// Create string literal
				return std::make_unique<ast::LiteralNode>(
						token->getLexeme(),
						ast::LiteralNode::Kind::STRING,
						token->getLocation());
			}

			case lexer::TokenType::IDENTIFIER: {
				// Rule 167: F -> identifier
				consume();

				// Create identifier
				return std::make_unique<ast::IdentifierNode>(
						token->getLexeme(),
						token->getLocation());
			}

			case lexer::TokenType::LPAREN: {
				// Rule 168: F -> ( EXPRS )
				consume();

				ast::ASTNodePtr expr = parseExprs();

				expect(lexer::TokenType::RPAREN, "Expected ')' after expression");

				return expr;
			}

			case lexer::TokenType::KW_CAST: {
				// Rule 169: F -> E_CAST
				return parseECast();
			}

			default:
				error("Expected expression");
		}
	}

	ast::ASTNodePtr Parser::parseECast() {
		// Rule 170: E_CAST -> cast < TYPE > ( EXPR )
		auto castToken = expect(lexer::TokenType::KW_CAST, "Expected 'cast'");

		expect(lexer::TokenType::OP_LESS, "Expected '<' after 'cast'");
		ast::ASTNodePtr targetType = parseType();
		expect(lexer::TokenType::OP_GREATER, "Expected '>' after cast type");

		expect(lexer::TokenType::LPAREN, "Expected '(' after cast type");
		ast::ASTNodePtr expression = parseExpr();
		expect(lexer::TokenType::RPAREN, "Expected ')' after cast expression");

		// Create cast expression
		return std::make_unique<ast::CastExpressionNode>(
				std::move(targetType),
				std::move(expression),
				castToken->getLocation());
	}

} // namespace tinyc::parser
