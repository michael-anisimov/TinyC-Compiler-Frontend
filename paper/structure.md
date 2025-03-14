# TinyC Compiler Frontend: Thesis Structure

## 1. Introduction

- **1.1 Problem Statement & Motivation**: Why students need a reliable TinyC frontend
- **1.2 Project Goals & Objectives**: Clear enumeration of deliverables
- **1.3 TinyC Language Overview**: Brief introduction to TinyC and its educational purpose
- **1.4 Target Audience**: Discussion of how this will be used by students
- **1.5 Thesis Structure**: Overview of remaining chapters

## 2. Theoretical Background

- **2.1 Compiler Architecture Fundamentals**: Overview of compiler components
- **2.2 Lexical Analysis Theory**: Tokens, regular expressions, finite automata
- **2.3 Parsing Theory**: Context-free grammars, derivations, parse trees
    - **2.3.1 Top-down Parsing**: LL(1), recursive descent, predictive parsing
    - **2.3.2 Bottom-up Parsing**: LR, and so on
- **2.4 Abstract Syntax Trees**: Purpose, design principles, traversal techniques

## 3. Analysis of Existing Solutions

- **3.1 Parser Categories & Comparison Criteria**: Establishing evaluation framework
    - **3.1.1 Parser Generators**: ANTLR, Bison, YACC
    - **3.1.2 Hand-written Parsers**: Recursive descent, Pratt parsing
    - **3.1.3 Parser Combinators**: Approaches in functional languages
- **3.2 Language-Agnostic AST Representations**:
    - **3.2.1 JavaScript Ecosystem**: Babel/parser, ESTree, TypeScript Compiler API
    - **3.2.2 LLVM IR & Clang AST**: Structure and design philosophy
    - **3.2.3 Roslyn (C# Compiler Platform)**: .NET approach to language services
    - **3.2.4 Tree-sitter**: Universal parsing approach
- **3.3 AST Serialization Formats**:
    - **3.3.1 JSON-based AST Representations**: Benefits and limitations
    - **3.3.2 Binary Formats**: Protobuf, Flatbuffers, Cap'n Proto
    - **3.3.3 Custom DSLs**: Domain-specific notations for ASTs
- **3.4 Source Location & Error Reporting**:
    - **3.4.1 Location Tracking Techniques**: Different approaches
    - **3.4.2 Error Recovery Strategies**: How parsers handle syntax errors
- **3.5 Educational Compiler Frontends**:
    - **3.5.1 Case Studies**: Notable examples from academia
    - **3.5.2 Design Patterns**: Common patterns in educational compilers

## 4. Design Requirements & Architecture

- **4.1 Functional Requirements**: What the compiler frontend must do
- **4.2 Non-functional Requirements**: Performance, extensibility, educational value
- **4.3 TinyC Grammar Analysis**:
    - **4.3.1 Grammar Transformations**: Converting to LL(1) form
    - **4.3.2 Parsing Challenges**: Difficult aspects of TinyC grammar
- **4.4 System Architecture**: High-level components and their interactions
- **4.5 AST Design**:
    - **4.5.1 Node Class Hierarchy**: Core design decisions and OOP principles applied
    - **4.5.2 Visitor Pattern Implementation**: Design for traversal
    - **4.5.3 JSON Representation Design**: Mapping between AST and JSON
- **4.6 API Design**: How users will interact with the frontend
    - **4.6.1 Library API**: Class interfaces for direct usage
    - **4.6.2 CLI Design**: Command-line interface for standalone usage

## 5. Implementation

- **5.1 Development Environment**: Tools, libraries, and workflow
- **5.2 Lexical Analyzer Implementation**: Scanner details
- **5.3 Parser Implementation**: Parsing algorithm details
- **5.4 AST Construction**: Building the tree during parsing
- **5.5 Source Location Tracking**: Implementation details
- **5.6 Error Handling & Recovery**: How errors are detected and reported
- **5.7 JSON Serialization**: Implementation details
- **5.8 Testing Framework**: Unit and integration testing approach

## 6. Evaluation

- **6.1 Methodology**: Approach to validating the implementation
- **6.2 Test Coverage Analysis**: Metrics and results
- **6.3 Correctness Validation**: How parser correctness is verified
- **6.4 Comparison with Alternative Implementations**: Evaluating against existing parsers
    - **6.4.1 Feature Comparison**: Capabilities comparison
    - **6.4.2 Performance Analysis**: Speed and memory benchmarks
    - **6.4.3 Maintainability Metrics**: Code quality metrics
- **6.5 User Feedback**: Preliminary student or instructor feedback
- **6.6 Educational Value Assessment**: Evaluating how well it serves its purpose

## 7. Conclusion & Future Work

- **7.1 Summary of Contributions**: What was accomplished
- **7.2 Lessons Learned**: Insights gained during development
- **7.3 Limitations of Current Implementation**: Known issues or constraints
- **7.4 Future Extensions**:
    - **7.4.1 Semantic Analysis**: Type checking, name resolution
    - **7.4.2 Optimization Passes**: Middle-end potential
    - **7.4.3 Code Generation Interfaces**: Backend integration
    - **7.4.4 IDE Integration**: Developer tooling possibilities
    - **7.4.5 Language Extensions**: Potential TinyC enhancements

## Appendices

- **A. Complete TinyC Grammar**: Formal grammar specification
- **B. API Documentation**: Detailed interface reference
- **C. JSON Schema**: Schema for AST serialization
- **D. Sample Programs**: Example TinyC programs with corresponding ASTs
- **E. User Guide**: Documentation for students and instructors