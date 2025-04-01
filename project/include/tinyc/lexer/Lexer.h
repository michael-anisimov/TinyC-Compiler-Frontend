#ifndef TINYC_LEXER_H
#define TINYC_LEXER_H

#include "tinyc/lexer/Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>


namespace tinyc::lexer {

	/**
	 * @brief Exception thrown for lexical errors
	 */
	class LexerError : public std::runtime_error {
	public:
		LexerError(const std::string &message, const SourceLocation &location) : std::runtime_error(
				location.filename + ":" +
				std::to_string(location.line) + ":" +
				std::to_string(location.column) + ": " + message),
																				 location(location) {}

		[[nodiscard]] const SourceLocation &getLocation() const { return location; }

	private:
		SourceLocation location;
	};

	/**
	 * @brief Lexer class for converting source code to tokens
	 */
	class Lexer {
	public:
		/**
		 * @brief Construct a new Lexer object from source code
		 *
		 * @param source The source code to tokenize
		 * @param filename The name of the source file (for error reporting)
		 */
		explicit Lexer(std::string source, std::string filename = "<input>");

		/**
		 * @brief Get the next token from the source
		 *
		 * @return TokenPtr The next token
		 */
		TokenPtr nextToken();


		/**
		 * @brief Peek at the next token without consuming it
		 *
		 * @return TokenPtr The next token
		 */
		TokenPtr peekNextToken();

		/**
		 * @brief Tokenize the entire source and return all tokens
		 *
		 * @return std::vector<TokenPtr> All tokens in the source
		 */
		std::vector<TokenPtr> tokenize();


		/**
		 * @brief Get the name of the source file
		 *
		 * @return std::string The name of the source file
		 */
		[[nodiscard]] std::string getSourceName() const;

		/**
		 * @brief Get the current source location
		 *
		 * @return SourceLocation The current location in the source
		 */
		[[nodiscard]] SourceLocation getCurrentLocation() const;

	private:
		// Source code and position tracking
		std::string source;
		std::string filename;
		int position;
		int line;
		int column;

		/**
		 * @brief Get the current character
		 * Used to read the character at the current position
		 *
		 * @return char The current character in the source
		 */
		[[nodiscard]] char current() const;


		// Advance to the next character
		void advance();

		// Check if we've reached the end of the source
		[[nodiscard]] bool isAtEnd() const;

		// Skip whitespace and comments
		void skipWhitespace();

		// Lexing methods for different token types
		TokenPtr lexIdentifierOrKeyword();

		TokenPtr lexNumber();

		TokenPtr lexCharLiteral();

		TokenPtr lexStringLiteral();

		TokenPtr lexOperatorOrPunctuation();

		// Helper for creating tokens
		[[nodiscard]] TokenPtr createToken(TokenType type, const std::string &lexeme) const;

		// Keyword lookup map
		static const std::unordered_map<std::string, TokenType> keywords;
	};

} // namespace tinyc::lexer


#endif // TINYC_LEXER_H