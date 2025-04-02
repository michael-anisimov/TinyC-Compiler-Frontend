#ifndef TINYC_AST_DUMP_VISITOR_H
#define TINYC_AST_DUMP_VISITOR_H

#include "tinyc/ast/NodeVisitor.h"
#include "tinyc/ast/ASTNode.h"
#include <ostream>
#include <string>

namespace tinyc::ast {

/**
 * @brief Visitor for dumping AST nodes to an output stream
 * 
 * This visitor implements the dump (into console) functionality for all node types.
 */
	class DumpVisitor : public NodeVisitor {
	public:
		/**
		 * @brief Construct a new Dump Visitor
		 *
		 * @param os The output stream to dump to
		 */
		explicit DumpVisitor(std::ostream &os);

		// Program nodes
		void visit(const ProgramNode &node) override;

		// Declaration nodes
		void visit(const VariableNode &node) override;

		void visit(const MultipleDeclarationNode &node) override;

		void visit(const ParameterNode &node) override;

		void visit(const FunctionDeclarationNode &node) override;

		void visit(const StructDeclarationNode &node) override;

		void visit(const FunctionPointerDeclarationNode &node) override;

		// Type nodes
		void visit(const PrimitiveTypeNode &node) override;

		void visit(const NamedTypeNode &node) override;

		void visit(const PointerTypeNode &node) override;

		// Expression nodes
		void visit(const LiteralNode &node) override;

		void visit(const IdentifierNode &node) override;

		void visit(const BinaryExpressionNode &node) override;

		void visit(const UnaryExpressionNode &node) override;

		void visit(const CastExpressionNode &node) override;

		void visit(const CallExpressionNode &node) override;

		void visit(const IndexExpressionNode &node) override;

		void visit(const MemberExpressionNode &node) override;

		void visit(const CommaExpressionNode &node) override;

		// Statement nodes
		void visit(const BlockStatementNode &node) override;

		void visit(const ExpressionStatementNode &node) override;

		void visit(const IfStatementNode &node) override;

		void visit(const WhileStatementNode &node) override;

		void visit(const DoWhileStatementNode &node) override;

		void visit(const ForStatementNode &node) override;

		void visit(const SwitchStatementNode &node) override;

		void visit(const BreakStatementNode &node) override;

		void visit(const ContinueStatementNode &node) override;

		void visit(const ReturnStatementNode &node) override;

		void visit(const ErrorProgramNode &node) override;

	private:
		std::ostream &os;
		int indentLevel = 0;

		/**
		 * @brief Get the current indentation string
		 */
		[[nodiscard]] std::string getIndent() const;

		/**
		 * @brief Increase the indentation level
		 */
		void increaseIndent();

		/**
		 * @brief Decrease the indentation level
		 */
		void decreaseIndent();

		/**
		 * @brief Helper to dump a child node with increased indentation
		 */
		void dumpChild(const ASTNode &node);
	};

} // namespace tinyc::ast

#endif // TINYC_AST_DUMP_VISITOR_H