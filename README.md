# TinyC Compiler Frontend

**Student:** Mykhailo Anisimov

**Supervisor:** Ing. Petr Máj, Ph.D

### First fortnight tasks

1. Conversion of [Initial Grammar](project/grammar/Grammar_reference.md) into its [LL(1) form](project/grammar/Grammar_LL1.txt).
   The syntax used is suited for [LL(1) Parsing Table](https://pages.fit.cvut.cz/peckato1/parsingtbl)
2. The test file with TinyC code can be found [here](project/grammar/LL1%20tests.txt). You can put it into the [LL(1) Parsing Table](https://pages.fit.cvut.cz/peckato1/parsingtbl) to check if it's working.
3. I put the LL(1) grammar into the [thesis template](paper/thesis.pdf)

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
