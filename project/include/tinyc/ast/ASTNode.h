#ifndef TINYC_AST_NODE_H
#define TINYC_AST_NODE_H

#include "tinyc/lexer/Token.h"
#include "tinyc/ast/NodeVisitor.h"
#include <memory>
#include <vector>
#include <string>
#include <ostream>


namespace tinyc::ast {

	/**
	 * @brief Base class for all AST nodes
	 */
	class ASTNode {
	public:
		explicit ASTNode(lexer::SourceLocation location);

		virtual ~ASTNode() = default;

		/**
		 * @brief Get the source location of this node
		 */
		[[nodiscard]] lexer::SourceLocation getLocation() const;

		/**
		 * @brief Accept a visitor to this node
		 *
		 * This is the core method of the Visitor pattern. Each concrete
		 * node type overrides this method to call the appropriate visit
		 * method on the visitor.
		 *
		 * @param visitor The visitor to accept
		 */
		virtual void accept(NodeVisitor &visitor) const = 0;

	private:
		const lexer::SourceLocation location;
	};

	using ASTNodePtr = std::unique_ptr<ASTNode>;

	/* ===== Type Nodes ===== */

	/**
	 * @brief Primitive type node
	 *
	 * Represents a primitive type (int, double, char, void).
	 */
	class PrimitiveTypeNode : public ASTNode {
	public:
		enum class Kind {
			INT,
			DOUBLE,
			CHAR,
			VOID
		};

		PrimitiveTypeNode(Kind kind, lexer::SourceLocation location);

		/**
		 * @brief Get the type kind
		 */
		[[nodiscard]] Kind getKind() const;

		/**
		 * @brief Convert kind to string
		 */
		[[nodiscard]] std::string getKindString() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		Kind kind;
	};

	/**
	 * @brief Named type node
	 *
	 * Represents a user-defined type referenced by name (struct or typedef).
	 */
	class NamedTypeNode : public ASTNode {
	public:
		NamedTypeNode(std::string identifier, lexer::SourceLocation location);

		/**
		 * @brief Get the type name
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::string identifier;
	};

	/**
	 * @brief Pointer type node
	 *
	 * Represents a pointer to another type.
	 */
	class PointerTypeNode : public ASTNode {
	public:
		PointerTypeNode(ASTNodePtr baseType, lexer::SourceLocation location);

		/**
		 * @brief Get the base type
		 */
		[[nodiscard]] const ASTNodePtr &getBaseType() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr baseType;
	};

	/* ===== Expression Nodes ===== */

	/**
	 * @brief Literal expression node
	 *
	 * Represents a literal value (integer, double, character, string).
	 */
	class LiteralNode : public ASTNode {
	public:
		enum class Kind {
			INTEGER,
			DOUBLE,
			CHAR,
			STRING
		};

		LiteralNode(std::string value, Kind kind, lexer::SourceLocation location);

		/**
		 * @brief Get the literal kind
		 */
		[[nodiscard]] Kind getKind() const;

		/**
		 * @brief Get the literal value as a string
		 */
		[[nodiscard]] const std::string &getValue() const;

		/**
		 * @brief Convert kind to string
		 */
		[[nodiscard]] std::string getKindString() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		Kind kind;
		std::string value;
	};

	/**
	 * @brief Identifier expression node
	 *
	 * Represents a variable or function name.
	 */
	class IdentifierNode : public ASTNode {
	public:
		IdentifierNode(std::string identifier, lexer::SourceLocation location);

		/**
		 * @brief Get the identifier name
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::string identifier;
	};

	/**
	 * @brief Binary expression node
	 *
	 * Represents a binary expression (e.g., addition, subtraction).
	 */
	class BinaryExpressionNode : public ASTNode {
	public:
		enum class Operator {
			// Arithmetic
			ADD,      // +
			SUBTRACT, // -
			MULTIPLY, // *
			DIVIDE,   // /
			MODULO,   // %

			// Bitwise
			BITWISE_AND, // &
			BITWISE_OR,  // |
			LEFT_SHIFT,  // <<
			RIGHT_SHIFT, // >>

			// Logical
			LOGICAL_AND, // &&
			LOGICAL_OR,  // ||

			// Comparison
			EQUAL,         // ==
			NOT_EQUAL,     // !=
			LESS,          // <
			LESS_EQUAL,    // <=
			GREATER,       // >
			GREATER_EQUAL, // >=

			// Assignment
			ASSIGN // =
		};

		BinaryExpressionNode(Operator op, ASTNodePtr left, ASTNodePtr right, lexer::SourceLocation location);

		/**
		 * @brief Get the operator
		 */
		[[nodiscard]] Operator getOperator() const;

		/**
		 * @brief Convert operator to string
		 */
		[[nodiscard]] std::string getOperatorString() const;

		/**
		 * @brief Get the left operand
		 */
		[[nodiscard]] const ASTNodePtr &getLeft() const;

		/**
		 * @brief Get the right operand
		 */
		[[nodiscard]] const ASTNodePtr &getRight() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		Operator op;
		ASTNodePtr left;
		ASTNodePtr right;
	};

	/**
	 * @brief Unary expression node
	 *
	 * Represents a unary expression (e.g., negation, address-of).
	 */
	class UnaryExpressionNode : public ASTNode {
	public:
		enum class Operator {
			// Prefix
			POSITIVE,      // +
			NEGATIVE,      // -
			LOGICAL_NOT,   // !
			BITWISE_NOT,   // ~
			PRE_INCREMENT, // ++
			PRE_DECREMENT, // --
			DEREFERENCE,   // *
			ADDRESS_OF,    // &

			// Postfix
			POST_INCREMENT, // ++
			POST_DECREMENT  // --
		};

		UnaryExpressionNode(Operator op, ASTNodePtr operand, lexer::SourceLocation location);

		/**
		 * @brief Get the operator
		 */
		[[nodiscard]] Operator getOperator() const;

		/**
		 * @brief Convert operator to string
		 */
		[[nodiscard]] std::string getOperatorString() const;

		/**
		 * @brief Check if this is a prefix operator
		 */
		[[nodiscard]] bool isPrefix() const;

		/**
		 * @brief Get the operand
		 */
		[[nodiscard]] const ASTNodePtr &getOperand() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		Operator op;
		ASTNodePtr operand;
	};

	/**
	 * @brief Cast expression node
	 *
	 * Represents a cast expression (e.g., cast<int>(x)).
	 */
	class CastExpressionNode : public ASTNode {
	public:
		CastExpressionNode(ASTNodePtr targetType, ASTNodePtr expression, lexer::SourceLocation location);

		/**
		 * @brief Get the target type
		 */
		[[nodiscard]] const ASTNodePtr &getTargetType() const;

		/**
		 * @brief Get the expression to cast
		 */
		[[nodiscard]] const ASTNodePtr &getExpression() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr targetType;
		ASTNodePtr expression;
	};

	/**
	 * @brief Call expression node
	 *
	 * Represents a function call.
	 */
	class CallExpressionNode : public ASTNode {
	public:
		CallExpressionNode(ASTNodePtr callee, std::vector<ASTNodePtr> arguments, lexer::SourceLocation location);

		/**
		 * @brief Get the callee expression
		 */
		[[nodiscard]] const ASTNodePtr &getCallee() const;

		/**
		 * @brief Get the arguments
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getArguments() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr callee;
		std::vector<ASTNodePtr> arguments;
	};

	/**
	 * @brief Index expression node
	 *
	 * Represents an array indexing operation.
	 */
	class IndexExpressionNode : public ASTNode {
	public:
		IndexExpressionNode(ASTNodePtr array, ASTNodePtr index, lexer::SourceLocation location);

		/**
		 * @brief Get the array expression
		 */
		[[nodiscard]] const ASTNodePtr &getArray() const;

		/**
		 * @brief Get the index expression
		 */
		[[nodiscard]] const ASTNodePtr &getIndex() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr array;
		ASTNodePtr index;
	};

	/**
	 * @brief Member expression node
	 *
	 * Represents a member access operation (e.g., a.b or a->b).
	 */
	class MemberExpressionNode : public ASTNode {
	public:
		enum class Kind {
			DOT,  // a.b
			ARROW // a->b
		};

		MemberExpressionNode(Kind kind, ASTNodePtr object, std::string member, lexer::SourceLocation location);

		/**
		 * @brief Get the access kind
		 */
		[[nodiscard]] Kind getKind() const;

		/**
		 * @brief Get the object expression
		 */
		[[nodiscard]] const ASTNodePtr &getObject() const;

		/**
		 * @brief Get the member name
		 */
		[[nodiscard]] const std::string &getMember() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		Kind kind;
		ASTNodePtr object;
		std::string member;
	};

	/**
	 * @brief Comma expression node
	 *
	 * Represents a comma-separated list of expressions.
	 */
	class CommaExpressionNode : public ASTNode {
	public:
		CommaExpressionNode(std::vector<ASTNodePtr> expressions, lexer::SourceLocation location);

		/**
		 * @brief Get the expressions
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getExpressions() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::vector<ASTNodePtr> expressions;
	};

	/* ===== Statement Nodes ===== */

	/**
	 * @brief Block statement node
	 *
	 * Represents a block of statements enclosed in curly braces.
	 */
	class BlockStatementNode : public ASTNode {
	public:
		BlockStatementNode(std::vector<ASTNodePtr> statements, lexer::SourceLocation location);

		/**
		 * @brief Get the statements in the block
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getStatements() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::vector<ASTNodePtr> statements;
	};

	/**
	 * @brief Expression statement node
	 *
	 * Represents a statement that consists of an expression.
	 */
	class ExpressionStatementNode : public ASTNode {
	public:
		ExpressionStatementNode(ASTNodePtr expression, lexer::SourceLocation location);

		/**
		 * @brief Get the expression
		 */
		[[nodiscard]] const ASTNodePtr &getExpression() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr expression;
	};

	/**
	 * @brief If statement node
	 *
	 * Represents an if statement with a condition, then branch, and optional else branch.
	 */
	class IfStatementNode : public ASTNode {
	public:
		IfStatementNode(ASTNodePtr condition,
						ASTNodePtr thenBranch,
						ASTNodePtr elseBranch, // Optional
						lexer::SourceLocation location);

		/**
		 * @brief Get the condition expression
		 */
		[[nodiscard]] const ASTNodePtr &getCondition() const;

		/**
		 * @brief Get the then branch statement
		 */
		[[nodiscard]] const ASTNodePtr &getThenBranch() const;

		/**
		 * @brief Check if this if statement has an else branch
		 */
		[[nodiscard]] bool hasElseBranch() const;

		/**
		 * @brief Get the else branch statement (or nullptr if no else branch)
		 */
		[[nodiscard]] const ASTNodePtr &getElseBranch() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr condition;
		ASTNodePtr thenBranch;
		ASTNodePtr elseBranch; // Optional
	};

	/**
	 * @brief While statement node
	 *
	 * Represents a while loop with a condition and body.
	 */
	class WhileStatementNode : public ASTNode {
	public:
		WhileStatementNode(ASTNodePtr condition, ASTNodePtr body, lexer::SourceLocation location);

		/**
		 * @brief Get the condition expression
		 */
		[[nodiscard]] const ASTNodePtr &getCondition() const;

		/**
		 * @brief Get the loop body statement
		 */
		[[nodiscard]] const ASTNodePtr &getBody() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr condition;
		ASTNodePtr body;
	};

	/**
	 * @brief Do-while statement node
	 *
	 * Represents a do-while loop with a body and condition.
	 */
	class DoWhileStatementNode : public ASTNode {
	public:
		DoWhileStatementNode(ASTNodePtr body, ASTNodePtr condition, lexer::SourceLocation location);

		/**
		 * @brief Get the loop body statement
		 */
		[[nodiscard]] const ASTNodePtr &getBody() const;

		/**
		 * @brief Get the condition expression
		 */
		[[nodiscard]] const ASTNodePtr &getCondition() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr body;
		ASTNodePtr condition;
	};

	/**
	 * @brief For statement node
	 *
	 * Represents a for loop with initialization, condition, update, and body.
	 */
	class ForStatementNode : public ASTNode {
	public:
		ForStatementNode(
				ASTNodePtr initialization, // Optional
				ASTNodePtr condition,      // Optional
				ASTNodePtr update,         // Optional
				ASTNodePtr body,
				lexer::SourceLocation location);

		/**
		 * @brief Check if this for loop has an initialization
		 */
		[[nodiscard]] bool hasInitialization() const;

		/**
		 * @brief Get the initialization expression or declaration (or nullptr if none)
		 */
		[[nodiscard]] const ASTNodePtr &getInitialization() const;

		/**
		 * @brief Check if this for loop has a condition
		 */
		[[nodiscard]] bool hasCondition() const;

		/**
		 * @brief Get the condition expression (or nullptr if none)
		 */
		[[nodiscard]] const ASTNodePtr &getCondition() const;

		/**
		 * @brief Check if this for loop has an update expression
		 */
		[[nodiscard]] bool hasUpdate() const;

		/**
		 * @brief Get the update expression (or nullptr if none)
		 */
		[[nodiscard]] const ASTNodePtr &getUpdate() const;

		/**
		 * @brief Get the loop body statement
		 */
		[[nodiscard]] const ASTNodePtr &getBody() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr initialization; // Optional
		ASTNodePtr condition;      // Optional
		ASTNodePtr update;         // Optional
		ASTNodePtr body;
	};

	/**
	 * @brief Switch statement node
	 *
	 * Represents a switch statement with an expression and cases.
	 */
	class SwitchStatementNode : public ASTNode {
	public:
		/**
		 * @brief Case in a switch statement
		 */
		struct Case {
			int value;                    // Case value (for regular cases)
			bool isDefault;               // Whether this is the default case
			std::vector<ASTNodePtr> body; // Case body statements
		};

		SwitchStatementNode(ASTNodePtr expression, std::vector<Case> cases, lexer::SourceLocation location);

		/**
		 * @brief Get the switch expression
		 */
		[[nodiscard]] const ASTNodePtr &getExpression() const;

		/**
		 * @brief Get the switch cases
		 */
		[[nodiscard]] const std::vector<Case> &getCases() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr expression;
		std::vector<Case> cases;
	};

	/**
	 * @brief Break statement node
	 */
	class BreakStatementNode : public ASTNode {
	public:
		explicit BreakStatementNode(lexer::SourceLocation location) : ASTNode(std::move(location)) {}

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}
	};

	/**
	 * @brief Continue statement node
	 */
	class ContinueStatementNode : public ASTNode {
	public:
		explicit ContinueStatementNode(lexer::SourceLocation location) : ASTNode(std::move(location)) {}

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}
	};

	/**
	 * @brief Return statement node
	 *
	 * Represents a return statement with an optional expression.
	 */
	class ReturnStatementNode : public ASTNode {
	public:
		ReturnStatementNode(
				ASTNodePtr expression, // Optional
				lexer::SourceLocation location);

		/**
		 * @brief Check if this return statement has a value
		 */
		[[nodiscard]] bool hasValue() const;

		/**
		 * @brief Get the return value expression (or nullptr if void return)
		 */
		[[nodiscard]] const ASTNodePtr &getExpression() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr expression; // Optional
	};

	/* ===== Declaration Nodes ===== */

	/**
	 * @brief Variable declaration node
	 *
	 * Represents a variable declaration or definition, which has a type,
	 * identifier, optional array size, and optional initializer.
	 */
	class VariableNode : public ASTNode {
	public:
		VariableNode(
				std::string identifier,
				ASTNodePtr type,
				lexer::SourceLocation location,
				ASTNodePtr arraySize = nullptr,
				ASTNodePtr initializer = nullptr)
				: ASTNode(std::move(location)),
				  identifier(std::move(identifier)),
				  type(std::move(type)),
				  arraySize(std::move(arraySize)),
				  initializer(std::move(initializer)) {}

		[[nodiscard]] const std::string &getIdentifier() const {
			return identifier;
		}

		/**
		 * @brief Get the variable type
		 */
		[[nodiscard]] const ASTNodePtr &getType() const;

		/**
		 * @brief Check if this is an array declaration
		 */
		[[nodiscard]] bool isArray() const;

		/**
		 * @brief Get the array size expression (or nullptr if not an array)
		 */
		[[nodiscard]] const ASTNodePtr &getArraySize() const;

		/**
		 * @brief Check if this declaration has an initializer
		 */
		[[nodiscard]] bool hasInitializer() const;

		/**
		 * @brief Get the initializer expression (or nullptr if no initializer)
		 */
		[[nodiscard]] const ASTNodePtr &getInitializer() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::string identifier;
		ASTNodePtr type;
		ASTNodePtr arraySize;   // Optional array size expression
		ASTNodePtr initializer; // Optional initializer expression
	};

	class MultipleDeclarationNode : public ASTNode {
	public:
		MultipleDeclarationNode(std::vector<ASTNodePtr> declarations,lexer::SourceLocation location)
				: ASTNode(std::move(location)), declarations(std::move(declarations)) {}

		[[nodiscard]] const std::vector<ASTNodePtr>& getDeclarations() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::vector<ASTNodePtr> declarations;
	};


	/**
	 * @brief Function parameter node
	 *
	 * Represents a function parameter, which has a type and name.
	 */
	class ParameterNode : public ASTNode {
	public:
		ParameterNode(std::string identifier, ASTNodePtr type,lexer::SourceLocation location) :
						ASTNode(std::move(location)),
						identifier(std::move(identifier)),
						type(std::move(type)) {}

		/**
		 * @brief Get the parameter type
		 */
		[[nodiscard]] const ASTNodePtr &getType() const;

		/**
		 * @brief Get the parameter identifier
		 */
		[[nodiscard]] const std::string &getIdentifier() const {
			return identifier;
		}

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		const std::string identifier;
		ASTNodePtr type;
	};

	/**
	 * @brief Function declaration node
	 *
	 * Represents a function declaration or definition, which has a return type,
	 * name, parameters, and an optional body.
	 */
	class FunctionDeclarationNode : public ASTNode {
	public:
		FunctionDeclarationNode(
				ASTNodePtr returnType,
				std::string identifier,
				std::vector<ASTNodePtr> parameters,
				ASTNodePtr body, // Optional for forward declarations
				lexer::SourceLocation location);

		/**
		 * @brief Get the return type
		 */
		[[nodiscard]] const ASTNodePtr &getReturnType() const;

		/**
		 * @brief Get the function identifier
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Get the parameters
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getParameters() const;

		/**
		 * @brief Check if this is a function definition (has a body)
		 */
		[[nodiscard]] bool isDefinition() const;

		/**
		 * @brief Get the function body (or nullptr if declaration only)
		 */
		[[nodiscard]] const ASTNodePtr &getBody() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr returnType;
		std::string identifier;
		std::vector<ASTNodePtr> parameters;
		ASTNodePtr body; // Optional for forward declarations
	};

	/**
	 * @brief Struct declaration node
	 *
	 * Represents a struct declaration, which has a name and optional fields.
	 */
	class StructDeclarationNode : public ASTNode {
	public:
		StructDeclarationNode(
				std::string identifier,
				std::vector<ASTNodePtr> fields, // Empty for forward declarations
				lexer::SourceLocation location);

		/**
		 * @brief Get the struct identifier
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Check if this is a struct definition (has fields)
		 */
		[[nodiscard]] bool isDefinition() const;

		/**
		 * @brief Set the struct fields (converting a forward declaration to a definition)
		 */
		void setFields(std::vector<ASTNodePtr> newFields) {
			fields = std::move(newFields);
		}

		/**
		 * @brief Get the struct fields
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getFields() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::string identifier;
		std::vector<ASTNodePtr> fields; // Empty for forward declarations
	};

/**
 * @brief Function pointer type declaration node
 *
 * Represents a function pointer type declaration, which has a return type,
 * name, and parameter types.
 */
	class FunctionPointerDeclarationNode : public ASTNode {
	public:
		FunctionPointerDeclarationNode(
				ASTNodePtr returnType,
				std::string identifier,
				std::vector<ASTNodePtr> parameterTypes,
				lexer::SourceLocation location);

		/**
		 * @brief Get the return type
		 */
		[[nodiscard]] const ASTNodePtr &getReturnType() const;

		/**
		 * @brief Get the function pointer identifier
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Get the parameter types
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getParameterTypes() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		ASTNodePtr returnType;
		std::string identifier;
		std::vector<ASTNodePtr> parameterTypes;
	};

/**
 * @brief Program node (root of the AST)
 *
 * Represents an entire TinyC program, which is a collection of
 * declarations (functions, variables, structs, function pointers).
 */
	class ProgramNode : public ASTNode {
	public:
		/**
		 * @brief Construct a new Program Node
		 */
		explicit ProgramNode(std::string sourceName);

		/**
		 * @brief Add a declaration to the program
		 */
		void addDeclaration(ASTNodePtr declaration);

		/**
		 * @brief Get all declarations
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getDeclarations() const;

		/**
		 * @brief Accept a visitor
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::vector<ASTNodePtr> declarations;
	};

} // namespace tinyc::ast


#endif // TINYC_AST_NODE_H