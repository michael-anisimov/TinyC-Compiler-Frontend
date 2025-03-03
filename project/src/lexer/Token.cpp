#include "tinyc/lexer/Token.h"
#include <stdexcept>

namespace tinyc
{
    namespace lexer
    {

        Token::Token(TokenType type, const std::string &lexeme, const SourceLocation &location)
            : type(type), lexeme(lexeme), location(location), hasValue(false)
        {
        }

        Token::Token(TokenType type, int value, const std::string &lexeme, const SourceLocation &location)
            : type(type), lexeme(lexeme), location(location), intValue(value), hasValue(true)
        {
            if (type != TokenType::INTEGER_LITERAL)
            {
                throw std::invalid_argument("Cannot create non-integer token with integer value");
            }
        }

        Token::Token(TokenType type, double value, const std::string &lexeme, const SourceLocation &location)
            : type(type), lexeme(lexeme), location(location), doubleValue(value), hasValue(true)
        {
            if (type != TokenType::DOUBLE_LITERAL)
            {
                throw std::invalid_argument("Cannot create non-double token with double value");
            }
        }

        Token::Token(TokenType type, char value, const std::string &lexeme, const SourceLocation &location)
            : type(type), lexeme(lexeme), location(location), charValue(value), hasValue(true)
        {
            if (type != TokenType::CHAR_LITERAL)
            {
                throw std::invalid_argument("Cannot create non-char token with char value");
            }
        }

        int Token::getIntValue() const
        {
            if (!hasIntValue())
            {
                throw std::runtime_error("Token does not have an integer value");
            }
            return intValue;
        }

        double Token::getDoubleValue() const
        {
            if (!hasDoubleValue())
            {
                throw std::runtime_error("Token does not have a double value");
            }
            return doubleValue;
        }

        char Token::getCharValue() const
        {
            if (!hasCharValue())
            {
                throw std::runtime_error("Token does not have a char value");
            }
            return charValue;
        }

        std::string Token::typeToString(TokenType type)
        {
            switch (type)
            {
            case TokenType::END_OF_FILE:
                return "EOF";

            // Keywords
            case TokenType::KW_IF:
                return "KW_IF";
            case TokenType::KW_ELSE:
                return "KW_ELSE";
            case TokenType::KW_WHILE:
                return "KW_WHILE";
            case TokenType::KW_DO:
                return "KW_DO";
            case TokenType::KW_FOR:
                return "KW_FOR";
            case TokenType::KW_SWITCH:
                return "KW_SWITCH";
            case TokenType::KW_CASE:
                return "KW_CASE";
            case TokenType::KW_DEFAULT:
                return "KW_DEFAULT";
            case TokenType::KW_BREAK:
                return "KW_BREAK";
            case TokenType::KW_CONTINUE:
                return "KW_CONTINUE";
            case TokenType::KW_RETURN:
                return "KW_RETURN";
            case TokenType::KW_INT:
                return "KW_INT";
            case TokenType::KW_DOUBLE:
                return "KW_DOUBLE";
            case TokenType::KW_CHAR:
                return "KW_CHAR";
            case TokenType::KW_VOID:
                return "KW_VOID";
            case TokenType::KW_STRUCT:
                return "KW_STRUCT";
            case TokenType::KW_TYPEDEF:
                return "KW_TYPEDEF";
            case TokenType::KW_CAST:
                return "KW_CAST";

            // Identifiers and literals
            case TokenType::IDENTIFIER:
                return "IDENTIFIER";
            case TokenType::INTEGER_LITERAL:
                return "INTEGER_LITERAL";
            case TokenType::DOUBLE_LITERAL:
                return "DOUBLE_LITERAL";
            case TokenType::CHAR_LITERAL:
                return "CHAR_LITERAL";
            case TokenType::STRING_LITERAL:
                return "STRING_LITERAL";

            // Operators
            case TokenType::OP_PLUS:
                return "OP_PLUS";
            case TokenType::OP_MINUS:
                return "OP_MINUS";
            case TokenType::OP_MULTIPLY:
                return "OP_MULTIPLY";
            case TokenType::OP_DIVIDE:
                return "OP_DIVIDE";
            case TokenType::OP_MODULO:
                return "OP_MODULO";
            case TokenType::OP_ASSIGN:
                return "OP_ASSIGN";
            case TokenType::OP_EQUAL:
                return "OP_EQUAL";
            case TokenType::OP_NOT_EQUAL:
                return "OP_NOT_EQUAL";
            case TokenType::OP_LESS:
                return "OP_LESS";
            case TokenType::OP_LESS_EQUAL:
                return "OP_LESS_EQUAL";
            case TokenType::OP_GREATER:
                return "OP_GREATER";
            case TokenType::OP_GREATER_EQUAL:
                return "OP_GREATER_EQUAL";
            case TokenType::OP_AND:
                return "OP_AND";
            case TokenType::OP_OR:
                return "OP_OR";
            case TokenType::OP_LOGICAL_AND:
                return "OP_LOGICAL_AND";
            case TokenType::OP_LOGICAL_OR:
                return "OP_LOGICAL_OR";
            case TokenType::OP_NOT:
                return "OP_NOT";
            case TokenType::OP_BITWISE_NOT:
                return "OP_BITWISE_NOT";
            case TokenType::OP_INCREMENT:
                return "OP_INCREMENT";
            case TokenType::OP_DECREMENT:
                return "OP_DECREMENT";
            case TokenType::OP_LEFT_SHIFT:
                return "OP_LEFT_SHIFT";
            case TokenType::OP_RIGHT_SHIFT:
                return "OP_RIGHT_SHIFT";
            case TokenType::OP_ARROW:
                return "OP_ARROW";
            case TokenType::OP_DOT:
                return "OP_DOT";
            case TokenType::OP_ADDRESS_OF:
                return "OP_ADDRESS_OF";
            case TokenType::OP_DEREFERENCE:
                return "OP_DEREFERENCE";

            // Punctuation
            case TokenType::LPAREN:
                return "LPAREN";
            case TokenType::RPAREN:
                return "RPAREN";
            case TokenType::LBRACE:
                return "LBRACE";
            case TokenType::RBRACE:
                return "RBRACE";
            case TokenType::LBRACKET:
                return "LBRACKET";
            case TokenType::RBRACKET:
                return "RBRACKET";
            case TokenType::SEMICOLON:
                return "SEMICOLON";
            case TokenType::COLON:
                return "COLON";
            case TokenType::COMMA:
                return "COMMA";

            // Error
            case TokenType::ERROR:
                return "ERROR";

            default:
                return "UNKNOWN";
            }
        }

        std::ostream &operator<<(std::ostream &os, const Token &token)
        {
            os << Token::typeToString(token.type) << " '" << token.lexeme << "' at " << token.location;

            if (token.hasIntValue())
            {
                os << " (value: " << token.intValue << ")";
            }
            else if (token.hasDoubleValue())
            {
                os << " (value: " << token.doubleValue << ")";
            }
            else if (token.hasCharValue())
            {
                os << " (value: '" << token.charValue << "')";
            }

            return os;
        }

    } // namespace lexer
} // namespace tinyc