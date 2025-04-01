#ifndef TINYC_PARSER_H
#define TINYC_PARSER_H

#include "tinyc/lexer/Lexer.h"
#include "tinyc/ast/ASTNode.h"
#include "tinyc/ast/ASTNode.h"
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>


namespace tinyc::parser {

	/**
	 * @brief Exception thrown for parsing errors
	 */
	class ParserError : public std::runtime_error {
	public:
		ParserError(const std::string &message, const lexer::SourceLocation &location)
				: std::runtime_error(location.filename + ":" +
									 std::to_string(location.line) + ":" +
									 std::to_string(location.column) + ": " + message),
				  location(location) {}

		[[nodiscard]] const lexer::SourceLocation &getLocation() const { return location; }

	private:
		lexer::SourceLocation location;
	};

	/**
	 * @brief Parser for the TinyC language
	 *
	 * Implements a recursive descent parser for TinyC according to the LL(1) grammar.
	 */
	class Parser {
	public:
		/**
		 * @brief Construct a new Parser
		 *
		 * @param lexer The lexer to get tokens from
		 */
		explicit Parser(lexer::Lexer &lexer);

		/**
		 * @brief Parse a TinyC program
		 *
		 * @return The AST for the program
		 * @throws ParserError if there is a syntax error
		 */
		ast::ASTNodePtr parseProgram();

	private:
		lexer::Lexer &lexer;
		lexer::TokenPtr currentToken;

		// Helper methods

		/**
		 * @brief Consume the current token and advance to the next one
		 *
		 * @return The consumed token
		 */
		lexer::TokenPtr consume();

		/**
		 * @brief Check if the current token matches the given type
		 *
		 * @param type The token type to check for
		 * @return true if the current token matches, false otherwise
		 */
		[[nodiscard]] bool check(lexer::TokenType type) const;

		/**
		 * @brief Consume the current token if it matches the given type
		 *
		 * @param type The token type to match
		 * @return true if matched and consumed, false otherwise
		 */
		bool match(lexer::TokenType type);

		/**
		 * @brief Expect the current token to be of the given type
		 *
		 * Consumes the token if it matches, otherwise throws an error.
		 *
		 * @param type The expected token type
		 * @param message The error message if the token doesn't match
		 * @return The consumed token
		 * @throws ParserError if the token doesn't match
		 */
		lexer::TokenPtr expect(lexer::TokenType type, const std::string &message);

		/**
		 * @brief Report a parsing error
		 *
		 * @param message The error message
		 * @throws ParserError with the given message
		 */
		[[noreturn]] void error(const std::string &message) const;

		// Parse methods for each non-terminal in the grammar

		// Program
		ast::ASTNodePtr parseProgramItem();

		// Types
		ast::ASTNodePtr parseType();

		ast::ASTNodePtr parseNonVoidType();

		ast::ASTNodePtr parseBaseType();

		ast::ASTNodePtr parseNamedType();

		ast::ASTNodePtr parseTypeFunRet();

		ast::ASTNodePtr parseFunRetTypes();

		void parseStarPlus(ast::ASTNodePtr &baseType);

		void parseStarSeq(ast::ASTNodePtr &baseType);

		ast::ASTNodePtr parseStructDecl();

		ast::ASTNodePtr parseFunPtrDecl();

		// Function and variable declarations
		ast::ASTNodePtr parseNotVoidFunctionOrVariable(
				ast::ASTNodePtr type,
				const std::string &name,
				const lexer::SourceLocation &location);

		ast::ASTNodePtr parseVoidDeclTail(const lexer::SourceLocation &voidLocation);

		ast::ASTNodePtr parseFuncOrVarTail(
				ast::ASTNodePtr type,
				const std::string &name,
				const lexer::SourceLocation &location);

		ast::ASTNodePtr parseVariableTail(
				ast::ASTNodePtr type,
				const std::string &identifier,
				const lexer::SourceLocation &location);

		ast::ASTNodePtr parseFunctionDeclarationTail(
				ast::ASTNodePtr returnType,
				const std::string &name,
				const lexer::SourceLocation &location);

		ast::ASTNodePtr parseFuncTail();

		std::vector<ast::ASTNodePtr> parseOptFunArgs();

		std::vector<ast::ASTNodePtr> parseFunArgTail(std::vector<ast::ASTNodePtr> args);

		ast::ASTNodePtr parseFunArg();

		std::vector<ast::ASTNodePtr> parseOptFunPtrArgs();

		std::vector<ast::ASTNodePtr> parseFunPtrArgs();

		std::vector<ast::ASTNodePtr> parseFunPtrArgsTail(std::vector<ast::ASTNodePtr> types);

		// Statements
		ast::ASTNodePtr parseStatement();

		ast::ASTNodePtr parseBlockStmt();

		std::vector<ast::ASTNodePtr> parseStatementStar();

		ast::ASTNodePtr parseIfStmt();

		ast::ASTNodePtr parseElsePart();

		ast::ASTNodePtr parseSwitchStmt();

		std::vector<ast::SwitchStatementNode::Case> parseCaseWithDefaultStmtStar();

		std::vector<ast::SwitchStatementNode::Case> parseCaseStmtStar(
				std::vector<ast::SwitchStatementNode::Case> cases);

		ast::SwitchStatementNode::Case parseCaseStmt();

		std::vector<ast::ASTNodePtr> parseCaseBody();

		ast::SwitchStatementNode::Case parseDefaultCase();

		ast::ASTNodePtr parseWhileStmt();

		ast::ASTNodePtr parseDoWhileStmt();

		ast::ASTNodePtr parseForStmt();

		ast::ASTNodePtr parseOptExprOrVarDecl();

		ast::ASTNodePtr parseOptExpr();

		ast::ASTNodePtr parseBreakStmt();

		ast::ASTNodePtr parseContinueStmt();

		ast::ASTNodePtr parseReturnStmt();

		ast::ASTNodePtr parseExprStmt();

		ast::ASTNodePtr parseExprOrVarDecl();

		// Variable declarations
		ast::ASTNodePtr parseVarDecls();

		std::vector<ast::ASTNodePtr> parseVarDeclsTail(std::vector<ast::ASTNodePtr> declarations);

		ast::ASTNodePtr parseVarDecl();

		ast::ASTNodePtr parseOptArraySize();

		ast::ASTNodePtr parseOptInit();

		// Expressions
		ast::ASTNodePtr parseExprs();

		std::vector<ast::ASTNodePtr> parseExprsTail(std::vector<ast::ASTNodePtr> expressions);

		ast::ASTNodePtr parseExpr();

		ast::ASTNodePtr parseExprTail(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE9();

		ast::ASTNodePtr parseE9Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE8();

		ast::ASTNodePtr parseE8Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE7();

		ast::ASTNodePtr parseE7Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE6();

		ast::ASTNodePtr parseE6Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE5();

		ast::ASTNodePtr parseE5Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE4();

		ast::ASTNodePtr parseE4Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE3();

		ast::ASTNodePtr parseE3Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE2();

		ast::ASTNodePtr parseE2Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseE1();

		ast::ASTNodePtr parseE1Prime(ast::ASTNodePtr left);

		ast::ASTNodePtr parseEUnaryPre();

		ast::ASTNodePtr parseECallIndexMemberPost();

		ast::ASTNodePtr parseECallIndexMemberPostPrime(ast::ASTNodePtr expr);

		ast::ASTNodePtr parseECall(ast::ASTNodePtr callee);

		std::vector<ast::ASTNodePtr> parseOptExprList();

		std::vector<ast::ASTNodePtr> parseExprTailList(std::vector<ast::ASTNodePtr> expressions);

		ast::ASTNodePtr parseEIndex(ast::ASTNodePtr array);

		ast::ASTNodePtr parseEMember(ast::ASTNodePtr object);

		ast::ASTNodePtr parseEPost(ast::ASTNodePtr operand);

		ast::ASTNodePtr parseF();

		ast::ASTNodePtr parseECast();
	};

} // namespace tinyc::parser


#endif // TINYC_PARSER_H