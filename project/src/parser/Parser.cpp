#include "tinyc/parser/Parser.h"
#include <sstream>
#include <iostream>

namespace tinyc::parser {

	Parser::Parser(lexer::Lexer &lexer) : lexer(lexer) {
		// Initialize by reading the first token
		currentToken = lexer.nextToken();
	}

	lexer::TokenPtr Parser::consume() {
		lexer::TokenPtr oldToken = currentToken;
		currentToken = lexer.nextToken();

		return oldToken;
	}

	bool Parser::check(lexer::TokenType type) const {
		return currentToken->getType() == type;
	}

	bool Parser::match(lexer::TokenType type) {
		if (check(type)) {
			consume();
			return true;
		}
		return false;
	}

	lexer::TokenPtr Parser::expect(lexer::TokenType type, const std::string &message) {
		if (check(type)) {
			return consume();
		}
		error(message);
	}

	void Parser::error(const std::string &message) const {
		throw ParserError(message, currentToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseProgram() {
		auto program = std::make_shared<ast::ProgramNode>(lexer.getSourceName());

		// Parse declarations until EOF
		while (!check(lexer::TokenType::END_OF_FILE)) {
			auto item = parseProgramItem();
			program->addDeclaration(item);
		}

		return program;
	}

	ast::ASTNodePtr Parser::parseProgramItem() {
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR: {
				// Rule 3: PROGRAM_ITEM -> NON_VOID_TYPE identifier NOT_VOID_FUNCTION_OR_VARIABLE
				ast::ASTNodePtr type = parseNonVoidType();

				auto identifierToken = expect(lexer::TokenType::IDENTIFIER,
											  "Expected identifier after type");
				std::string name = identifierToken->getLexeme();

				return parseNotVoidFunctionOrVariable(type, name, identifierToken->getLocation());
			}

			case lexer::TokenType::KW_VOID: {
				// Rule 4: PROGRAM_ITEM -> void VOID_DECL_TAIL
				// Save location before consuming
				auto voidLocation = currentToken->getLocation();
				auto voidToken = consume(); // Consume "void"
				return parseVoidDeclTail(voidLocation);
			}

			case lexer::TokenType::KW_STRUCT: {
				// Rule 5: PROGRAM_ITEM -> STRUCT_DECL
				return parseStructDecl();
			}

			case lexer::TokenType::KW_TYPEDEF: {
				// Rule 6: PROGRAM_ITEM -> FUNPTR_DECL
				return parseFunPtrDecl();
			}

			default:
				error("Expected type, struct, or typedef");
		}
	}

} // namespace tinyc::parser
