# TinyC Compiler Frontend: Thesis Structure

## 1. Introduction

- **1.1 Problem Statement & Motivation**: Why students need a reliable TinyC frontend
- **1.2 Project Goals**: Clear enumeration of objectives (parser, AST representation, etc.)
- **1.3 TinyC Language Overview**: Brief introduction to TinyC and its educational purpose
- **1.4 Thesis Structure**: Brief overview of remaining chapters

## 2. Background & State of the Art

- **2.1 Compiler Architecture**: Overview of compiler components (frontend, middle-end, backend)
- **2.2 Parsing Techniques**: Discussion of parsing approaches (LL, LR, recursive descent, etc.)
- **2.3 AST Representations**: Review of different AST designs in modern compilers
- **2.4 Parser Generators vs. Handwritten Parsers**: Analysis of tradeoffs
- **2.5 Existing Tools**: Review of tools like LLVM, ANTLR, Bison, etc.
- **2.6 Quality Metrics for Parsers**: Exploring metrics for parser quality (performance, error recovery, etc.)

## 3. Design

- **3.1 Requirements Analysis**: Detailed functional and non-functional requirements
- **3.2 Architecture Overview**: High-level design of the complete frontend
- **3.3 Lexical Analysis Design**: Token design and scanner approach
- **3.4 AST Class Hierarchy**: Detailed design of AST nodes following OOP principles
- **3.5 JSON Representation**: Design of the JSON format for serializing the AST
- **3.6 Error Handling Strategy**: Approach to syntax errors and recovery
- **3.7 API Design**: Interface design for either library or standalone use

## 4. Implementation

- **4.1 Development Environment & Tools**: C++ version, libraries, build system
- **4.2 Lexer Implementation**: Implementation details of the scanner
- **4.3 Parser Implementation**: Details of the recursive descent parser implementation
- **4.4 AST Construction**: How parse rules map to AST creation
- **4.5 JSON Serialization**: Implementation of the AST to JSON conversion
- **4.6 Error Reporting**: Implementation of error detection and recovery
- **4.7 Testing Framework**: Unit testing approach and framework

## 5. Evaluation

- **5.1 Testing Methodology**: Approach to validation and verification
- **5.2 Test Coverage Analysis**: Metrics on test coverage
- **5.3 Comparison with Existing Solutions**: Comparison with current parsers used in the course
- **5.4 Performance Metrics**: Speed and memory usage benchmarks
- **5.5 Code Quality Metrics**: Analysis of code maintainability, complexity
- **5.6 Educational Value**: Evaluation of how well it serves educational purposes

## 6. Conclusion & Future Work

- **6.1 Summary of Contributions**: Review of achievements
- **6.2 Limitations**: Current limitations of the implementation
- **6.3 Future Developments**: Potential extensions:
    - Semantic analysis (type checking, symbol resolution)
    - Middle-end integration (intermediate representation)
    - Back-end connection
    - Performance optimizations

## Appendices

- **A. Grammar Specification**: Complete formal grammar for TinyC
- **B. API Documentation**: Detailed API reference
- **C. JSON Schema**: Schema specification for the AST JSON format
- **D. Example Programs & ASTs**: Sample TinyC programs and their AST representations
