#include "tinyc/lexer/Lexer.h"
#include <cctype>
#include <sstream>
#include <utility>

namespace tinyc::lexer {

	// Initialize the keywords map
	const std::unordered_map<std::string, TokenType> Lexer::keywords = {
			{"if",       TokenType::KW_IF},
			{"else",     TokenType::KW_ELSE},
			{"while",    TokenType::KW_WHILE},
			{"do",       TokenType::KW_DO},
			{"for",      TokenType::KW_FOR},
			{"switch",   TokenType::KW_SWITCH},
			{"case",     TokenType::KW_CASE},
			{"default",  TokenType::KW_DEFAULT},
			{"break",    TokenType::KW_BREAK},
			{"continue", TokenType::KW_CONTINUE},
			{"return",   TokenType::KW_RETURN},
			{"int",      TokenType::KW_INT},
			{"double",   TokenType::KW_DOUBLE},
			{"char",     TokenType::KW_CHAR},
			{"void",     TokenType::KW_VOID},
			{"struct",   TokenType::KW_STRUCT},
			{"typedef",  TokenType::KW_TYPEDEF},
			{"cast",     TokenType::KW_CAST}
	};

	Lexer::Lexer(std::string source, std::string filename)
			: source(std::move(source)), filename(std::move(filename)), position(0), line(1), column(1) {
	}

	TokenPtr Lexer::nextToken() {
		// Skip whitespace and comments
		skipWhitespace();

		// Check if we've reached the end of the source
		if (isAtEnd()) {
			return createToken(TokenType::END_OF_FILE, "");
		}

		// Get current character
		char c = current();

		// Process based on the character
		if (std::isalpha(c) || c == '_') {
			return lexIdentifierOrKeyword();
		}

		if (std::isdigit(c)) {
			return lexNumber();
		}

		if (c == '\'') {
			return lexCharLiteral();
		}

		if (c == '"') {
			return lexStringLiteral();
		}

		// Operators and punctuation
		return lexOperatorOrPunctuation();
	}

	std::vector<TokenPtr> Lexer::tokenize() {
		std::vector<TokenPtr> tokens;
		TokenPtr token;

		do {
			token = nextToken();
			tokens.push_back(token);
		} while (token->getType() != TokenType::END_OF_FILE);

		return tokens;
	}

	SourceLocation Lexer::getCurrentLocation() const {
		return {filename, line, column};
	}

	char Lexer::current() const {
		// Get the current character
		if (isAtEnd()) {
			return '\0';
		}
		return source[position];
	}

	void Lexer::advance() {
		char c = current();
		position++;

		if (c == '\n') {
			line++;
			column = 1;
		} else {
			column++;
		}
	}

	bool Lexer::isAtEnd() const {
		return position >= static_cast<int>(source.length());
	}

	void Lexer::skipWhitespace() {
		while (!isAtEnd()) {
			char c = current();

			if (std::isspace(c)) {
				advance();
			} else if (c == '/' && position + 1 < static_cast<int>(source.length())) {
				// Check for C-style comments
				if (source[position + 1] == '/') {
					// Single-line comment, skip until end of line
					advance(); // Skip '/'
					advance(); // Skip '/'

					while (!isAtEnd() && current() != '\n') {
						advance();
					}
				} else if (source[position + 1] == '*') {
					// Multi-line comment, skip until closing */
					advance(); // Skip '/'
					advance(); // Skip '*'

					bool commentClosed = false;
					while (!isAtEnd() && !commentClosed) {
						if (current() == '*' && position + 1 < static_cast<int>(source.length()) &&
							source[position + 1] == '/') {
							advance(); // Skip '*'
							advance(); // Skip '/'
							commentClosed = true;
						} else {
							advance();
						}
					}

					if (!commentClosed) {
						throw LexerError("Unclosed multi-line comment", getCurrentLocation());
					}
				} else {
					// Not a comment, just a division operator
					break;
				}
			} else {
				break;
			}
		}
	}

	TokenPtr Lexer::lexIdentifierOrKeyword() {
		SourceLocation start = getCurrentLocation();
		std::string lexeme;

		while (!isAtEnd() && (std::isalnum(current()) || current() == '_')) {
			lexeme += current();
			advance();
		}

		// Check if it's a keyword
		auto it = keywords.find(lexeme);
		if (it != keywords.end()) {
			return std::make_shared<Token>(it->second, lexeme, start);
		}

		// It's an identifier
		return std::make_shared<Token>(TokenType::IDENTIFIER, lexeme, start);
	}

	TokenPtr Lexer::lexNumber() {
		SourceLocation start = getCurrentLocation();
		std::string lexeme;
		bool isDouble = false;

		// Process integer part
		while (!isAtEnd() && std::isdigit(current())) {
			lexeme += current();
			advance();
		}

		// Check for decimal point
		if (!isAtEnd() && current() == '.') {
			isDouble = true;
			lexeme += current();
			advance();

			// Process fractional part
			while (!isAtEnd() && std::isdigit(current())) {
				lexeme += current();
				advance();
			}
		}

		// Check for scientific notation
		if (!isAtEnd() && (current() == 'e' || current() == 'E')) {
			isDouble = true;
			lexeme += current();
			advance();

			// Optional sign
			if (!isAtEnd() && (current() == '+' || current() == '-')) {
				lexeme += current();
				advance();
			}

			// Must have at least one digit
			if (isAtEnd() || !std::isdigit(current())) {
				throw LexerError("Invalid scientific notation in number", getCurrentLocation());
			}

			// Process exponent
			while (!isAtEnd() && std::isdigit(current())) {
				lexeme += current();
				advance();
			}
		}

		// Create token with the appropriate type and value
		if (isDouble) {
			try {
				double value = std::stod(lexeme);
				return std::make_shared<Token>(TokenType::DOUBLE_LITERAL, value, lexeme, start);
			}
			catch (const std::exception &e) {
				throw LexerError("Invalid double literal: " + lexeme, start);
			}
		} else {
			try {
				int value = std::stoi(lexeme);
				return std::make_shared<Token>(TokenType::INTEGER_LITERAL, value, lexeme, start);
			}
			catch (const std::exception &e) {
				throw LexerError("Invalid integer literal: " + lexeme, start);
			}
		}
	}

	TokenPtr Lexer::lexCharLiteral() {
		SourceLocation start = getCurrentLocation();
		advance(); // Skip opening quote

		if (isAtEnd()) {
			throw LexerError("Unterminated character literal", start);
		}

		char value;
		std::string lexeme = "'";

		if (current() == '\\') {
			// Handle escape sequence
			lexeme += current();
			advance();

			if (isAtEnd()) {
				throw LexerError("Unterminated character literal", start);
			}

			switch (current()) {
				case 'n':
					value = '\n';
					break;
				case 't':
					value = '\t';
					break;
				case 'r':
					value = '\r';
					break;
				case '0':
					value = '\0';
					break;
				case '\\':
					value = '\\';
					break;
				case '\'':
					value = '\'';
					break;
				case '"':
					value = '"';
					break;
				default:
					throw LexerError(std::string("Invalid escape sequence: \\") + current(), getCurrentLocation());
			}

			lexeme += current();
			advance();
		} else {
			value = current();
			lexeme += current();
			advance();
		}

		if (isAtEnd() || current() != '\'') {
			throw LexerError("Unterminated character literal", start);
		}

		lexeme += current(); // Add closing quote
		advance();           // Skip closing quote

		return std::make_shared<Token>(TokenType::CHAR_LITERAL, value, lexeme, start);
	}

	TokenPtr Lexer::lexStringLiteral() {
		SourceLocation start = getCurrentLocation();
		advance(); // Skip opening quote

		std::string lexeme = "\"";
		std::string value;

		while (!isAtEnd() && current() != '"') {
			if (current() == '\\') {
				// Handle escape sequence
				lexeme += current();
				advance();

				if (isAtEnd()) {
					throw LexerError("Unterminated string literal", start);
				}

				char escapedChar;
				switch (current()) {
					case 'n':
						escapedChar = '\n';
						break;
					case 't':
						escapedChar = '\t';
						break;
					case 'r':
						escapedChar = '\r';
						break;
					case '0':
						escapedChar = '\0';
						break;
					case '\\':
						escapedChar = '\\';
						break;
					case '\'':
						escapedChar = '\'';
						break;
					case '"':
						escapedChar = '"';
						break;
					default:
						throw LexerError(std::string("Invalid escape sequence: \\") + current(), getCurrentLocation());
				}

				value += escapedChar;
				lexeme += current();
				advance();
			} else {
				value += current();
				lexeme += current();
				advance();
			}
		}

		if (isAtEnd()) {
			throw LexerError("Unterminated string literal", start);
		}

		lexeme += current(); // Add closing quote
		advance();           // Skip closing quote

		return std::make_shared<Token>(TokenType::STRING_LITERAL, lexeme, start);
	}

	TokenPtr Lexer::lexOperatorOrPunctuation() {
		SourceLocation start = getCurrentLocation();
		char c = current();
		advance();

		switch (c) {
			// Single-character operators and punctuation
			case '(':
				return createToken(TokenType::LPAREN, "(");
			case ')':
				return createToken(TokenType::RPAREN, ")");
			case '{':
				return createToken(TokenType::LBRACE, "{");
			case '}':
				return createToken(TokenType::RBRACE, "}");
			case '[':
				return createToken(TokenType::LBRACKET, "[");
			case ']':
				return createToken(TokenType::RBRACKET, "]");
			case ';':
				return createToken(TokenType::SEMICOLON, ";");
			case ':':
				return createToken(TokenType::COLON, ":");
			case ',':
				return createToken(TokenType::COMMA, ",");
			case '.':
				return createToken(TokenType::OP_DOT, ".");
			case '+':
				if (!isAtEnd() && current() == '+') {
					advance();
					return createToken(TokenType::OP_INCREMENT, "++");
				}
				return createToken(TokenType::OP_PLUS, "+");
			case '-':
				if (!isAtEnd()) {
					if (current() == '-') {
						advance();
						return createToken(TokenType::OP_DECREMENT, "--");
					} else if (current() == '>') {
						advance();
						return createToken(TokenType::OP_ARROW, "->");
					}
				}
				return createToken(TokenType::OP_MINUS, "-");
			case '*':
				return createToken(TokenType::OP_MULTIPLY, "*");
			case '/':
				return createToken(TokenType::OP_DIVIDE, "/");
			case '%':
				return createToken(TokenType::OP_MODULO, "%");
			case '=':
				if (!isAtEnd() && current() == '=') {
					advance();
					return createToken(TokenType::OP_EQUAL, "==");
				}
				return createToken(TokenType::OP_ASSIGN, "=");
			case '!':
				if (!isAtEnd() && current() == '=') {
					advance();
					return createToken(TokenType::OP_NOT_EQUAL, "!=");
				}
				return createToken(TokenType::OP_NOT, "!");
			case '<':
				if (!isAtEnd()) {
					if (current() == '=') {
						advance();
						return createToken(TokenType::OP_LESS_EQUAL, "<=");
					} else if (current() == '<') {
						advance();
						return createToken(TokenType::OP_LEFT_SHIFT, "<<");
					}
				}
				return createToken(TokenType::OP_LESS, "<");
			case '>':
				if (!isAtEnd()) {
					if (current() == '=') {
						advance();
						return createToken(TokenType::OP_GREATER_EQUAL, ">=");
					} else if (current() == '>') {
						advance();
						return createToken(TokenType::OP_RIGHT_SHIFT, ">>");
					}
				}
				return createToken(TokenType::OP_GREATER, ">");
			case '&':
				if (!isAtEnd() && current() == '&') {
					advance();
					return createToken(TokenType::OP_LOGICAL_AND, "&&");
				}
				return createToken(TokenType::OP_AND, "&");
			case '|':
				if (!isAtEnd() && current() == '|') {
					advance();
					return createToken(TokenType::OP_LOGICAL_OR, "||");
				}
				return createToken(TokenType::OP_OR, "|");
			case '~':
				return createToken(TokenType::OP_BITWISE_NOT, "~");
			default:
				// Unknown character
				std::stringstream ss;
				ss << "Unexpected character: '" << c << "' (ASCII: " << static_cast<int>(c) << ")";
				throw LexerError(ss.str(), start);
		}
	}

	TokenPtr Lexer::createToken(TokenType type, const std::string &lexeme) const {
		SourceLocation location = {filename, line, column - static_cast<int>(lexeme.length())};
		return std::make_shared<Token>(type, lexeme, location);
	}

} // namespace tinyc::lexer