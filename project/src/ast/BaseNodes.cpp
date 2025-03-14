#include "tinyc/ast/ASTNode.h"
#include "tinyc/ast/TypeNodes.h"
#include "tinyc/ast/DeclarationNodes.h"
#include "tinyc/ast/StatementNodes.h"
#include "tinyc/ast/ExpressionNodes.h"


namespace tinyc::ast {

	// Base class implementations

	// TypeNode implementation
	TypeNode::TypeNode(const lexer::SourceLocation &location)
			: location(location) {
	}

	lexer::SourceLocation TypeNode::getLocation() const {
		return location;
	}

	// DeclarationNode implementation
	DeclarationNode::DeclarationNode(const std::string &name, const lexer::SourceLocation &location)
			: name(name), location(location) {
	}

	const std::string &DeclarationNode::getName() const {
		return name;
	}

	lexer::SourceLocation DeclarationNode::getLocation() const {
		return location;
	}

	// StatementNode implementation
	StatementNode::StatementNode(const lexer::SourceLocation &location)
			: location(location) {
	}

	lexer::SourceLocation StatementNode::getLocation() const {
		return location;
	}

	// ExpressionNode implementation
	ExpressionNode::ExpressionNode(const lexer::SourceLocation &location)
			: location(location) {
	}

	lexer::SourceLocation ExpressionNode::getLocation() const {
		return location;
	}

} // namespace tinyc::ast
