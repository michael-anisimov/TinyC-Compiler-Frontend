# TinyC Compiler Frontend - Bachelor's Thesis

**Student:** Mykhailo Anisimov  
**Supervisor:** Ing. Petr Máj, Ph.D  
**Department:** Department of Computer Science  
**University:** Czech Technical University in Prague  
**Academic Year:** 2025

## Overview

This repository contains the implementation and documentation for my bachelor's thesis project. The project focuses on developing a universal compiler frontend for the TinyC programming language, specifically designed for use in the NI-GEN course.

## Repository Structure

- [`paper/`](paper/) - Contains the LaTeX source files for the thesis documentation
  - [Thesis PDF](paper/thesis.pdf) - The complete thesis document
  - [Assignment PDF](paper/Assignment.pdf) - Original assignment specification
- [`project/`](project/) - Implementation of the TinyC compiler frontend
  - See [project README](project/README.md) for detailed implementation information

## Project Description

The aim of the project is to design universal compiler frontend for the TinyC
programming language as used in the NI-GEN course that can be given to its students so
they can focus on the middle- and back-end work. The frontend should be implemented
in C++. It should parse the TinyC language into an abstract syntax tree whose
representation should follow established Object Oriented Programming principles. It
should be available either as a library with the AST classes directly usable by students, or
as a standalone executable that will output the parsed AST in a standardized JSON
format (including source location information).

The thesis covers:

1. Analysis of the landscape of language parsers and language agnostic AST
   representations (such as babel/parser for JavaScript)
2. Design and documentation of AST representation for TinyC and its JSON format
3. Design, documentation, implementation and testing of the TinyC parser
4. Discussion of further development of the project
