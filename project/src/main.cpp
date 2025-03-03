#include "tinyc/lexer/Lexer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace tinyc::lexer;

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void printTokens(const std::vector<TokenPtr> &tokens)
{
    for (const auto &token : tokens)
    {
        std::cout << *token << std::endl;
    }
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
            return 1;
        }

        // Read the source file
        std::string filename = argv[1];
        std::string source = readFile(filename);

        // Create lexer and tokenize
        Lexer lexer(source, filename);
        auto tokens = lexer.tokenize();

        // Print tokens
        std::cout << "Tokens from " << filename << ":" << std::endl;
        printTokens(tokens);

        return 0;
    }
    catch (const LexerError &e)
    {
        std::cerr << "Lexer error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}