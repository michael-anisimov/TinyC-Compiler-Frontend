# TinyC AST JSON Documentation

This document describes the JSON format used to represent the Abstract Syntax Tree (AST) for the TinyC language.

## Overview

The TinyC AST is serialized as a JSON object with a hierarchical structure that represents the parsed source code. The top-level object is a `Program` node containing an array of declarations. Each node in the AST has at least a `nodeType` property and a `location` property.

## JSON Schema

The structure of the TinyC AST is formally defined in the accompanying `tinyc-ast-schema.json` file, which is a [JSON Schema](https://json-schema.org/) document. This schema can be used to validate AST JSON documents and provides a formal specification of the expected format.

## Node Types

All nodes in the AST extend from a base `Node` type, which includes common properties:

### Common Node Properties

- `nodeType`: A string indicating the type of node (e.g., "Program", "VariableDeclaration", "Identifier")
- `location`: An object with source location information
  - `filename`: Source file name
  - `line`: Line number (1-based, 0 for whole file)
  - `column`: Column number (1-based, 0 for whole file)

### Node Categories

The AST nodes are divided into the following categories:

1. **Program**: The root node representing the entire program
2. **Declaration**: Nodes that declare identifiers (variables, functions, structs, etc.)
3. **Type**: Nodes representing types (primitive, named, pointer)
4. **Expression**: Nodes that evaluate to a value
5. **Statement**: Nodes that represent actions to perform

## Node Specifications

### Program

The program node is the root of the AST and represents an entire TinyC program.

```json
{
  "nodeType": "Program",
  "declarations": [
    // Array of top-level declarations
  ],
  "location": { "filename": "example.c", "line": 0, "column": 0 }
}
```

### Declaration Nodes

#### VariableDeclaration

Represents a variable declaration, including global variables and struct fields.

```json
{
  "nodeType": "VariableDeclaration",
  "identifier": "x",
  "type": { /* Type node */ },
  "arraySize": { /* Expression node (optional) */ },
  "initializer": { /* Expression node (optional) */ },
  "location": { /* Source location */ }
}
```

#### Parameter

Represents a function parameter.

```json
{
  "nodeType": "Parameter",
  "identifier": "param",
  "type": { /* Type node */ },
  "location": { /* Source location */ }
}
```

#### FunctionDeclaration / FunctionDefinition

Represents a function declaration or definition. The `nodeType` will be "FunctionDeclaration" for declarations without a body and "FunctionDefinition" for definitions with a body.

```json
{
  "nodeType": "FunctionDefinition",  // or "FunctionDeclaration"
  "identifier": "main",
  "returnType": { /* Type node */ },
  "parameters": [
    // Array of Parameter nodes
  ],
  "body": { /* BlockStatement (null for declarations) */ },
  "location": { /* Source location */ }
}
```

#### StructDeclaration / StructDefinition

Represents a struct declaration or definition. The `nodeType` will be "StructDeclaration" for forward declarations and "StructDefinition" for definitions with fields.

```json
{
  "nodeType": "StructDefinition",  // or "StructDeclaration"
  "identifier": "Point",
  "fields": [
    // Array of VariableDeclaration nodes (empty for forward declarations)
  ],
  "location": { /* Source location */ }
}
```

#### FunctionPointerDeclaration

Represents a function pointer type declaration.

```json
{
  "nodeType": "FunctionPointerDeclaration",
  "identifier": "Callback",
  "returnType": { /* Type node */ },
  "parameterTypes": [
    // Array of Type nodes
  ],
  "location": { /* Source location */ }
}
```

### Type Nodes

#### PrimitiveType

Represents a primitive type like int, double, char, or void.

```json
{
  "nodeType": "PrimitiveType",
  "kind": "int",  // or "double", "char", "void"
  "location": { /* Source location */ }
}
```

#### NamedType

Represents a named type, such as a struct or typedef name.

```json
{
  "nodeType": "NamedType",
  "identifier": "Point",  // or "struct:Point" for struct types
  "location": { /* Source location */ }
}
```

#### PointerType

Represents a pointer type.

```json
{
  "nodeType": "PointerType",
  "baseType": { /* Type node */ },
  "location": { /* Source location */ }
}
```

### Expression Nodes

#### Literal

Represents a literal value (integer, double, char, string).

```json
{
  "nodeType": "Literal",
  "kind": "integer",  // or "double", "char", "string"
  "value": "42",  // String representation of the value
  "location": { /* Source location */ }
}
```

#### Identifier

Represents a reference to a declared identifier.

```json
{
  "nodeType": "Identifier",
  "identifier": "x",
  "location": { /* Source location */ }
}
```

#### BinaryExpression

Represents a binary operation (e.g., addition, multiplication, comparison).

```json
{
  "nodeType": "BinaryExpression",
  "operator": "+",  // Operator string
  "left": { /* Expression node */ },
  "right": { /* Expression node */ },
  "location": { /* Source location */ }
}
```

#### UnaryExpression

Represents a unary operation (e.g., negation, pre/post increment/decrement).

```json
{
  "nodeType": "UnaryExpression",
  "operator": "-",  // Operator string
  "prefix": "true",  // "true" for prefix, "false" for postfix
  "operand": { /* Expression node */ },
  "location": { /* Source location */ }
}
```

#### CastExpression

Represents a type cast operation.

```json
{
  "nodeType": "CastExpression",
  "targetType": { /* Type node */ },
  "expression": { /* Expression node */ },
  "location": { /* Source location */ }
}
```

#### CallExpression

Represents a function call.

```json
{
  "nodeType": "CallExpression",
  "callee": { /* Expression node */ },
  "arguments": [
    // Array of Expression nodes
  ],
  "location": { /* Source location */ }
}
```

#### IndexExpression

Represents an array index operation.

```json
{
  "nodeType": "IndexExpression",
  "array": { /* Expression node */ },
  "index": { /* Expression node */ },
  "location": { /* Source location */ }
}
```

#### MemberExpression

Represents a struct member access operation.

```json
{
  "nodeType": "MemberExpression",
  "kind": "dot",  // "dot" for direct access, "arrow" for pointer access
  "object": { /* Expression node */ },
  "member": "x",  // Member name
  "location": { /* Source location */ }
}
```

#### CommaExpression

Represents a comma-separated list of expressions.

```json
{
  "nodeType": "CommaExpression",
  "expressions": [
    // Array of Expression nodes
  ],
  "location": { /* Source location */ }
}
```

### Statement Nodes

#### BlockStatement

Represents a block of statements enclosed in curly braces.

```json
{
  "nodeType": "BlockStatement",
  "statements": [
    // Array of Statement nodes
  ],
  "location": { /* Source location */ }
}
```

#### ExpressionStatement

Represents an expression used as a statement.

```json
{
  "nodeType": "ExpressionStatement",
  "expression": { /* Expression node */ },
  "location": { /* Source location */ }
}
```

#### IfStatement

Represents an if statement with optional else clause.

```json
{
  "nodeType": "IfStatement",
  "condition": { /* Expression node */ },
  "thenBranch": { /* Statement node */ },
  "elseBranch": { /* Statement node (optional) */ },
  "location": { /* Source location */ }
}
```

#### WhileStatement

Represents a while loop.

```json
{
  "nodeType": "WhileStatement",
  "condition": { /* Expression node */ },
  "body": { /* Statement node */ },
  "location": { /* Source location */ }
}
```

#### DoWhileStatement

Represents a do-while loop.

```json
{
  "nodeType": "DoWhileStatement",
  "body": { /* Statement node */ },
  "condition": { /* Expression node */ },
  "location": { /* Source location */ }
}
```

#### ForStatement

Represents a for loop.

```json
{
  "nodeType": "ForStatement",
  "initialization": { /* Expression or Declaration node (optional) */ },
  "condition": { /* Expression node (optional) */ },
  "update": { /* Expression node (optional) */ },
  "body": { /* Statement node */ },
  "location": { /* Source location */ }
}
```

#### SwitchStatement

Represents a switch statement.

```json
{
  "nodeType": "SwitchStatement",
  "expression": { /* Expression node */ },
  "cases": [
    {
      "isDefault": false,
      "value": 1