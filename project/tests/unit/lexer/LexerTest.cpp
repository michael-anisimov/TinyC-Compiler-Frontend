#include "tinyc/lexer/Lexer.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace tinyc::lexer;

// Helper function to assert token types
void assertTokenTypes(const std::vector<TokenPtr> &tokens,
                      const std::vector<TokenType> &expectedTypes)
{
    ASSERT_EQ(tokens.size(), expectedTypes.size() + 1); // +1 for EOF

    for (size_t i = 0; i < expectedTypes.size(); ++i)
    {
        EXPECT_EQ(tokens[i]->getType(), expectedTypes[i])
            << "Token at position " << i << " has wrong type. Expected: "
            << Token::typeToString(expectedTypes[i]) << ", got: "
            << Token::typeToString(tokens[i]->getType());
    }

    // Last token should be EOF
    EXPECT_EQ(tokens.back()->getType(), TokenType::END_OF_FILE);
}

// Helper function to assert token lexemes
void assertTokenLexemes(const std::vector<TokenPtr> &tokens,
                        const std::vector<std::string> &expectedLexemes)
{
    ASSERT_EQ(tokens.size(), expectedLexemes.size() + 1); // +1 for EOF

    for (size_t i = 0; i < expectedLexemes.size(); ++i)
    {
        EXPECT_EQ(tokens[i]->getLexeme(), expectedLexemes[i])
            << "Token at position " << i << " has wrong lexeme. Expected: '"
            << expectedLexemes[i] << "', got: '" << tokens[i]->getLexeme() << "'";
    }
}

// Test keywords
TEST(LexerTest, Keywords)
{
    std::string source = "if else while do for switch case default break continue return "
                         "int double char void struct typedef cast";
    Lexer lexer(source);

    std::vector<TokenPtr> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::KW_IF, TokenType::KW_ELSE, TokenType::KW_WHILE, TokenType::KW_DO,
        TokenType::KW_FOR, TokenType::KW_SWITCH, TokenType::KW_CASE, TokenType::KW_DEFAULT,
        TokenType::KW_BREAK, TokenType::KW_CONTINUE, TokenType::KW_RETURN, TokenType::KW_INT,
        TokenType::KW_DOUBLE, TokenType::KW_CHAR, TokenType::KW_VOID, TokenType::KW_STRUCT,
        TokenType::KW_TYPEDEF, TokenType::KW_CAST};

    assertTokenTypes(tokens, expectedTypes);
}

// Test identifiers
TEST(LexerTest, Identifiers)
{
    std::string source = "identifier _underscore leading123 mixed_123_case";
    Lexer lexer(source);

    std::vector<TokenPtr> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::IDENTIFIER, TokenType::IDENTIFIER, TokenType::IDENTIFIER, TokenType::IDENTIFIER};

    std::vector<std::string> expectedLexemes = {
        "identifier", "_underscore", "leading123", "mixed_123_case"};

    assertTokenTypes(tokens, expectedTypes);
    assertTokenLexemes(tokens, expectedLexemes);
}

// Test literals
TEST(LexerTest, Literals)
{
    std::string source = "123 123.456 'a' '\\n' \"string\" \"escaped\\tstring\"";
    Lexer lexer(source);

    std::vector<TokenPtr> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::INTEGER_LITERAL, TokenType::DOUBLE_LITERAL,
        TokenType::CHAR_LITERAL, TokenType::CHAR_LITERAL,
        TokenType::STRING_LITERAL, TokenType::STRING_LITERAL};

    assertTokenTypes(tokens, expectedTypes);

    // Check integer and double values
    EXPECT_TRUE(tokens[0]->hasIntValue());
    EXPECT_EQ(tokens[0]->getIntValue(), 123);

    EXPECT_TRUE(tokens[1]->hasDoubleValue());
    EXPECT_DOUBLE_EQ(tokens[1]->getDoubleValue(), 123.456);

    // Check char values
    EXPECT_TRUE(tokens[2]->hasCharValue());
    EXPECT_EQ(tokens[2]->getCharValue(), 'a');

    EXPECT_TRUE(tokens[3]->hasCharValue());
    EXPECT_EQ(tokens[3]->getCharValue(), '\n');
}

// Test operators
TEST(LexerTest, Operators)
{
    std::string source = "+ - * / % = == != < <= > >= & | && || ! ~ ++ -- << >> -> .";
    Lexer lexer(source);

    std::vector<TokenPtr> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::OP_PLUS, TokenType::OP_MINUS, TokenType::OP_MULTIPLY, TokenType::OP_DIVIDE,
        TokenType::OP_MODULO, TokenType::OP_ASSIGN, TokenType::OP_EQUAL, TokenType::OP_NOT_EQUAL,
        TokenType::OP_LESS, TokenType::OP_LESS_EQUAL, TokenType::OP_GREATER, TokenType::OP_GREATER_EQUAL,
        TokenType::OP_AND, TokenType::OP_OR, TokenType::OP_LOGICAL_AND, TokenType::OP_LOGICAL_OR,
        TokenType::OP_NOT, TokenType::OP_BITWISE_NOT, TokenType::OP_INCREMENT, TokenType::OP_DECREMENT,
        TokenType::OP_LEFT_SHIFT, TokenType::OP_RIGHT_SHIFT, TokenType::OP_ARROW, TokenType::OP_DOT};

    assertTokenTypes(tokens, expectedTypes);
}

// Test punctuation
TEST(LexerTest, Punctuation)
{
    std::string source = "( ) { } [ ] ; : ,";
    Lexer lexer(source);

    std::vector<TokenPtr> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::LPAREN, TokenType::RPAREN, TokenType::LBRACE, TokenType::RBRACE,
        TokenType::LBRACKET, TokenType::RBRACKET, TokenType::SEMICOLON, TokenType::COLON,
        TokenType::COMMA};

    assertTokenTypes(tokens, expectedTypes);
}

// Test comments
TEST(LexerTest, Comments)
{
    std::string source = "int a; // Single line comment\n"
                         "/* Multi-line\n"
                         "   comment */\n"
                         "double b;";
    Lexer lexer(source);

    std::vector<TokenPtr> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::KW_INT, TokenType::IDENTIFIER, TokenType::SEMICOLON,
        TokenType::KW_DOUBLE, TokenType::IDENTIFIER, TokenType::SEMICOLON};

    std::vector<std::string> expectedLexemes = {
        "int", "a", ";", "double", "b", ";"};

    assertTokenTypes(tokens, expectedTypes);
    assertTokenLexemes(tokens, expectedLexemes);
}

// Test source locations
TEST(LexerTest, SourceLocations)
{
    std::string source = "int a;\ndouble b;";
    Lexer lexer(source, "test.c");

    std::vector<TokenPtr> tokens = lexer.tokenize();

    // int is at line 1, column 1
    EXPECT_EQ(tokens[0]->getLocation().line, 1);
    EXPECT_EQ(tokens[0]->getLocation().column, 1);

    // double is at line 2, column 1
    EXPECT_EQ(tokens[3]->getLocation().line, 2);
    EXPECT_EQ(tokens[3]->getLocation().column, 1);
}

// Test error handling
TEST(LexerTest, ErrorHandling)
{
    // Invalid character
    {
        std::string source = "int a; # double b;";
        Lexer lexer(source);

        EXPECT_THROW({
            try {
                lexer.tokenize();
            } catch (const LexerError& e) {
                EXPECT_NE(std::string(e.what()).find("Unexpected character"), std::string::npos);
                throw;
            } }, LexerError);
    }

    // Unterminated string
    {
        std::string source = "\"unterminated string";
        Lexer lexer(source);

        EXPECT_THROW({
            try {
                lexer.tokenize();
            } catch (const LexerError& e) {
                EXPECT_NE(std::string(e.what()).find("Unterminated string"), std::string::npos);
                throw;
            } }, LexerError);
    }

    // Unterminated character
    {
        std::string source = "'";
        Lexer lexer(source);

        EXPECT_THROW({
            try {
                lexer.tokenize();
            } catch (const LexerError& e) {
                EXPECT_NE(std::string(e.what()).find("Unterminated character"), std::string::npos);
                throw;
            } }, LexerError);
    }

    // Invalid escape sequence
    {
        std::string source = "'\\z'";
        Lexer lexer(source);

        EXPECT_THROW({
            try {
                lexer.tokenize();
            } catch (const LexerError& e) {
                EXPECT_NE(std::string(e.what()).find("Invalid escape sequence"), std::string::npos);
                throw;
            } }, LexerError);
    }

    // Unterminated comment
    {
        std::string source = "/* unterminated comment";
        Lexer lexer(source);

        EXPECT_THROW({
            try {
                lexer.tokenize();
            } catch (const LexerError& e) {
                EXPECT_NE(std::string(e.what()).find("Unclosed multi-line comment"), std::string::npos);
                throw;
            } }, LexerError);
    }
}

// Test complex source
TEST(LexerTest, ComplexSource)
{
    std::string source =
        "int main() {\n"
        "    int a = 10;\n"
        "    double b = 20.5;\n"
        "    if (a < b) {\n"
        "        return 1;\n"
        "    } else {\n"
        "        return 0;\n"
        "    }\n"
        "}";

    Lexer lexer(source);
    std::vector<TokenPtr> tokens = lexer.tokenize();

    std::vector<TokenType> expectedTypes = {
        TokenType::KW_INT, TokenType::IDENTIFIER, TokenType::LPAREN, TokenType::RPAREN, TokenType::LBRACE,
        TokenType::KW_INT, TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::INTEGER_LITERAL, TokenType::SEMICOLON,
        TokenType::KW_DOUBLE, TokenType::IDENTIFIER, TokenType::OP_ASSIGN, TokenType::DOUBLE_LITERAL, TokenType::SEMICOLON,
        TokenType::KW_IF, TokenType::LPAREN, TokenType::IDENTIFIER, TokenType::OP_LESS, TokenType::IDENTIFIER, TokenType::RPAREN, TokenType::LBRACE,
        TokenType::KW_RETURN, TokenType::INTEGER_LITERAL, TokenType::SEMICOLON,
        TokenType::RBRACE, TokenType::KW_ELSE, TokenType::LBRACE,
        TokenType::KW_RETURN, TokenType::INTEGER_LITERAL, TokenType::SEMICOLON,
        TokenType::RBRACE,
        TokenType::RBRACE};

    assertTokenTypes(tokens, expectedTypes);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}