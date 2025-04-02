#ifndef TINYC_AST_JSON_VISITOR_H
#define TINYC_AST_JSON_VISITOR_H

#include "tinyc/ast/NodeVisitor.h"
#include "tinyc/ast/ASTNode.h"
#include <string>
#include <sstream>

namespace tinyc::ast {

	/**
	 * @brief Visitor for converting AST nodes to JSON
	 */
	class JSONVisitor : public NodeVisitor {
	public:
		explicit JSONVisitor(bool prettyPrint);

		/**
		 * @brief Get the resulting JSON string
		 */
		std::string getJSON() const;

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

		// Error node
		void visit(const ErrorProgramNode &node) override;

	private:
		std::ostringstream json;
		int indentLevel = 0;
		bool prettyPrint = true;

		/**
		 * @brief Add a field to the JSON object
		 */
		void addField(const std::string &name, const std::string &value);

		/**
		* @brief Add a boolean field to the JSON object
		*/
		void addBooleanField(const std::string& name, bool value);

		/**
		 * @brief Add a number (int) field to the JSON object
		 */
		void addNumberField(const std::string& name, int value);

		/**
		 * @brief Add a number (double) field to the JSON object
		 */
		void addNumberField(const std::string& name, double value);

		/**
		 * @brief Start a new JSON object
		 */
		void startObject();

		/**
		 * @brief End the current JSON object
		 */
		void endObject();

		/**
		 * @brief Start a new JSON array
		 */
		void startArray(const std::string &name);

		/**
		 * @brief End the current JSON array
		 */
		void endArray();

		/**
		 * @brief Get indentation string
		 */
		std::string getIndent() const;

		/**
		 * @brief Increase indentation level
		 */
		void increaseIndent();

		/**
		 * @brief Decrease indentation level
		 */
		void decreaseIndent();

		/**
		 * @brief Add a node field to the JSON object
		 */
		void addNodeField(const std::string &name, const ASTNode &node);

		/**
		 * @brief Escape a string for JSON
		 */
		static std::string escapeString(const std::string &s);

		/**
		 * @brief Add a source location field to the JSON object
		 * @attention Location is always the last field, so it doesn't add a comma afterwards
		 */
		void addLocationField(const lexer::SourceLocation &location);
	};

} // namespace tinyc::ast

#endif // TINYC_AST_JSON_VISITOR_H