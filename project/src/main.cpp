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

void runParserMode(const std::string &source, const std::string &filename, bool prettyPrint) {
	// Create lexer
	Lexer lexer(source, filename);

	// Create parser and parse
	Parser parser(lexer);
	auto ast = parser.parseProgram();

	// Create JSON visitor with pretty print option
	JSONVisitor jsonVisitor(prettyPrint);
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

	if (parserMode) {
		std::cout << "Pretty print JSON? (y/n, default is 'n'): ";
		std::string prettyPrintInput;
		std::getline(std::cin, prettyPrintInput);
		bool prettyPrint = (prettyPrintInput == "y" || prettyPrintInput == "yes" ||
							prettyPrintInput == "Y" || prettyPrintInput == "Yes");

		std::cout << "Pretty printing " << (prettyPrint ? "enabled" : "disabled") << std::endl;
	}

	std::cout << "Enter TinyC code (type 'exit' to quit):" << std::endl;

	std::string line;
	std::string source;
	int lineNumber = 1;
	bool prettyPrint = false;

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
			runParserMode(source, "<interactive>", prettyPrint);
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

void printUsage(const char* programName) {
	std::cerr << "Usage: " << programName << " [--lex|-l|--parse|-p] [--pretty|-pp] <source_file>" << std::endl;
	std::cerr << "       Run without arguments for interactive mode." << std::endl;
	std::cerr << "Options:" << std::endl;
	std::cerr << "  --lex, -l       Run in lexer mode (output tokens)" << std::endl;
	std::cerr << "  --parse, -p     Run in parser mode (output AST as JSON)" << std::endl;
	std::cerr << "  --pretty, -pp   Pretty print JSON output (only with parser mode)" << std::endl;
}

int main(int argc, char *argv[]) {
	try {
		if (argc == 1) {
			// No arguments, run in interactive mode
			runInteractiveMode();
			return 0;
		} else {
			// Parse command line arguments
			std::string mode;
			std::string filename;
			bool prettyPrint = false;

			// Process all arguments
			for (int i = 1; i < argc; i++) {
				std::string arg = argv[i];

				if (arg == "--lex" || arg == "-l") {
					if (!mode.empty()) {
						std::cerr << "Error: Multiple mode options specified" << std::endl;
						printUsage(argv[0]);
						return 1;
					}
					mode = "--lex";
				} else if (arg == "--parse" || arg == "-p") {
					if (!mode.empty()) {
						std::cerr << "Error: Multiple mode options specified" << std::endl;
						printUsage(argv[0]);
						return 1;
					}
					mode = "--parse";
				} else if (arg == "--pretty" || arg == "-pp") {
					prettyPrint = true;
				} else if (arg[0] == '-') {
					std::cerr << "Unknown option: " << arg << std::endl;
					printUsage(argv[0]);
					return 1;
				} else {
					// Must be the filename
					if (!filename.empty()) {
						std::cerr << "Error: Multiple filenames specified" << std::endl;
						printUsage(argv[0]);
						return 1;
					}
					filename = arg;
				}
			}

			// Check if we have a filename
			if (filename.empty()) {
				std::cerr << "Error: No source file specified" << std::endl;
				printUsage(argv[0]);
				return 1;
			}

			// Default to parser mode if not specified
			if (mode.empty()) {
				mode = "--parse";
			}

			// Read the source file
			std::string source = readFile(filename);

			// Run in the selected mode
			if (mode == "--lex") {
				if (prettyPrint) {
					std::cerr << "Warning: Pretty print option is ignored in lexer mode" << std::endl;
				}
				runLexerMode(source, filename);
			} else if (mode == "--parse") {
				runParserMode(source, filename, prettyPrint);
			}

			return 0;
		}
	}
	catch (const LexerError &e) {
		std::cerr << "Lexer error: " << e.what() << std::endl;
		return 1;
	}
	catch (const ParserError &e) {
		std::cerr << "Parser error: " << e.what() << std::endl;
		return 2;
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 3;
	}
}