#include "tinyc/ast/visitors/DumpVisitor.h"
#include "tinyc/ast/ASTNode.h"

namespace tinyc::ast {

	DumpVisitor::DumpVisitor(std::ostream &os) : os(os), indentLevel(0) {}

	std::string DumpVisitor::getIndent() const {
		return std::string(indentLevel * 2, ' ');
	}

	void DumpVisitor::increaseIndent() {
		indentLevel++;
	}

	void DumpVisitor::decreaseIndent() {
		if (indentLevel > 0) {
			indentLevel--;
		}
	}

	void DumpVisitor::dumpChild(const ASTNode &node) {
		increaseIndent();
		node.accept(*this);
		decreaseIndent();
	}

// Program node
	void DumpVisitor::visit(const ProgramNode &node) {
		os << getIndent() << "Program" << std::endl;

		for (const auto &decl: node.getDeclarations()) {
			dumpChild(*decl);
		}
	}

// Declaration nodes
	void DumpVisitor::visit(const VariableNode &node) {
		os << getIndent() << "VariableDeclaration: " << node.getIdentifier() << std::endl;

		os << getIndent() << "  Type:" << std::endl;
		dumpChild(*node.getType());

		if (node.isArray()) {
			os << getIndent() << "  Array Size:" << std::endl;
			dumpChild(*node.getArraySize());
		}

		if (node.hasInitializer()) {
			os << getIndent() << "  Initializer:" << std::endl;
			dumpChild(*node.getInitializer());
		}
	}

	void DumpVisitor::visit(const ParameterNode &node) {
		os << getIndent() << "Parameter: " << node.getIdentifier() << std::endl;

		os << getIndent() << "  Type:" << std::endl;
		dumpChild(*node.getType());
	}

	void DumpVisitor::visit(const FunctionDeclarationNode &node) {
		if (node.isDefinition()) {
			os << getIndent() << "FunctionDefinition: " << node.getIdentifier() << std::endl;
		} else {
			os << getIndent() << "FunctionDeclaration: " << node.getIdentifier() << std::endl;
		}

		os << getIndent() << "  Return Type:" << std::endl;
		dumpChild(*node.getReturnType());

		os << getIndent() << "  Parameters:" << std::endl;
		for (const auto &param: node.getParameters()) {
			dumpChild(*param);
		}

		if (node.isDefinition()) {
			os << getIndent() << "  Body:" << std::endl;
			dumpChild(*node.getBody());
		}
	}

	void DumpVisitor::visit(const StructDeclarationNode &node) {
		if (node.isDefinition()) {
			os << getIndent() << "StructDefinition: " << node.getIdentifier() << std::endl;

			os << getIndent() << "  Fields:" << std::endl;
			for (const auto &field: node.getFields()) {
				dumpChild(*field);
			}
		} else {
			os << getIndent() << "StructDeclaration: " << node.getIdentifier() << std::endl;
		}
	}

	void DumpVisitor::visit(const FunctionPointerDeclarationNode &node) {
		os << getIndent() << "FunctionPointerDeclaration: " << node.getIdentifier() << std::endl;

		os << getIndent() << "  Return Type:" << std::endl;
		dumpChild(*node.getReturnType());

		os << getIndent() << "  Parameter Types:" << std::endl;
		for (const auto &type: node.getParameterTypes()) {
			dumpChild(*type);
		}
	}

// Type nodes
	void DumpVisitor::visit(const PrimitiveTypeNode &node) {
		os << getIndent() << "PrimitiveType: " << node.getKindString() << std::endl;
	}

	void DumpVisitor::visit(const NamedTypeNode &node) {
		os << getIndent() << "NamedType: " << node.getIdentifier() << std::endl;
	}

	void DumpVisitor::visit(const PointerTypeNode &node) {
		os << getIndent() << "PointerType:" << std::endl;
		dumpChild(*node.getBaseType());
	}

// Expression nodes
	void DumpVisitor::visit(const LiteralNode &node) {
		os << getIndent() << "Literal (" << node.getKindString() << "): "
		   << node.getValue() << std::endl;
	}

	void DumpVisitor::visit(const IdentifierNode &node) {
		os << getIndent() << "Identifier: " << node.getIdentifier() << std::endl;
	}

	void DumpVisitor::visit(const BinaryExpressionNode &node) {
		os << getIndent() << "BinaryExpression: " << node.getOperatorString() << std::endl;

		os << getIndent() << "  Left:" << std::endl;
		dumpChild(*node.getLeft());

		os << getIndent() << "  Right:" << std::endl;
		dumpChild(*node.getRight());
	}

	void DumpVisitor::visit(const UnaryExpressionNode &node) {
		os << getIndent() << "UnaryExpression: " << node.getOperatorString() << std::endl;
		dumpChild(*node.getOperand());
	}

	void DumpVisitor::visit(const CastExpressionNode &node) {
		os << getIndent() << "CastExpression:" << std::endl;

		os << getIndent() << "  Target Type:" << std::endl;
		dumpChild(*node.getTargetType());

		os << getIndent() << "  Expression:" << std::endl;
		dumpChild(*node.getExpression());
	}

	void DumpVisitor::visit(const CallExpressionNode &node) {
		os << getIndent() << "CallExpression:" << std::endl;

		os << getIndent() << "  Callee:" << std::endl;
		dumpChild(*node.getCallee());

		os << getIndent() << "  Arguments:" << std::endl;
		for (const auto &arg: node.getArguments()) {
			dumpChild(*arg);
		}
	}

	void DumpVisitor::visit(const IndexExpressionNode &node) {
		os << getIndent() << "IndexExpression:" << std::endl;

		os << getIndent() << "  Array:" << std::endl;
		dumpChild(*node.getArray());

		os << getIndent() << "  Index:" << std::endl;
		dumpChild(*node.getIndex());
	}

	void DumpVisitor::visit(const MemberExpressionNode &node) {
		os << getIndent() << "MemberExpression (" << (node.getKind() == MemberExpressionNode::Kind::DOT ? "." : "->")
		   << "):" << std::endl;

		os << getIndent() << "  Object:" << std::endl;
		dumpChild(*node.getObject());

		os << getIndent() << "  Member: " << node.getMember() << std::endl;
	}

	void DumpVisitor::visit(const CommaExpressionNode &node) {
		os << getIndent() << "CommaExpression:" << std::endl;

		for (const auto &expr: node.getExpressions()) {
			dumpChild(*expr);
		}
	}

	// Statement nodes
	void DumpVisitor::visit(const BlockStatementNode &node) {
		os << getIndent() << "BlockStatement:" << std::endl;

		for (const auto &stmt: node.getStatements()) {
			dumpChild(*stmt);
		}
	}

	void DumpVisitor::visit(const ExpressionStatementNode &node) {
		os << getIndent() << "ExpressionStatement:" << std::endl;
		dumpChild(*node.getExpression());
	}

	void DumpVisitor::visit(const IfStatementNode &node) {
		os << getIndent() << "IfStatement:" << std::endl;

		os << getIndent() << "  Condition:" << std::endl;
		dumpChild(*node.getCondition());

		os << getIndent() << "  Then:" << std::endl;
		dumpChild(*node.getThenBranch());

		if (node.hasElseBranch()) {
			os << getIndent() << "  Else:" << std::endl;
			dumpChild(*node.getElseBranch());
		}
	}

	void DumpVisitor::visit(const WhileStatementNode &node) {
		os << getIndent() << "WhileStatement:" << std::endl;

		os << getIndent() << "  Condition:" << std::endl;
		dumpChild(*node.getCondition());

		os << getIndent() << "  Body:" << std::endl;
		dumpChild(*node.getBody());
	}

	void DumpVisitor::visit(const DoWhileStatementNode &node) {
		os << getIndent() << "DoWhileStatement:" << std::endl;

		os << getIndent() << "  Body:" << std::endl;
		dumpChild(*node.getBody());

		os << getIndent() << "  Condition:" << std::endl;
		dumpChild(*node.getCondition());
	}

	void DumpVisitor::visit(const ForStatementNode &node) {
		os << getIndent() << "ForStatement:" << std::endl;

		if (node.hasInitialization()) {
			os << getIndent() << "  Initialization:" << std::endl;
			dumpChild(*node.getInitialization());
		}

		if (node.hasCondition()) {
			os << getIndent() << "  Condition:" << std::endl;
			dumpChild(*node.getCondition());
		}

		if (node.hasUpdate()) {
			os << getIndent() << "  Update:" << std::endl;
			dumpChild(*node.getUpdate());
		}

		os << getIndent() << "  Body:" << std::endl;
		dumpChild(*node.getBody());
	}

	void DumpVisitor::visit(const SwitchStatementNode &node) {
		os << getIndent() << "SwitchStatement:" << std::endl;

		os << getIndent() << "  Expression:" << std::endl;
		dumpChild(*node.getExpression());

		os << getIndent() << "  Cases:" << std::endl;
		for (const auto &caseItem: node.getCases()) {
			if (caseItem.isDefault) {
				os << getIndent() << "    DefaultCase:" << std::endl;
			} else {
				os << getIndent() << "    Case: " << caseItem.value << std::endl;
			}

			increaseIndent();
			increaseIndent();
			for (const auto &stmt: caseItem.body) {
				stmt->accept(*this);
			}
			decreaseIndent();
			decreaseIndent();
		}
	}

	void DumpVisitor::visit(const BreakStatementNode &) {
		os << getIndent() << "BreakStatement" << std::endl;
	}

	void DumpVisitor::visit(const ContinueStatementNode &) {
		os << getIndent() << "ContinueStatement" << std::endl;
	}

	void DumpVisitor::visit(const ReturnStatementNode &node) {
		os << getIndent() << "ReturnStatement:" << std::endl;

		if (node.hasValue()) {
			dumpChild(*node.getExpression());
		} else {
			os << getIndent() << "  <void>" << std::endl;
		}
	}

} // namespace tinyc::ast