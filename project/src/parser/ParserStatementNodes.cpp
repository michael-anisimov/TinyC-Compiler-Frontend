#include "tinyc/parser/Parser.h"


namespace tinyc::parser {

	// Statement parsing methods

	ast::ASTNodePtr Parser::parseStatement() {
		// Rules 24-33: STATEMENT -> ...
		switch (currentToken->getType()) {
			case lexer::TokenType::LBRACE:
				// Rule 24: STATEMENT -> BLOCK_STMT
				return parseBlockStmt();

			case lexer::TokenType::KW_IF:
				// Rule 25: STATEMENT -> IF_STMT
				return parseIfStmt();

			case lexer::TokenType::KW_SWITCH:
				// Rule 26: STATEMENT -> SWITCH_STMT
				return parseSwitchStmt();

			case lexer::TokenType::KW_WHILE:
				// Rule 27: STATEMENT -> WHILE_STMT
				return parseWhileStmt();

			case lexer::TokenType::KW_DO:
				// Rule 28: STATEMENT -> DO_WHILE_STMT
				return parseDoWhileStmt();

			case lexer::TokenType::KW_FOR:
				// Rule 29: STATEMENT -> FOR_STMT
				return parseForStmt();

			case lexer::TokenType::KW_BREAK:
				// Rule 30: STATEMENT -> BREAK_STMT
				return parseBreakStmt();

			case lexer::TokenType::KW_CONTINUE:
				// Rule 31: STATEMENT -> CONTINUE_STMT
				return parseContinueStmt();

			case lexer::TokenType::KW_RETURN:
				// Rule 32: STATEMENT -> RETURN_STMT
				return parseReturnStmt();

			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR:
			case lexer::TokenType::KW_VOID:
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
			case lexer::TokenType::KW_CAST:
				// Rule 33: STATEMENT -> EXPR_STMT
				return parseExprStmt();

			default:
				error("Expected statement");
		}
	}

	ast::ASTNodePtr Parser::parseBlockStmt() {
		// Rule 34: BLOCK_STMT -> { STATEMENT_STAR }
		auto lbraceToken = expect(lexer::TokenType::LBRACE, "Expected '{'");

		// Parse statements
		std::vector<ast::ASTNodePtr> statements = parseStatementStar();

		expect(lexer::TokenType::RBRACE, "Expected '}'");

		// Create block statement node
		return std::make_shared<ast::BlockStatementNode>(
				statements,
				lbraceToken->getLocation());
	}

	std::vector<ast::ASTNodePtr> Parser::parseStatementStar() {
		// Rule 35: STATEMENT_STAR -> STATEMENT STATEMENT_STAR
		// Rule 36: STATEMENT_STAR -> ε
		std::vector<ast::ASTNodePtr> statements;

		// Parse statements until we reach '}', 'case', or 'default'
		while (currentToken->getType() != lexer::TokenType::RBRACE &&
			   currentToken->getType() != lexer::TokenType::KW_CASE &&
			   currentToken->getType() != lexer::TokenType::KW_DEFAULT) {

			ast::ASTNodePtr stmt = parseStatement();
			statements.push_back(stmt);
		}

		return statements;
	}

	ast::ASTNodePtr Parser::parseIfStmt() {
		// Rule 37: IF_STMT -> if ( EXPR ) { STATEMENT } ELSE_PART
		auto ifToken = expect(lexer::TokenType::KW_IF, "Expected 'if'");

		expect(lexer::TokenType::LPAREN, "Expected '(' after 'if'");
		ast::ASTNodePtr condition = parseExpr();
		expect(lexer::TokenType::RPAREN, "Expected ')' after condition");

		ast::ASTNodePtr thenBranch;
		if (check(lexer::TokenType::LBRACE)) {
			thenBranch = parseBlockStmt();
		} else {
			thenBranch = parseStatement();
		}

		// Parse optional else part
		ast::ASTNodePtr elseBranch = parseElsePart();

		// Create if statement node
		return std::make_shared<ast::IfStatementNode>(
				condition,
				thenBranch,
				elseBranch,
				ifToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseElsePart() {
		// Rule 38: ELSE_PART -> else STATEMENT
		// Rule 39: ELSE_PART -> ε
		if (match(lexer::TokenType::KW_ELSE)) {
			return parseStatement();
		}

		// No else branch
		return nullptr;
	}

	ast::ASTNodePtr Parser::parseSwitchStmt() {
		// Rule 40: SWITCH_STMT -> switch ( EXPR ) { CASE_WITH_DEFAULT_STMT_STAR }
		auto switchToken = expect(lexer::TokenType::KW_SWITCH, "Expected 'switch'");

		expect(lexer::TokenType::LPAREN, "Expected '(' after 'switch'");
		ast::ASTNodePtr expression = parseExpr();
		expect(lexer::TokenType::RPAREN, "Expected ')' after switch expression");

		expect(lexer::TokenType::LBRACE, "Expected '{' after switch declaration");

		// Parse cases
		std::vector<ast::SwitchStatementNode::Case> cases = parseCaseWithDefaultStmtStar();

		expect(lexer::TokenType::RBRACE, "Expected '}' after switch body");

		// Create switch statement node
		return std::make_shared<ast::SwitchStatementNode>(
				expression,
				cases,
				switchToken->getLocation());
	}

	std::vector<ast::SwitchStatementNode::Case> Parser::parseCaseWithDefaultStmtStar() {
		// Rule 41: CASE_WITH_DEFAULT_STMT_STAR -> CASE_STMT CASE_WITH_DEFAULT_STMT_STAR
		// Rule 42: CASE_WITH_DEFAULT_STMT_STAR -> ε
		// Rule 43: CASE_WITH_DEFAULT_STMT_STAR -> DEFAULT_CASE CASE_STMT_STAR

		std::vector<ast::SwitchStatementNode::Case> cases;

		if (check(lexer::TokenType::KW_CASE)) {
			// Rule 41: Start with regular cases
			cases.push_back(parseCaseStmt());

			// Parse more cases
			while (check(lexer::TokenType::KW_CASE) || check(lexer::TokenType::KW_DEFAULT)) {
				if (check(lexer::TokenType::KW_CASE)) {
					cases.push_back(parseCaseStmt());
				} else {
					// Parse default case
					cases.push_back(parseDefaultCase());

					// Parse any remaining cases
					return parseCaseStmtStar(cases);
				}
			}
		} else if (check(lexer::TokenType::KW_DEFAULT)) {
			// Rule 43: Start with default case
			cases.push_back(parseDefaultCase());

			// Parse any remaining cases
			return parseCaseStmtStar(cases);
		}

		// If no cases, we're done (Rule 42)
		return cases;
	}

	std::vector<ast::SwitchStatementNode::Case> Parser::parseCaseStmtStar(
			std::vector<ast::SwitchStatementNode::Case> cases) {
		// Rule 44: CASE_STMT_STAR -> CASE_STMT CASE_STMT_STAR
		// Rule 45: CASE_STMT_STAR -> ε
		while (check(lexer::TokenType::KW_CASE)) {
			cases.push_back(parseCaseStmt());
		}

		return cases;
	}

	ast::SwitchStatementNode::Case Parser::parseCaseStmt() {
		// Rule 46: CASE_STMT -> case integer_literal : CASE_BODY
		expect(lexer::TokenType::KW_CASE, "Expected 'case'");

		auto intLiteralToken = expect(lexer::TokenType::INTEGER_LITERAL, "Expected integer literal after 'case'");
		int value = intLiteralToken->getIntValue();

		expect(lexer::TokenType::COLON, "Expected ':' after case value");

		// Parse case body
		std::vector<ast::ASTNodePtr> body = parseCaseBody();

		// Create case
		ast::SwitchStatementNode::Case caseNode;
		caseNode.value = value;
		caseNode.isDefault = false;
		caseNode.body = body;

		return caseNode;
	}

	std::vector<ast::ASTNodePtr> Parser::parseCaseBody() {
		// Rule 47: CASE_BODY -> STATEMENT_STAR
		return parseStatementStar();
	}

	ast::SwitchStatementNode::Case Parser::parseDefaultCase() {
		// Rule 48: DEFAULT_CASE -> default : CASE_BODY
		expect(lexer::TokenType::KW_DEFAULT, "Expected 'default'");
		expect(lexer::TokenType::COLON, "Expected ':' after 'default'");

		// Parse case body
		std::vector<ast::ASTNodePtr> body = parseCaseBody();

		// Create default case
		ast::SwitchStatementNode::Case defaultCase;
		defaultCase.value = 0; // Default case value doesn't matter
		defaultCase.isDefault = true;
		defaultCase.body = body;

		return defaultCase;
	}

	ast::ASTNodePtr Parser::parseWhileStmt() {
		// Rule 49: WHILE_STMT -> while ( EXPR ) STATEMENT
		auto whileToken = expect(lexer::TokenType::KW_WHILE, "Expected 'while'");

		expect(lexer::TokenType::LPAREN, "Expected '(' after 'while'");
		ast::ASTNodePtr condition = parseExpr();
		expect(lexer::TokenType::RPAREN, "Expected ')' after condition");

		ast::ASTNodePtr body = parseStatement();

		// Create while statement node
		return std::make_shared<ast::WhileStatementNode>(
				condition,
				body,
				whileToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseDoWhileStmt() {
		// Rule 50: DO_WHILE_STMT -> do STATEMENT while ( EXPR ) ;
		auto doToken = expect(lexer::TokenType::KW_DO, "Expected 'do'");

		ast::ASTNodePtr body = parseStatement();

		expect(lexer::TokenType::KW_WHILE, "Expected 'while' after do-statement");
		expect(lexer::TokenType::LPAREN, "Expected '(' after 'while'");
		ast::ASTNodePtr condition = parseExpr();
		expect(lexer::TokenType::RPAREN, "Expected ')' after condition");
		expect(lexer::TokenType::SEMICOLON, "Expected ';' after do-while statement");

		// Create do-while statement node
		return std::make_shared<ast::DoWhileStatementNode>(
				body,
				condition,
				doToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseForStmt() {
		// Rule 51: FOR_STMT -> for ( OPT_EXPR_OR_VAR_DECL ; OPT_EXPR ; OPT_EXPR ) STATEMENT
		auto forToken = expect(lexer::TokenType::KW_FOR, "Expected 'for'");

		expect(lexer::TokenType::LPAREN, "Expected '(' after 'for'");

		// Parse initialization (optional)
		ast::ASTNodePtr initialization = parseOptExprOrVarDecl();
		expect(lexer::TokenType::SEMICOLON, "Expected ';' after for-loop initialization");

		// Parse condition (optional)
		ast::ASTNodePtr condition = parseOptExpr();
		expect(lexer::TokenType::SEMICOLON, "Expected ';' after for-loop condition");

		// Parse update (optional)
		ast::ASTNodePtr update = parseOptExpr();
		expect(lexer::TokenType::RPAREN, "Expected ')' after for-loop update");

		// Parse body
		ast::ASTNodePtr body = parseStatement();

		// Create for statement node
		return std::make_shared<ast::ForStatementNode>(
				initialization,
				condition,
				update,
				body,
				forToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseOptExprOrVarDecl() {
		// Rule 52: OPT_EXPR_OR_VAR_DECL -> EXPR_OR_VAR_DECL
		// Rule 53: OPT_EXPR_OR_VAR_DECL -> ε
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR:
			case lexer::TokenType::KW_VOID:
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
			case lexer::TokenType::KW_CAST:
				return parseExprOrVarDecl();

			default:
				// No expression or variable declaration
				return nullptr;
		}
	}

	ast::ASTNodePtr Parser::parseOptExpr() {
		// Rule 54: OPT_EXPR -> EXPR
		// Rule 55: OPT_EXPR -> ε
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
			case lexer::TokenType::KW_CAST:
				return parseExpr();

			default:
				// No expression
				return nullptr;
		}
	}

	ast::ASTNodePtr Parser::parseBreakStmt() {
		// Rule 56: BREAK_STMT -> break ;
		auto breakToken = expect(lexer::TokenType::KW_BREAK, "Expected 'break'");
		expect(lexer::TokenType::SEMICOLON, "Expected ';' after 'break'");

		// Create break statement node
		return std::make_shared<ast::BreakStatementNode>(breakToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseContinueStmt() {
		// Rule 57: CONTINUE_STMT -> continue ;
		auto continueToken = expect(lexer::TokenType::KW_CONTINUE, "Expected 'continue'");
		expect(lexer::TokenType::SEMICOLON, "Expected ';' after 'continue'");

		// Create continue statement node
		return std::make_shared<ast::ContinueStatementNode>(continueToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseReturnStmt() {
		// Rule 58: RETURN_STMT -> return OPT_EXPR ;
		auto returnToken = expect(lexer::TokenType::KW_RETURN, "Expected 'return'");

		// Parse optional expression
		ast::ASTNodePtr expression = nullptr;
		if (currentToken->getType() != lexer::TokenType::SEMICOLON) {
			expression = parseExpr();
		}

		expect(lexer::TokenType::SEMICOLON, "Expected ';' after return statement");

		// Create return statement node
		return std::make_shared<ast::ReturnStatementNode>(
				expression,
				returnToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseExprStmt() {
		// Rule 59: EXPR_STMT -> EXPR_OR_VAR_DECL ;
		auto expr = parseExprOrVarDecl();
		expect(lexer::TokenType::SEMICOLON, "Expected ';' after expression");

		// If the expression is already a statement, return it directly
		if (dynamic_cast<ast::StatementNode *>(expr.get())) {
			return expr;
		}

		// Otherwise, wrap it in an expression statement
		return std::make_shared<ast::ExpressionStatementNode>(
				expr,
				expr->getLocation());
	}

	ast::ASTNodePtr Parser::parseExprOrVarDecl() {
		// Rule 60: EXPR_OR_VAR_DECL -> VAR_DECLS
		// Rule 61: EXPR_OR_VAR_DECL -> EXPRS
		switch (currentToken->getType()) {
			case lexer::TokenType::KW_INT:
			case lexer::TokenType::KW_DOUBLE:
			case lexer::TokenType::KW_CHAR:
			case lexer::TokenType::KW_VOID:
				return parseVarDecls();

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
			case lexer::TokenType::KW_CAST:
				return parseExprs();

			default:
				error("Expected expression or variable declaration");
		}
	}

	ast::ASTNodePtr Parser::parseVarDecls() {
		// Rule 62: VAR_DECLS -> VAR_DECL VAR_DECLS_TAIL
		ast::ASTNodePtr decl = parseVarDecl();

		std::vector<ast::ASTNodePtr> declarations = {decl};
		declarations = parseVarDeclsTail(declarations);

		// If only one declaration, return it directly
		if (declarations.size() == 1) {
			return declarations[0];
		}

		// Otherwise, return a list of declarations
		// We could create a special node for this, but for simplicity
		// we'll just return the first one for now
		return declarations[0];
	}

	std::vector<ast::ASTNodePtr> Parser::parseVarDeclsTail(std::vector<ast::ASTNodePtr> declarations) {
		// Rule 63: VAR_DECLS_TAIL -> , VAR_DECL VAR_DECLS_TAIL
		// Rule 64: VAR_DECLS_TAIL -> ε
		if (match(lexer::TokenType::COMMA)) {
			ast::ASTNodePtr decl = parseVarDecl();
			declarations.push_back(decl);
			return parseVarDeclsTail(declarations);
		}

		// No more declarations
		return declarations;
	}

	ast::ASTNodePtr Parser::parseVarDecl() {
		// Rule 65: VAR_DECL -> TYPE identifier OPT_ARRAY_SIZE OPT_INIT
		ast::ASTNodePtr type = parseType();

		auto identifierToken = expect(lexer::TokenType::IDENTIFIER, "Expected variable name");
		std::string name = identifierToken->getLexeme();

		// Parse optional array size
		ast::ASTNodePtr arraySize = parseOptArraySize();

		// Parse optional initializer
		ast::ASTNodePtr initializer = parseOptInit();

		// Create variable declaration node
		return std::make_shared<ast::VariableDeclarationNode>(
				type,
				name,
				arraySize,
				initializer,
				identifierToken->getLocation());
	}

	ast::ASTNodePtr Parser::parseOptArraySize() {
		// Rule 66: OPT_ARRAY_SIZE -> [ E9 ]
		// Rule 67: OPT_ARRAY_SIZE -> ε
		if (match(lexer::TokenType::LBRACKET)) {
			ast::ASTNodePtr size = parseE9();
			expect(lexer::TokenType::RBRACKET, "Expected ']' after array size");
			return size;
		}

		// No array size
		return nullptr;
	}

	ast::ASTNodePtr Parser::parseOptInit() {
		// Rule 68: OPT_INIT -> = EXPR
		// Rule 69: OPT_INIT -> ε
		if (match(lexer::TokenType::OP_ASSIGN)) {
			return parseExpr();
		}

		// No initializer
		return nullptr;
	}

} // namespace tinyc::parser
