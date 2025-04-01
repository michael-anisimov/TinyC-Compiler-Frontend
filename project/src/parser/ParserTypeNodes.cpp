#include "tinyc/parser/Parser.h"
#include <iostream>

namespace tinyc::parser {

	// Type parsing methods

	ast::ASTNodePtr Parser::parseType() {
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR: {
				// Rule 73: TYPE -> BASE_TYPE STAR_SEQ
				ast::ASTNodePtr baseType = parseBaseType();
				parseStarSeq(baseType);
				return baseType;
			}

			case lexer::TokenType::IDENTIFIER: {
				// Rule 74: TYPE -> TYPENAME STAR_SEQ
				ast::ASTNodePtr namedType = parseNamedType();
				parseStarSeq(namedType);
				return namedType;
			}

			case lexer::TokenType::KW_STRUCT: {
				// Handle "struct Name" as a type
				auto structToken = consume(); // Consume "struct"
				auto nameToken = expect(lexer::TokenType::IDENTIFIER, "Expected struct name after 'struct'");
				std::string name = nameToken->getLexeme();

				// Create a named type node that represents a struct type
				// We can use a special format like "struct:Name" or just use the name directly
				// For simplicity, we'll use "struct:Name" to distinguish from regular named types
				ast::ASTNodePtr structType = std::make_shared<ast::NamedTypeNode>(
						"struct:" + name,
						structToken->getLocation());

				parseStarSeq(structType);
				return structType;
			}

			case lexer::TokenType::KW_VOID: {
				// Rule 75: TYPE -> void STAR_PLUS
				auto voidToken = consume(); // Consume "void"

				// Create void type
				ast::ASTNodePtr voidType = std::make_shared<ast::PrimitiveTypeNode>(
						ast::PrimitiveTypeNode::Kind::VOID,
						voidToken->getLocation());

				// Parse star plus (at least one star)
				parseStarPlus(voidType);

				return voidType;
			}

			default:
				error("Expected type (int, double, char, void, struct, or identifier)");
		}
	}

	ast::ASTNodePtr Parser::parseNonVoidType() {
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR: {
				// Rule 76: NON_VOID_TYPE -> BASE_TYPE STAR_SEQ
				ast::ASTNodePtr baseType = parseBaseType();
				parseStarSeq(baseType);
				return baseType;
			}

			case lexer::TokenType::IDENTIFIER: {
				// Rule 77: NON_VOID_TYPE -> TYPENAME STAR_SEQ
				auto identifierToken = consume();
				std::string name = identifierToken->getLexeme();

				ast::ASTNodePtr namedType = std::make_shared<ast::NamedTypeNode>(
						name,
						identifierToken->getLocation());

				parseStarSeq(namedType);
				return namedType;
			}

			default:
				error("Expected non-void type (int, double, char, or identifier)");
		}
	}


	ast::ASTNodePtr Parser::parseNamedType() {
		lexer::TokenPtr token = currentToken;

		if (token->getType() != lexer::TokenType::IDENTIFIER) {
			error("Expected identifier for named type");
		}
		auto identifierToken = consume();
		std::string identifier = identifierToken->getLexeme();

		ast::ASTNodePtr namedType = std::make_shared<ast::NamedTypeNode>(
				identifier,
				identifierToken->getLocation());

		return namedType;
	}

	ast::ASTNodePtr Parser::parseBaseType() {
		lexer::TokenPtr token = currentToken;

		switch (token->getType()) {
			case lexer::TokenType::KW_INT:
				// Rule 78: BASE_TYPE -> int
				consume();
				return std::make_shared<ast::PrimitiveTypeNode>(
						ast::PrimitiveTypeNode::Kind::INT,
						token->getLocation());

			case lexer::TokenType::KW_DOUBLE:
				// Rule 79: BASE_TYPE -> double
				consume();
				return std::make_shared<ast::PrimitiveTypeNode>(
						ast::PrimitiveTypeNode::Kind::DOUBLE,
						token->getLocation());

			case lexer::TokenType::KW_CHAR:
				// Rule 80: BASE_TYPE -> char
				consume();
				return std::make_shared<ast::PrimitiveTypeNode>(
						ast::PrimitiveTypeNode::Kind::CHAR,
						token->getLocation());

			default:
				error("Expected base type (int, double, char)");
		}
	}

	ast::ASTNodePtr Parser::parseTypeFunRet() {
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_VOID:
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR: {
				// Rule 81: TYPE_FUN_RET -> FUN_RET_TYPES STAR_SEQ
				ast::ASTNodePtr baseType = parseFunRetTypes();
				parseStarSeq(baseType);
				return baseType;
			}

			default:
				error("Expected function return type (void, int, double, char)");
		}
	}

	ast::ASTNodePtr Parser::parseFunRetTypes() {
		lexer::TokenPtr token = currentToken;

		switch (token->getType()) {
			case lexer::TokenType::KW_VOID:
				// Rule 82: FUN_RET_TYPES -> void
				consume();
				return std::make_shared<ast::PrimitiveTypeNode>(
						ast::PrimitiveTypeNode::Kind::VOID,
						token->getLocation());

			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR:
				// Rule 83: FUN_RET_TYPES -> BASE_TYPE
				return parseBaseType();

			default:
				error("Expected function return type (void, int, double, char)");
		}
	}

	void Parser::parseStarPlus(ast::ASTNodePtr &baseType) {
		// Rule 85: STAR_PLUS -> * STAR_SEQ
		expect(lexer::TokenType::OP_MULTIPLY, "Expected '*' for pointer type");

		// Create pointer type
		baseType = std::make_shared<ast::PointerTypeNode>(
				baseType,
				currentToken->getLocation());

		// Parse additional stars if any
		parseStarSeq(baseType);
	}

	void Parser::parseStarSeq(ast::ASTNodePtr &baseType) {
		// Rule 86: STAR_SEQ -> * STAR_SEQ
		// Rule 87: STAR_SEQ -> ε
		while (match(lexer::TokenType::OP_MULTIPLY)) {
			// Create pointer type for each star
			baseType = std::make_shared<ast::PointerTypeNode>(
					baseType,
					currentToken->getLocation());
		}

		// If no stars, we're done (Rule 87)
	}

	ast::ASTNodePtr Parser::parseStructDecl() {
		// Rule 88: STRUCT_DECL -> struct identifier [ '{' { TYPE identifier ';' } '}' ] ';'
		auto structToken = expect(lexer::TokenType::KW_STRUCT, "Expected 'struct'");
		auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected struct name");
		std::string name = identifierToken->getLexeme();

		// Parse optional struct body
		std::vector <ast::ASTNodePtr> fields;

		if (match(lexer::TokenType::LBRACE)) {
			// Parse struct fields
			while (!check(lexer::TokenType::RBRACE)) {
				// Parse field type
				ast::ASTNodePtr type = parseType();

				// Parse field name
				auto fieldNameToken = expect(lexer::TokenType::IDENTIFIER, "Expected field name");
				std::string fieldIdentifier = fieldNameToken->getLexeme();

				// Create variable declaration node for the field
				auto field = std::make_shared<ast::VariableNode>(
						fieldIdentifier,
						type,
						fieldNameToken->getLocation(),
						nullptr, // No array size
						nullptr); // No initializer

				fields.push_back(field);

				expect(lexer::TokenType::SEMICOLON, "Expected ';' after struct field");
			}

			expect(lexer::TokenType::RBRACE, "Expected '}' after struct fields");
		}

		expect(lexer::TokenType::SEMICOLON, "Expected ';' after struct declaration");

		// Create struct declaration node
		return std::make_shared<ast::StructDeclarationNode>(
				name,
				fields,
				structToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseFunPtrDecl() {
		// Rule 94: FUNPTR_DECL -> typedef TYPE_FUN_RET ( * identifier ) ( OPT_FUNPTR_ARGS ) ;
		auto typedefToken = expect(lexer::TokenType::KW_TYPEDEF, "Expected 'typedef'");
		ast::ASTNodePtr returnType = parseTypeFunRet();

		expect(lexer::TokenType::LPAREN, "Expected '(' after return type");
		expect(lexer::TokenType::OP_MULTIPLY, "Expected '*' for function pointer");

		auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected function pointer name");
		std::string name = identifierToken->getLexeme();

		expect(lexer::TokenType::RPAREN, "Expected ')' after function pointer name");
		expect(lexer::TokenType::LPAREN, "Expected '(' for parameter list");

		// Parse optional parameter types
		std::vector<ast::ASTNodePtr> parameterTypes = parseOptFunPtrArgs();

		expect(lexer::TokenType::RPAREN, "Expected ')' after parameter list");
		expect(lexer::TokenType::SEMICOLON, "Expected ';' after function pointer declaration");

		// Create function pointer declaration node
		return std::make_shared<ast::FunctionPointerDeclarationNode>(
				returnType,
				name,
				parameterTypes,
				typedefToken->getLocation());
	}

	std::vector<ast::ASTNodePtr> Parser::parseOptFunPtrArgs() {
		// Rule 95: OPT_FUNPTR_ARGS -> FUNPTR_ARGS
		// Rule 96: OPT_FUNPTR_ARGS -> ε
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_VOID:
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR:
				return parseFunPtrArgs();

			default:
				// Empty parameter list
				return std::vector<ast::ASTNodePtr>();
		}
	}

	std::vector<ast::ASTNodePtr> Parser::parseFunPtrArgs() {
		// Rule 97: FUNPTR_ARGS -> TYPE FUNPTR_ARGS_TAIL
		ast::ASTNodePtr type = parseType();

		std::vector<ast::ASTNodePtr> types = {type};
		return parseFunPtrArgsTail(types);
	}

	std::vector<ast::ASTNodePtr> Parser::parseFunPtrArgsTail(std::vector<ast::ASTNodePtr> types) {
		// Rule 98: FUNPTR_ARGS_TAIL -> , TYPE FUNPTR_ARGS_TAIL
		// Rule 99: FUNPTR_ARGS_TAIL -> ε
		if (match(lexer::TokenType::COMMA)) {
			ast::ASTNodePtr type = parseType();
			types.push_back(type);
			return parseFunPtrArgsTail(types);
		}

		// No more parameters
		return types;
	}

} // namespace tinyc::parser
