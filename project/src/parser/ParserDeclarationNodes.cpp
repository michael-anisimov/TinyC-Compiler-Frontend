#include "tinyc/parser/Parser.h"


namespace tinyc::parser {

	// Declaration parsing methods

	ast::ASTNodePtr Parser::parseNotVoidFunctionOrVariable(
			ast::ASTNodePtr type,
			const std::string &identifier,
			const lexer::SourceLocation &location) {
		// Rule 7: NOT_VOID_FUNCTION_OR_VARIABLE -> VARIABLE_TAIL
		// Rule 8: NOT_VOID_FUNCTION_OR_VARIABLE -> FUNCTION_DECLARATION_TAIL
		switch (currentToken->getType()) {
			case lexer::TokenType::LBRACKET:
			case lexer::TokenType::OP_ASSIGN:
			case lexer::TokenType::COMMA:
			case lexer::TokenType::SEMICOLON:
				return parseVariableTail(std::move(type), identifier, location);

			case lexer::TokenType::LPAREN:
				return parseFunctionDeclarationTail(std::move(type), identifier, location);

			default:
				error("Expected variable or function declaration");
		}
	}

	ast::ASTNodePtr Parser::parseVoidDeclTail(const lexer::SourceLocation &voidLocation) {
		// Rule 9: VOID_DECL_TAIL -> identifier FUNCTION_DECLARATION_TAIL
		// Rule 10: VOID_DECL_TAIL -> STAR_PLUS identifier FUNC_OR_VAR_TAIL

		// Create void type
		ast::ASTNodePtr voidType = std::make_unique<ast::PrimitiveTypeNode>(
				ast::PrimitiveTypeNode::Kind::VOID,
				voidLocation);

		if (check(lexer::TokenType::IDENTIFIER)) {
			// Rule 9: identifier FUNCTION_DECLARATION_TAIL
			auto identifierToken = consume();
			std::string identifier = identifierToken->getLexeme();

			// Only functions can have a void return type directly
			return parseFunctionDeclarationTail(std::move(voidType), identifier, identifierToken->getLocation());
		} else if (check(lexer::TokenType::OP_MULTIPLY)) {
			// Rule 10: STAR_PLUS identifier FUNC_OR_VAR_TAIL
			parseStarPlus(voidType);

			auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected identifier after void*");
			std::string identifier = identifierToken->getLexeme();

			return parseFuncOrVarTail(std::move(voidType), identifier, identifierToken->getLocation());
		} else {
			error("Expected identifier or '*' after 'void'");
		}
	}

	ast::ASTNodePtr Parser::parseFuncOrVarTail(
			ast::ASTNodePtr type,
			const std::string &identifier,
			const lexer::SourceLocation &location) {
		// Rule 11: FUNC_OR_VAR_TAIL -> VARIABLE_TAIL
		// Rule 12: FUNC_OR_VAR_TAIL -> FUNCTION_DECLARATION_TAIL
		switch (currentToken->getType()) {
			case lexer::TokenType::LBRACKET:
			case lexer::TokenType::OP_ASSIGN:
			case lexer::TokenType::COMMA:
			case lexer::TokenType::SEMICOLON:
				return parseVariableTail(std::move(type), identifier, location);

			case lexer::TokenType::LPAREN:
				return parseFunctionDeclarationTail(std::move(type), identifier, location);

			default:
				error("Expected variable or function declaration");
		}
	}

	ast::ASTNodePtr Parser::parseVariableTail(
			ast::ASTNodePtr type,
			const std::string &identifier,
			const lexer::SourceLocation &location) {
		// Rule 13: VARIABLE_TAIL -> OPT_ARRAY_SIZE OPT_INIT VAR_DECLS_TAIL ;

		// Parse optional array size
		ast::ASTNodePtr arraySize = parseOptArraySize();

		// Parse optional initializer
		ast::ASTNodePtr initializer = parseOptInit();

		// Create variable declaration node
		auto varDecl = std::make_unique<ast::VariableNode>(
				identifier,
				std::move(type),
				location,
				std::move(arraySize),
				std::move(initializer));

		// If there are more variables, collect them all
		std::vector<ast::ASTNodePtr> declarations;
		declarations.push_back(std::move(varDecl));

		declarations = parseVarDeclsTail(std::move(declarations));
		expect(lexer::TokenType::SEMICOLON, "Expected ';' after variable declaration");

		// If only one variable, return it directly
		if (declarations.size() == 1) {
			return std::move(declarations[0]);
		}

		return std::make_unique<ast::MultipleDeclarationNode>(
				std::move(declarations),
				location);
	}

	ast::ASTNodePtr Parser::parseFunctionDeclarationTail(
			ast::ASTNodePtr returnType,
			const std::string &identifier,
			const lexer::SourceLocation &location) {
		// Rule 14: FUNCTION_DECLARATION_TAIL -> ( OPT_FUN_ARGS ) FUNC_TAIL
		expect(lexer::TokenType::LPAREN, "Expected '(' after function name");

		// Parse optional function arguments
		std::vector<ast::ASTNodePtr> parameters = parseOptFunArgs();

		expect(lexer::TokenType::RPAREN, "Expected ')' after function parameters");

		// Parse function body or semicolon
		ast::ASTNodePtr body = parseFuncTail();

		// Create function declaration node
		return std::make_unique<ast::FunctionDeclarationNode>(
				identifier,
				std::move(returnType),
				std::move(parameters),
				std::move(body),
				location);
	}

	ast::ASTNodePtr Parser::parseFuncTail() {
		// Rule 15: FUNC_TAIL -> BLOCK_STMT
		// Rule 16: FUNC_TAIL -> ;
		if (check(lexer::TokenType::LBRACE)) {
			// Function definition
			return parseBlockStmt();
		} else if (match(lexer::TokenType::SEMICOLON)) {
			// Function declaration (no body)
			return nullptr;
		} else {
			error("Expected '{' or ';' after function declaration");
		}
	}

	std::vector<ast::ASTNodePtr> Parser::parseOptFunArgs() {
		// Rule 19: OPT_FUN_ARGS -> FUN_ARG FUN_ARG_TAIL
		// Rule 20: OPT_FUN_ARGS -> ε
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_VOID:
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR:
			case lexer::TokenType::IDENTIFIER:
			{
				ast::ASTNodePtr arg = parseFunArg();
				std::vector<ast::ASTNodePtr> args;
				args.push_back(std::move(arg));
				return parseFunArgTail(std::move(args));
			}

			default:
				// Empty parameter list
				return {};
		}
	}

	std::vector<ast::ASTNodePtr> Parser::parseFunArgTail(std::vector<ast::ASTNodePtr> args) {
		// Rule 21: FUN_ARG_TAIL -> , FUN_ARG FUN_ARG_TAIL
		// Rule 22: FUN_ARG_TAIL -> ε
		if (match(lexer::TokenType::COMMA)) {
			ast::ASTNodePtr arg = parseFunArg();
			args.push_back(std::move(arg));
			return parseFunArgTail(std::move(args));
		}

		// No more parameters
		return args;
	}

	ast::ASTNodePtr Parser::parseFunArg() {
		// Rule 23: FUN_ARG -> TYPE identifier
		ast::ASTNodePtr type = parseType();

		auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected parameter identifier");
		std::string identifier = identifierToken->getLexeme();

		// Create parameter node
		return std::make_unique<ast::ParameterNode>(
				identifier,
				std::move(type),
				identifierToken->getLocation());
	}

} // namespace tinyc::parser
