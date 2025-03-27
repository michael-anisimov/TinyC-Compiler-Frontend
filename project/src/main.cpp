#include "tinyc/lexer/Lexer.h"
#include "tinyc/parser/Parser.h"
#include "tinyc/ast/visitors/DumpVisitor.h"
#include "tinyc/ast/visitors/JSONVisitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace tinyc::lexer;
using namespace tinyc::parser;
using namespace tinyc::ast;

std::string readFile(const std::string &filename) {
	std::ifstream file(filename);
	if (!file) {
		throw std::runtime_error("Could not open file: " + filename);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void printTokens(const std::vector<TokenPtr> &tokens) {
	for (const auto &token: tokens) {
		std::cout << *token << std::endl;
	}
}

void runLexerMode(const std::string &source, const std::string &filename) {
	// Create lexer and tokenize
	Lexer lexer(source, filename);
	auto tokens = lexer.tokenize();

	// Print tokens
	std::cout << "Tokens from " << filename << ":" << std::endl;
	printTokens(tokens);
}

void runParserMode(const std::string &source, const std::string &filename) {
	// Create lexer
	Lexer lexer(source, filename);

	// Create parser and parse
	Parser parser(lexer);
	auto ast = parser.parseProgram();

	// Print AST
//	std::cout << "AST from " << filename << ":" << std::endl;
//	DumpVisitor printer(std::cout);
//	ast->accept(printer);

	JSONVisitor jsonVisitor;
	ast->accept(jsonVisitor);
	std::string json = jsonVisitor.getJSON();
	std::cout << json << std::endl;
}

void runInteractiveMode() {
	std::cout << "TinyC Compiler Interactive Mode (type 'exit' to quit)" << std::endl;
	std::cout << "Select mode: 'lexer (l)' or 'parser (p)' (default is 'lexer'): ";

	std::string mode;
	std::getline(std::cin, mode);

	bool parserMode = (mode == "parser" || mode == "p");

	std::cout << "Enter TinyC code (type 'exit' to quit):" << std::endl;

	std::string line;
	std::string source;
	int lineNumber = 1;

	while (true) {
		std::cout << lineNumber << "> ";

		std::string input;
		if (!std::getline(std::cin, input) || input == "exit") {
			break;
		}

		// Add the input to the source
		source += input + "\n";

		// In lexer mode, we process each line individually
		if (!parserMode) {
			try {
				// Create lexer for current line
				Lexer lexer(input, "<interactive>");
				auto tokens = lexer.tokenize();

				// Skip printing EOF token
				tokens.pop_back();

				// Print tokens
				if (!tokens.empty()) {
					std::cout << "Tokens:" << std::endl;
					printTokens(tokens);
				}
			}
			catch (const LexerError &e) {
				std::cerr << "Lexer error: " << e.what() << std::endl;
			}
		}

		lineNumber++;
	}

	// In parser mode, we process the entire input at once
	if (parserMode && !source.empty()) {
		try {
			runParserMode(source, "<interactive>");
		}
		catch (const LexerError &e) {
			std::cerr << "Lexer error: " << e.what() << std::endl;
		}
		catch (const ParserError &e) {
			std::cerr << "Parser error: " << e.what() << std::endl;
		}
	}

	std::cout << "Exiting interactive mode." << std::endl;
}

int main(int argc, char *argv[]) {
	try {
		if (argc == 1) {
			// No arguments, run in interactive mode
			runInteractiveMode();
			return 0;
		} else if (argc == 3) {
			// Two arguments: mode and file
			std::string mode = argv[1];
			std::string filename = argv[2];
			std::string source = readFile(filename);

			if (mode == "--lex" || mode == "-l") {
				runLexerMode(source, filename);
			} else if (mode == "--parse" || mode == "-p") {
				runParserMode(source, filename);
			} else {
				std::cerr << "Unknown mode: " << mode << std::endl;
				std::cerr << "Usage: " << argv[0] << " [--lex|-l|--parse|-p] <source_file>" << std::endl;
				std::cerr << "       Run without arguments for interactive mode." << std::endl;
				return 1;
			}

			return 0;
		} else {
			std::cerr << "Usage: " << argv[0] << " [--lex|-l|--parse|-p] <source_file>" << std::endl;
			std::cerr << "       Run without arguments for interactive mode." << std::endl;
			return 1;
		}
	}
	catch (const LexerError &e) {
		std::cerr << "Lexer error: " << e.what() << std::endl;
		return 1;
	}
	catch (const ParserError &e) {
		std::cerr << "Parser error: " << e.what() << std::endl;
		return 1;
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
}