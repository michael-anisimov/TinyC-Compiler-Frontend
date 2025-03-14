# TinyC Compiler Frontend

**Student:** Mykhailo Anisimov

**Supervisor:** Ing. Petr Máj, Ph.D

### Second fortnight tasks

1. Lexer, Tokens
2. Unit tests for Lexer
3. Location info about each node
4. Interactive Lexer in console for development
5. AST nodes, includes with comments for future library
6. Recursive descent parser, dump function for development, which outputs AST in console
7. Some tests for LL(1) parser
8. Paper structure

## Description

Assignment can be found [here](paper/Assignment.pdf).

The aim of the project is to design universal compiler frontend for the TinyC
programming language as used in the NI-GEN course that can be given to its students so
they can focus on the middle- and back-end work. The frontend should be implemented
in C++. It should parse the TinyC language into an abstract syntax tree whose
representation should follow established Object Oriented Programming principles. It
should be available either as a library with the AST classes directly usable by students, or
as a standalone executable that will output the parsed AST in a standardized JSON
format (including source location information).
The thesis should:

1. Analyze the landscape of language parsers and language agnostic AST
   representations (such as babel/parser for JavaScript)
2. Design and document AST representation for TinyC and its JSON format.
3. Design, document, implement and test the TinyC parser.
4. Discuss further development of the project.
