#include "tinyc/parser/Parser.h"


namespace tinyc::parser {

	// Declaration parsing methods

	ast::ASTNodePtr Parser::parseNotVoidFunctionOrVariable(
			ast::ASTNodePtr type,
			const std::string &name,
			const lexer::SourceLocation &location) {
		// Rule 7: NOT_VOID_FUNCTION_OR_VARIABLE -> VARIABLE_TAIL
		// Rule 8: NOT_VOID_FUNCTION_OR_VARIABLE -> FUNCTION_DECLARATION_TAIL
		switch (currentToken->getType()) {
			case lexer::TokenType::LBRACKET:
			case lexer::TokenType::OP_ASSIGN:
			case lexer::TokenType::COMMA:
			case lexer::TokenType::SEMICOLON:
				return parseVariableTail(type, name, location);

			case lexer::TokenType::LPAREN:
				return parseFunctionDeclarationTail(type, name, location);

			default:
				error("Expected variable or function declaration");
		}
	}

	ast::ASTNodePtr Parser::parseVoidDeclTail() {
		// Rule 9: VOID_DECL_TAIL -> identifier FUNCTION_DECLARATION_TAIL
		// Rule 10: VOID_DECL_TAIL -> STAR_PLUS identifier FUNC_OR_VAR_TAIL

		// Create void type
		ast::ASTNodePtr voidType = std::make_shared<ast::PrimitiveTypeNode>(
				ast::PrimitiveTypeNode::Kind::VOID,
				currentToken->getLocation());

		if (check(lexer::TokenType::IDENTIFIER)) {
			// Rule 9: identifier FUNCTION_DECLARATION_TAIL
			auto identifierToken = consume();
			std::string name = identifierToken->getLexeme();

			// Only functions can have a void return type directly
			return parseFunctionDeclarationTail(voidType, name, identifierToken->getLocation());
		} else if (check(lexer::TokenType::OP_MULTIPLY)) {
			// Rule 10: STAR_PLUS identifier FUNC_OR_VAR_TAIL
			parseStarPlus(voidType);

			auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected identifier after void*");
			std::string name = identifierToken->getLexeme();

			return parseFuncOrVarTail(voidType, name, identifierToken->getLocation());
		} else {
			error("Expected identifier or '*' after 'void'");
		}
	}

	ast::ASTNodePtr Parser::parseFuncOrVarTail(
			const ast::ASTNodePtr &type,
			const std::string &name,
			const lexer::SourceLocation &location) {
		// Rule 11: FUNC_OR_VAR_TAIL -> VARIABLE_TAIL
		// Rule 12: FUNC_OR_VAR_TAIL -> FUNCTION_DECLARATION_TAIL
		switch (currentToken->getType()) {
			case lexer::TokenType::LBRACKET:
			case lexer::TokenType::OP_ASSIGN:
			case lexer::TokenType::COMMA:
			case lexer::TokenType::SEMICOLON:
				return parseVariableTail(type, name, location);

			case lexer::TokenType::LPAREN:
				return parseFunctionDeclarationTail(type, name, location);

			default:
				error("Expected variable or function declaration");
		}
	}

	ast::ASTNodePtr Parser::parseVariableTail(
			const ast::ASTNodePtr &type,
			const std::string &name,
			const lexer::SourceLocation &location) {
		// Rule 13: VARIABLE_TAIL -> OPT_ARRAY_SIZE OPT_INIT MORE_GLOBAL_VARS ;

		// Parse optional array size
		ast::ASTNodePtr arraySize = parseOptArraySize();

		// Parse optional initializer
		ast::ASTNodePtr initializer = parseOptInit();

		// Create variable declaration node
		auto varDecl = std::make_shared<ast::VariableDeclarationNode>(
				type,
				name,
				arraySize,
				initializer,
				location);

		// If there are more variables, collect them all
		std::vector<ast::ASTNodePtr> declarations = {varDecl};
		parseMoreGlobalVars(declarations, type);

		expect(lexer::TokenType::SEMICOLON, "Expected ';' after variable declaration");

		// If only one variable, return it directly
		if (declarations.size() == 1) {
			return declarations[0];
		}

		// Otherwise, return a list of declarations
		// We could create a special node for this, but for simplicity
		// we'll just return the first one for now
		return declarations[0];
	}

	ast::ASTNodePtr Parser::parseFunctionDeclarationTail(
			const ast::ASTNodePtr &returnType,
			const std::string &name,
			const lexer::SourceLocation &location) {
		// Rule 14: FUNCTION_DECLARATION_TAIL -> ( OPT_FUN_ARGS ) FUNC_TAIL
		expect(lexer::TokenType::LPAREN, "Expected '(' after function name");

		// Parse optional function arguments
		std::vector<ast::ASTNodePtr> parameters = parseOptFunArgs();

		expect(lexer::TokenType::RPAREN, "Expected ')' after function parameters");

		// Parse function body or semicolon
		ast::ASTNodePtr body = parseFuncTail();

		// Create function declaration node
		return std::make_shared<ast::FunctionDeclarationNode>(
				returnType,
				name,
				parameters,
				body,
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

	void Parser::parseMoreGlobalVars(
			std::vector<ast::ASTNodePtr> &declarations,
			const ast::ASTNodePtr &type) {
		// Rule 17: MORE_GLOBAL_VARS -> , identifier OPT_ARRAY_SIZE OPT_INIT MORE_GLOBAL_VARS
		// Rule 18: MORE_GLOBAL_VARS -> ε
		if (match(lexer::TokenType::COMMA)) {
			auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected variable name");
			std::string name = identifierToken->getLexeme();

			// Parse optional array size
			ast::ASTNodePtr arraySize = parseOptArraySize();

			// Parse optional initializer
			ast::ASTNodePtr initializer = parseOptInit();

			// Create variable declaration node
			auto varDecl = std::make_shared<ast::VariableDeclarationNode>(
					type,
					name,
					arraySize,
					initializer,
					identifierToken->getLocation());

			declarations.push_back(varDecl);

			// Parse more global variables if any
			parseMoreGlobalVars(declarations, type);
		}

		// If no comma, we're done (Rule 18)
	}

	std::vector<ast::ASTNodePtr> Parser::parseOptFunArgs() {
		// Rule 19: OPT_FUN_ARGS -> FUN_ARG FUN_ARG_TAIL
		// Rule 20: OPT_FUN_ARGS -> ε
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_VOID:
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR: {
				ast::ASTNodePtr arg = parseFunArg();
				std::vector<ast::ASTNodePtr> args = {arg};
				return parseFunArgTail(args);
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
			args.push_back(arg);
			return parseFunArgTail(args);
		}

		// No more parameters
		return args;
	}

	ast::ASTNodePtr Parser::parseFunArg() {
		// Rule 23: FUN_ARG -> TYPE identifier
		ast::ASTNodePtr type = parseType();

		auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected parameter name");
		std::string name = identifierToken->getLexeme();

		// Create parameter node
		return std::make_shared<ast::ParameterNode>(
				type,
				name,
				identifierToken->getLocation());
	}

} // namespace tinyc::parser
