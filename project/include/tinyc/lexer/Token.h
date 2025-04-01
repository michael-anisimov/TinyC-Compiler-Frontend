#ifndef TINYC_TOKEN_H
#define TINYC_TOKEN_H

#include <string>
#include <memory>
#include <ostream>
#include <utility>

namespace tinyc::lexer {

	/**
	 * @brief SourceLocation represents a position in the source code
	 */
	struct SourceLocation {
		std::string filename; // Source file name
		int line;             // Line number (1-based)
		int column;           // Column number (1-based)

		SourceLocation() : line(1), column(1) {}

		SourceLocation(std::string file, int l, int c) : filename(std::move(file)), line(l), column(c) {}

		friend std::ostream &operator<<(std::ostream &os, const SourceLocation &loc) {
			os << loc.filename << ":" << loc.line << ":" << loc.column;
			return os;
		}
	};

	/**
	 * @brief TokenType enumeration of all possible token types
	 */
	enum class TokenType {
		// End of file
		END_OF_FILE,

		// Keywords
		KW_IF,
		KW_ELSE,
		KW_WHILE,
		KW_DO,
		KW_FOR,
		KW_SWITCH,
		KW_CASE,
		KW_DEFAULT,
		KW_BREAK,
		KW_CONTINUE,
		KW_RETURN,
		KW_INT,
		KW_DOUBLE,
		KW_CHAR,
		KW_VOID,
		KW_STRUCT,
		KW_TYPEDEF,
		KW_CAST,

		// Identifiers
		IDENTIFIER,

		// Literals
		INTEGER_LITERAL,
		DOUBLE_LITERAL,
		CHAR_LITERAL,
		STRING_LITERAL,

		// Operators
		OP_PLUS,          // +
		OP_MINUS,         // -
		OP_MULTIPLY,      // *
		OP_DIVIDE,        // /
		OP_MODULO,        // %
		OP_ASSIGN,        // =
		OP_EQUAL,         // ==
		OP_NOT_EQUAL,     // !=
		OP_LESS,          // <
		OP_LESS_EQUAL,    // <=
		OP_GREATER,       // >
		OP_GREATER_EQUAL, // >=
		OP_AND,           // &
		OP_OR,            // |
		OP_LOGICAL_AND,   // &&
		OP_LOGICAL_OR,    // ||
		OP_NOT,           // !
		OP_BITWISE_NOT,   // ~
		OP_INCREMENT,     // ++
		OP_DECREMENT,     // --
		OP_LEFT_SHIFT,    // <<
		OP_RIGHT_SHIFT,   // >>
		OP_ARROW,         // ->
		OP_DOT,           // .
		OP_ADDRESS_OF,    // & (parsed as OP_AND and interpreted based on context)
		OP_DEREFERENCE,   // * (parsed as OP_MULTIPLY and interpreted based on context)

		// Punctuation
		LPAREN,    // (
		RPAREN,    // )
		LBRACE,    // {
		RBRACE,    // }
		LBRACKET,  // [
		RBRACKET,  // ]
		SEMICOLON, // ;
		COLON,     // :
		COMMA,     // ,

		// Error token
		ERROR
	};

	/**
	 * @brief Token class representing a lexical token
	 */
	class Token {
	private:
		TokenType type;
		std::string lexeme;
		SourceLocation location;

		// Values for literals
		union {
			int intValue;
			double doubleValue;
			char charValue;
		};

		bool hasValue;

	public:
		/**
		 * @brief Construct a new Token object
		 *
		 * @param type The token type
		 * @param lexeme The literal text of the token
		 * @param location The source location where the token was found
		 */
		Token(TokenType type, std::string lexeme, SourceLocation location);

		/**
		 * @brief Construct a new Token object with an integer value
		 */
		Token(TokenType type, int value, std::string lexeme, SourceLocation location);

		/**
		 * @brief Construct a new Token object with a double value
		 */
		Token(TokenType type, double value, std::string lexeme, SourceLocation location);

		/**
		 * @brief Construct a new Token object with a char value
		 */
		Token(TokenType type, char value, std::string lexeme, SourceLocation location);

		/**
		 * @brief Get the token type
		 */
		[[nodiscard]] TokenType getType() const { return type; }

		/**
		 * @brief Get the lexeme
		 */
		[[nodiscard]] const std::string &getLexeme() const { return lexeme; }

		/**
		 * @brief Get the source location
		 */
		[[nodiscard]] const SourceLocation &getLocation() const { return location; }

		/**
		 * @brief Check if the token has a value
		 */
		[[nodiscard]] bool hasIntValue() const { return hasValue && type == TokenType::INTEGER_LITERAL; }

		[[nodiscard]] bool hasDoubleValue() const { return hasValue && type == TokenType::DOUBLE_LITERAL; }

		[[nodiscard]] bool hasCharValue() const { return hasValue && type == TokenType::CHAR_LITERAL; }

		/**
		 * @brief Get the token value
		 */
		[[nodiscard]] int getIntValue() const;

		[[nodiscard]] double getDoubleValue() const;

		[[nodiscard]] char getCharValue() const;

		/**
		 * @brief Convert TokenType to string
		 */
		static std::string typeToString(TokenType type);

		/**
		 * @brief Output operator for Token
		 */
		friend std::ostream &operator<<(std::ostream &os, const Token &token);
	};

	// Using a shared_ptr for token management
	using TokenPtr = std::shared_ptr<Token>;

} // namespace tinyc::lexer


#endif // TINYC_TOKEN_H