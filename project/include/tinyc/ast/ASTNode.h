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
		/**
		 * @brief Construct a new AST Node
		 *
		 * @param location Source code location of this node
		 */
		explicit ASTNode(lexer::SourceLocation location);

		/**
		 * @brief Virtual destructor for proper inheritance
		 */
		virtual ~ASTNode() = default;

		/**
		 * @brief Get the source location of this node
		 *
		 * @return Source location
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

// Using a shared_ptr for node management
	using ASTNodePtr = std::unique_ptr<ASTNode>;

/* ===== Type Nodes ===== */

/**
 * @brief Primitive type node
 *
 * Represents a primitive type (int, double, char, void).
 */
	class PrimitiveTypeNode : public ASTNode {
	public:
		/**
		 * @brief Enumeration of primitive type kinds
		 */
		enum class Kind {
			INT,
			DOUBLE,
			CHAR,
			VOID
		};

		/**
		 * @brief Construct a new Primitive Type Node
		 *
		 * @param kind The primitive type kind
		 * @param location Source location
		 */
		PrimitiveTypeNode(Kind kind, lexer::SourceLocation location);

		/**
		 * @brief Get the type kind
		 *
		 * @return Kind of primitive type
		 */
		[[nodiscard]] Kind getKind() const;

		/**
		 * @brief Convert kind to string
		 *
		 * @return String representation of the kind
		 */
		[[nodiscard]] std::string getKindString() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Named Type Node
		 *
		 * @param identifier The type name
		 * @param location Source location
		 */
		NamedTypeNode(std::string identifier, lexer::SourceLocation location);

		/**
		 * @brief Get the type name
		 *
		 * @return The type identifier
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Pointer Type Node
		 *
		 * @param baseType The type being pointed to
		 * @param location Source location
		 */
		PointerTypeNode(ASTNodePtr baseType, lexer::SourceLocation location);

		/**
		 * @brief Get the base type
		 *
		 * @return The type being pointed to
		 */
		[[nodiscard]] const ASTNodePtr &getBaseType() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Enumeration of literal kinds
		 */
		enum class Kind {
			INTEGER,
			DOUBLE,
			CHAR,
			STRING
		};

		/**
		 * @brief Construct a new Literal Node
		 *
		 * @param value String representation of the literal value
		 * @param kind The literal kind
		 * @param location Source location
		 */
		LiteralNode(std::string value, Kind kind, lexer::SourceLocation location);

		/**
		 * @brief Get the literal kind
		 *
		 * @return Kind of literal
		 */
		[[nodiscard]] Kind getKind() const;

		/**
		 * @brief Get the literal value as a string
		 *
		 * @return String representation of the value
		 */
		[[nodiscard]] const std::string &getValue() const;

		/**
		 * @brief Convert kind to string
		 *
		 * @return String representation of the kind
		 */
		[[nodiscard]] std::string getKindString() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Identifier Node
		 *
		 * @param identifier The identifier name
		 * @param location Source location
		 */
		IdentifierNode(std::string identifier, lexer::SourceLocation location);

		/**
		 * @brief Get the identifier name
		 *
		 * @return The identifier string
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Enumeration of binary operators
		 */
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

		/**
		 * @brief Construct a new Binary Expression Node
		 *
		 * @param op The binary operator
		 * @param left The left operand
		 * @param right The right operand
		 * @param location Source location
		 */
		BinaryExpressionNode(Operator op, ASTNodePtr left, ASTNodePtr right, lexer::SourceLocation location);

		/**
		 * @brief Get the operator
		 *
		 * @return The binary operator
		 */
		[[nodiscard]] Operator getOperator() const;

		/**
		 * @brief Convert operator to string
		 *
		 * @return String representation of the operator
		 */
		[[nodiscard]] std::string getOperatorString() const;

		/**
		 * @brief Get the left operand
		 *
		 * @return The left operand expression
		 */
		[[nodiscard]] const ASTNodePtr &getLeft() const;

		/**
		 * @brief Get the right operand
		 *
		 * @return The right operand expression
		 */
		[[nodiscard]] const ASTNodePtr &getRight() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Enumeration of unary operators
		 */
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

		/**
		 * @brief Construct a new Unary Expression Node
		 *
		 * @param op The unary operator
		 * @param operand The operand
		 * @param location Source location
		 */
		UnaryExpressionNode(Operator op, ASTNodePtr operand, lexer::SourceLocation location);

		/**
		 * @brief Get the operator
		 *
		 * @return The unary operator
		 */
		[[nodiscard]] Operator getOperator() const;

		/**
		 * @brief Convert operator to string
		 *
		 * @return String representation of the operator
		 */
		[[nodiscard]] std::string getOperatorString() const;

		/**
		 * @brief Check if this is a prefix operator
		 *
		 * @return true if prefix, false if postfix
		 */
		[[nodiscard]] bool isPrefix() const;

		/**
		 * @brief Get the operand
		 *
		 * @return The operand expression
		 */
		[[nodiscard]] const ASTNodePtr &getOperand() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Cast Expression Node
		 *
		 * @param targetType The type to cast to
		 * @param expression The expression to cast
		 * @param location Source location
		 */
		CastExpressionNode(ASTNodePtr targetType, ASTNodePtr expression, lexer::SourceLocation location);

		/**
		 * @brief Get the target type
		 *
		 * @return The type to cast to
		 */
		[[nodiscard]] const ASTNodePtr &getTargetType() const;

		/**
		 * @brief Get the expression to cast
		 *
		 * @return The expression being cast
		 */
		[[nodiscard]] const ASTNodePtr &getExpression() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Call Expression Node
		 *
		 * @param callee Function to call
		 * @param arguments Arguments for the function call
		 * @param location Source location
		 */
		CallExpressionNode(ASTNodePtr callee, std::vector<ASTNodePtr> arguments, lexer::SourceLocation location);

		/**
		 * @brief Get the callee expression
		 *
		 * @return The function being called
		 */
		[[nodiscard]] const ASTNodePtr &getCallee() const;

		/**
		 * @brief Get the arguments
		 *
		 * @return The function call arguments
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getArguments() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Index Expression Node
		 *
		 * @param array Array or pointer to index
		 * @param index Index expression
		 * @param location Source location
		 */
		IndexExpressionNode(ASTNodePtr array, ASTNodePtr index, lexer::SourceLocation location);

		/**
		 * @brief Get the array expression
		 *
		 * @return The array being indexed
		 */
		[[nodiscard]] const ASTNodePtr &getArray() const;

		/**
		 * @brief Get the index expression
		 *
		 * @return The index expression
		 */
		[[nodiscard]] const ASTNodePtr &getIndex() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Kind of member access
		 */
		enum class Kind {
			DOT,  // a.b
			ARROW // a->b
		};

		/**
		 * @brief Construct a new Member Expression Node
		 *
		 * @param kind Kind of member access (dot or arrow)
		 * @param object Object to access
		 * @param member Name of the member to access
		 * @param location Source location
		 */
		MemberExpressionNode(Kind kind, ASTNodePtr object, std::string member, lexer::SourceLocation location);

		/**
		 * @brief Get the access kind
		 *
		 * @return The kind of member access
		 */
		[[nodiscard]] Kind getKind() const;

		/**
		 * @brief Get the object expression
		 *
		 * @return The object being accessed
		 */
		[[nodiscard]] const ASTNodePtr &getObject() const;

		/**
		 * @brief Get the member name
		 *
		 * @return The name of the accessed member
		 */
		[[nodiscard]] const std::string &getMember() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Comma Expression Node
		 *
		 * @param expressions List of expressions separated by commas
		 * @param location Source location
		 */
		CommaExpressionNode(std::vector<ASTNodePtr> expressions, lexer::SourceLocation location);

		/**
		 * @brief Get the expressions
		 *
		 * @return The list of expressions
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getExpressions() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Block Statement Node
		 *
		 * @param statements List of statements in the block
		 * @param location Source location
		 */
		BlockStatementNode(std::vector<ASTNodePtr> statements, lexer::SourceLocation location);

		/**
		 * @brief Get the statements in the block
		 *
		 * @return The list of statements
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getStatements() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Expression Statement Node
		 *
		 * @param expression The expression to execute as a statement
		 * @param location Source location
		 */
		ExpressionStatementNode(ASTNodePtr expression, lexer::SourceLocation location);

		/**
		 * @brief Get the expression
		 *
		 * @return The statement expression
		 */
		[[nodiscard]] const ASTNodePtr &getExpression() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new If Statement Node
		 *
		 * @param condition Condition to check
		 * @param thenBranch Statement to execute if condition is true
		 * @param elseBranch Statement to execute if condition is false (optional)
		 * @param location Source location
		 */
		IfStatementNode(ASTNodePtr condition,
						ASTNodePtr thenBranch,
						ASTNodePtr elseBranch, // Optional
						lexer::SourceLocation location);

		/**
		 * @brief Get the condition expression
		 *
		 * @return The condition expression
		 */
		[[nodiscard]] const ASTNodePtr &getCondition() const;

		/**
		 * @brief Get the then branch statement
		 *
		 * @return The then branch statement
		 */
		[[nodiscard]] const ASTNodePtr &getThenBranch() const;

		/**
		 * @brief Check if this if statement has an else branch
		 *
		 * @return true if has else branch, false otherwise
		 */
		[[nodiscard]] bool hasElseBranch() const;

		/**
		 * @brief Get the else branch statement
		 *
		 * @return The else branch statement or nullptr if no else branch
		 */
		[[nodiscard]] const ASTNodePtr &getElseBranch() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new While Statement Node
		 *
		 * @param condition Condition to check before each iteration
		 * @param body Statement to execute in each iteration
		 * @param location Source location
		 */
		WhileStatementNode(ASTNodePtr condition, ASTNodePtr body, lexer::SourceLocation location);

		/**
		 * @brief Get the condition expression
		 *
		 * @return The loop condition
		 */
		[[nodiscard]] const ASTNodePtr &getCondition() const;

		/**
		 * @brief Get the loop body statement
		 *
		 * @return The loop body
		 */
		[[nodiscard]] const ASTNodePtr &getBody() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Do While Statement Node
		 *
		 * @param body Statement to execute in each iteration
		 * @param condition Condition to check after each iteration
		 * @param location Source location
		 */
		DoWhileStatementNode(ASTNodePtr body, ASTNodePtr condition, lexer::SourceLocation location);

		/**
		 * @brief Get the loop body statement
		 *
		 * @return The loop body
		 */
		[[nodiscard]] const ASTNodePtr &getBody() const;

		/**
		 * @brief Get the condition expression
		 *
		 * @return The loop condition
		 */
		[[nodiscard]] const ASTNodePtr &getCondition() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new For Statement Node
		 *
		 * @param initialization Initialization expression or variable declaration (optional)
		 * @param condition Condition to check before each iteration (optional)
		 * @param update Update expression to execute after each iteration (optional)
		 * @param body Statement to execute in each iteration
		 * @param location Source location
		 */
		ForStatementNode(
				ASTNodePtr initialization, // Optional
				ASTNodePtr condition,      // Optional
				ASTNodePtr update,         // Optional
				ASTNodePtr body,
				lexer::SourceLocation location);

		/**
		 * @brief Check if this for loop has an initialization
		 *
		 * @return true if has initialization, false otherwise
		 */
		[[nodiscard]] bool hasInitialization() const;

		/**
		 * @brief Get the initialization expression or declaration
		 *
		 * @return The initialization or nullptr if none
		 */
		[[nodiscard]] const ASTNodePtr &getInitialization() const;

		/**
		 * @brief Check if this for loop has a condition
		 *
		 * @return true if has condition, false otherwise
		 */
		[[nodiscard]] bool hasCondition() const;

		/**
		 * @brief Get the condition expression
		 *
		 * @return The condition or nullptr if none
		 */
		[[nodiscard]] const ASTNodePtr &getCondition() const;

		/**
		 * @brief Check if this for loop has an update expression
		 *
		 * @return true if has update, false otherwise
		 */
		[[nodiscard]] bool hasUpdate() const;

		/**
		 * @brief Get the update expression
		 *
		 * @return The update expression or nullptr if none
		 */
		[[nodiscard]] const ASTNodePtr &getUpdate() const;

		/**
		 * @brief Get the loop body statement
		 *
		 * @return The loop body
		 */
		[[nodiscard]] const ASTNodePtr &getBody() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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

		/**
		 * @brief Construct a new Switch Statement Node
		 *
		 * @param expression Expression to switch on
		 * @param cases List of case clauses
		 * @param location Source location
		 */
		SwitchStatementNode(ASTNodePtr expression, std::vector<Case> cases, lexer::SourceLocation location);

		/**
		 * @brief Get the switch expression
		 *
		 * @return The expression being switched on
		 */
		[[nodiscard]] const ASTNodePtr &getExpression() const;

		/**
		 * @brief Get the switch cases
		 *
		 * @return The list of case clauses
		 */
		[[nodiscard]] const std::vector<Case> &getCases() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Break Statement Node
		 *
		 * @param location Source location
		 */
		explicit BreakStatementNode(lexer::SourceLocation location);

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Continue Statement Node
		 *
		 * @param location Source location
		 */
		explicit ContinueStatementNode(lexer::SourceLocation location);

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Return Statement Node
		 *
		 * @param expression Return value (optional for void functions)
		 * @param location Source location
		 */
		ReturnStatementNode(
				ASTNodePtr expression, // Optional
				lexer::SourceLocation location);

		/**
		 * @brief Check if this return statement has a value
		 *
		 * @return true if has return value, false otherwise
		 */
		[[nodiscard]] bool hasValue() const;

		/**
		 * @brief Get the return value expression
		 *
		 * @return The return value or nullptr if void return
		 */
		[[nodiscard]] const ASTNodePtr &getExpression() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Variable Node
		 *
		 * @param identifier Name of the variable
		 * @param type Type of the variable
		 * @param location Source location
		 * @param arraySize Size of the array (optional, null for non-array variables)
		 * @param initializer Initial value (optional)
		 */
		VariableNode(
				std::string identifier,
				ASTNodePtr type,
				lexer::SourceLocation location,
				ASTNodePtr arraySize = nullptr,
				ASTNodePtr initializer = nullptr);

		/**
		 * @brief Get the variable identifier
		 *
		 * @return The variable name
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Get the variable type
		 *
		 * @return The variable type
		 */
		[[nodiscard]] const ASTNodePtr &getType() const;

		/**
		 * @brief Check if this is an array declaration
		 *
		 * @return true if array, false otherwise
		 */
		[[nodiscard]] bool isArray() const;

		/**
		 * @brief Get the array size expression
		 *
		 * @return The array size or nullptr if not an array
		 */
		[[nodiscard]] const ASTNodePtr &getArraySize() const;

		/**
		 * @brief Check if this declaration has an initializer
		 *
		 * @return true if has initializer, false otherwise
		 */
		[[nodiscard]] bool hasInitializer() const;

		/**
		 * @brief Get the initializer expression
		 *
		 * @return The initializer expression or nullptr if no initializer
		 */
		[[nodiscard]] const ASTNodePtr &getInitializer() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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

/**
 * @brief Multiple declaration node
 *
 * Represents multiple declarations in a single statement.
 */
	class MultipleDeclarationNode : public ASTNode {
	public:
		/**
		 * @brief Construct a new Multiple Declaration Node
		 *
		 * @param declarations List of variable declarations
		 * @param location Source location
		 */
		MultipleDeclarationNode(std::vector<ASTNodePtr> declarations, lexer::SourceLocation location);

		/**
		 * @brief Get the declarations
		 *
		 * @return List of variable declarations
		 */
		[[nodiscard]] const std::vector<ASTNodePtr>& getDeclarations() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Parameter Node
		 *
		 * @param identifier Name of the parameter
		 * @param type Type of the parameter
		 * @param location Source location
		 */
		ParameterNode(std::string identifier, ASTNodePtr type, lexer::SourceLocation location);

		/**
		 * @brief Get the parameter type
		 *
		 * @return The parameter type
		 */
		[[nodiscard]] const ASTNodePtr &getType() const;

		/**
		 * @brief Get the parameter identifier
		 *
		 * @return The parameter name
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::string identifier;
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
		/**
		 * @brief Construct a new Function Declaration Node
		 *
		 * @param identifier Name of the function
		 * @param returnType Return type of the function
		 * @param parameters Function parameters
		 * @param body Function body (null for declarations, BlockStatement for definitions)
		 * @param location Source location
		 */
		FunctionDeclarationNode(
				std::string identifier,
				ASTNodePtr returnType,
				std::vector<ASTNodePtr> parameters,
				ASTNodePtr body, // Optional for forward declarations
				lexer::SourceLocation location);

		/**
		 * @brief Get the return type
		 *
		 * @return The function return type
		 */
		[[nodiscard]] const ASTNodePtr &getReturnType() const;

		/**
		 * @brief Get the function identifier
		 *
		 * @return The function name
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Get the parameters
		 *
		 * @return The function parameters
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getParameters() const;

		/**
		 * @brief Check if this is a function definition (has a body)
		 *
		 * @return true if definition, false if declaration only
		 */
		[[nodiscard]] bool isDefinition() const;

		/**
		 * @brief Get the function body
		 *
		 * @return The function body or nullptr if declaration only
		 */
		[[nodiscard]] const ASTNodePtr &getBody() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::string identifier;
		ASTNodePtr returnType;
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
		/**
		 * @brief Construct a new Struct Declaration Node
		 *
		 * @param identifier Name of the struct
		 * @param fields Struct fields (empty for forward declarations)
		 * @param location Source location
		 */
		StructDeclarationNode(
				std::string identifier,
				std::vector<ASTNodePtr> fields, // Empty for forward declarations
				lexer::SourceLocation location);

		/**
		 * @brief Get the struct identifier
		 *
		 * @return The struct name
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Check if this is a struct definition (has fields)
		 *
		 * @return true if definition, false if forward declaration
		 */
		[[nodiscard]] bool isDefinition() const;

		/**
		 * @brief Set the struct fields (converting a forward declaration to a definition)
		 *
		 * @param newFields The struct fields to set
		 */
		void setFields(std::vector<ASTNodePtr> newFields);

		/**
		 * @brief Get the struct fields
		 *
		 * @return The struct fields
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getFields() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
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
		/**
		 * @brief Construct a new Function Pointer Declaration Node
		 *
		 * @param identifier Name of the function pointer type
		 * @param returnType Return type of the function pointer
		 * @param parameterTypes Types of the function parameters
		 * @param location Source location
		 */
		FunctionPointerDeclarationNode(
				std::string identifier,
				ASTNodePtr returnType,
				std::vector<ASTNodePtr> parameterTypes,
				lexer::SourceLocation location);

		/**
		 * @brief Get the return type
		 *
		 * @return The function pointer return type
		 */
		[[nodiscard]] const ASTNodePtr &getReturnType() const;

		/**
		 * @brief Get the function pointer identifier
		 *
		 * @return The function pointer type name
		 */
		[[nodiscard]] const std::string &getIdentifier() const;

		/**
		 * @brief Get the parameter types
		 *
		 * @return The function parameter types
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getParameterTypes() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::string identifier;
		ASTNodePtr returnType;
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
		 *
		 * @param sourceName Name of the source file
		 */
		explicit ProgramNode(std::string sourceName);

		/**
		 * @brief Add a declaration to the program
		 *
		 * @param declaration The declaration to add
		 */
		void addDeclaration(ASTNodePtr declaration);

		/**
		 * @brief Get all declarations
		 *
		 * @return The list of top-level declarations
		 */
		[[nodiscard]] const std::vector<ASTNodePtr> &getDeclarations() const;

		/**
		 * @brief Accept a visitor
		 *
		 * @param visitor The visitor to accept
		 */
		void accept(NodeVisitor &visitor) const override {
			visitor.visit(*this);
		}

	private:
		std::vector<ASTNodePtr> declarations;
	};

} // namespace tinyc::ast

#endif // TINYC_AST_NODE_H