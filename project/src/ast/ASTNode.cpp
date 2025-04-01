#include <utility>

#include "tinyc/ast/ASTNode.h"

namespace tinyc::ast {

	// ASTNode implementation
	ASTNode::ASTNode(lexer::SourceLocation location) : location(std::move(location)) {}

	[[nodiscard]] lexer::SourceLocation ASTNode::getLocation() const {
		return location;
	}

	/* ===== Type Nodes ===== */

	// PrimitiveTypeNode implementation
	PrimitiveTypeNode::PrimitiveTypeNode(Kind kind, lexer::SourceLocation location)
			: ASTNode(std::move(location)), kind(kind) {
	}

	[[nodiscard]] PrimitiveTypeNode::Kind PrimitiveTypeNode::getKind() const {
		return kind;
	}

	[[nodiscard]] std::string PrimitiveTypeNode::getKindString() const {
		switch (kind) {
			case Kind::INT:
				return "int";
			case Kind::DOUBLE:
				return "double";
			case Kind::CHAR:
				return "char";
			case Kind::VOID:
				return "void";
			default:
				return "unknown";
		}
	}

	// NamedTypeNode implementation
	NamedTypeNode::NamedTypeNode(std::string identifier, lexer::SourceLocation location)
			: ASTNode(std::move(location)), identifier(std::move(identifier)) {
	}

	[[nodiscard]] const std::string &NamedTypeNode::getIdentifier() const {
		return identifier;
	}

	// PointerTypeNode implementation
	PointerTypeNode::PointerTypeNode(ASTNodePtr baseType, lexer::SourceLocation location)
			: ASTNode(std::move(location)), baseType(std::move(baseType)) {
	}

	[[nodiscard]] const ASTNodePtr &PointerTypeNode::getBaseType() const {
		return baseType;
	}

	/* ===== Expression Nodes ===== */

	// LiteralNode implementation
	LiteralNode::LiteralNode(std::string value, Kind kind, lexer::SourceLocation location)
			: ASTNode(std::move(location)), kind(kind), value(std::move(value)) {
	}

	[[nodiscard]] LiteralNode::Kind LiteralNode::getKind() const {
		return kind;
	}

	[[nodiscard]] const std::string &LiteralNode::getValue() const {
		return value;
	}

	[[nodiscard]] std::string LiteralNode::getKindString() const {
		switch (kind) {
			case Kind::INTEGER:
				return "integer";
			case Kind::DOUBLE:
				return "double";
			case Kind::CHAR:
				return "char";
			case Kind::STRING:
				return "string";
			default:
				return "unknown";
		}
	}

	// IdentifierNode implementation
	IdentifierNode::IdentifierNode(std::string identifier, lexer::SourceLocation location)
			: ASTNode(std::move(location)), identifier(std::move(identifier)) {
	}

	[[nodiscard]] const std::string &IdentifierNode::getIdentifier() const {
		return identifier;
	}

	// BinaryExpressionNode implementation
	BinaryExpressionNode::BinaryExpressionNode(
			Operator op,
			ASTNodePtr left,
			ASTNodePtr right,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), op(op), left(std::move(left)), right(std::move(right)) {
	}

	[[nodiscard]] BinaryExpressionNode::Operator BinaryExpressionNode::getOperator() const {
		return op;
	}

	[[nodiscard]] std::string BinaryExpressionNode::getOperatorString() const {
		switch (op) {
			case Operator::ADD:
				return "+";
			case Operator::SUBTRACT:
				return "-";
			case Operator::MULTIPLY:
				return "*";
			case Operator::DIVIDE:
				return "/";
			case Operator::MODULO:
				return "%";
			case Operator::BITWISE_AND:
				return "&";
			case Operator::BITWISE_OR:
				return "|";
			case Operator::LEFT_SHIFT:
				return "<<";
			case Operator::RIGHT_SHIFT:
				return ">>";
			case Operator::LOGICAL_AND:
				return "&&";
			case Operator::LOGICAL_OR:
				return "||";
			case Operator::EQUAL:
				return "==";
			case Operator::NOT_EQUAL:
				return "!=";
			case Operator::LESS:
				return "<";
			case Operator::LESS_EQUAL:
				return "<=";
			case Operator::GREATER:
				return ">";
			case Operator::GREATER_EQUAL:
				return ">=";
			case Operator::ASSIGN:
				return "=";
			default:
				return "unknown";
		}
	}

	[[nodiscard]] const ASTNodePtr &BinaryExpressionNode::getLeft() const {
		return left;
	}

	[[nodiscard]] const ASTNodePtr &BinaryExpressionNode::getRight() const {
		return right;
	}

	// UnaryExpressionNode implementation
	UnaryExpressionNode::UnaryExpressionNode(
			Operator op,
			ASTNodePtr operand,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), op(op), operand(std::move(operand)) {
	}

	[[nodiscard]] UnaryExpressionNode::Operator UnaryExpressionNode::getOperator() const {
		return op;
	}

	[[nodiscard]] std::string UnaryExpressionNode::getOperatorString() const {
		switch (op) {
			case Operator::POSITIVE:
				return "+";
			case Operator::NEGATIVE:
				return "-";
			case Operator::LOGICAL_NOT:
				return "!";
			case Operator::BITWISE_NOT:
				return "~";
			case Operator::PRE_INCREMENT:
				return "++ (pre)";
			case Operator::PRE_DECREMENT:
				return "-- (pre)";
			case Operator::DEREFERENCE:
				return "*";
			case Operator::ADDRESS_OF:
				return "&";
			case Operator::POST_INCREMENT:
				return "++ (post)";
			case Operator::POST_DECREMENT:
				return "-- (post)";
			default:
				return "unknown";
		}
	}

	[[nodiscard]] bool UnaryExpressionNode::isPrefix() const {
		switch (op) {
			case Operator::POSITIVE:
			case Operator::NEGATIVE:
			case Operator::LOGICAL_NOT:
			case Operator::BITWISE_NOT:
			case Operator::PRE_INCREMENT:
			case Operator::PRE_DECREMENT:
			case Operator::DEREFERENCE:
			case Operator::ADDRESS_OF:
				return true;
			case Operator::POST_INCREMENT:
			case Operator::POST_DECREMENT:
				return false;
			default:
				return true;
		}
	}

	[[nodiscard]] const ASTNodePtr &UnaryExpressionNode::getOperand() const {
		return operand;
	}

	// CastExpressionNode implementation
	CastExpressionNode::CastExpressionNode(
			ASTNodePtr targetType,
			ASTNodePtr expression,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), targetType(std::move(targetType)), expression(std::move(expression)) {
	}

	[[nodiscard]] const ASTNodePtr &CastExpressionNode::getTargetType() const {
		return targetType;
	}

	[[nodiscard]] const ASTNodePtr &CastExpressionNode::getExpression() const {
		return expression;
	}

	// CallExpressionNode implementation
	CallExpressionNode::CallExpressionNode(
			ASTNodePtr callee,
			std::vector<ASTNodePtr> arguments,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), callee(std::move(callee)), arguments(std::move(arguments)) {
	}

	const ASTNodePtr &CallExpressionNode::getCallee() const {
		return callee;
	}

	const std::vector<ASTNodePtr> &CallExpressionNode::getArguments() const {
		return arguments;
	}

	// IndexExpressionNode implementation
	IndexExpressionNode::IndexExpressionNode(
			ASTNodePtr array,
			ASTNodePtr index,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), array(std::move(array)), index(std::move(index)) {
	}

	const ASTNodePtr &IndexExpressionNode::getArray() const {
		return array;
	}

	const ASTNodePtr &IndexExpressionNode::getIndex() const {
		return index;
	}

	// MemberExpressionNode implementation
	MemberExpressionNode::MemberExpressionNode(
			Kind kind,
			ASTNodePtr object,
			std::string member,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), kind(kind), object(std::move(object)), member(std::move(member)) {
	}

	MemberExpressionNode::Kind MemberExpressionNode::getKind() const {
		return kind;
	}

	const ASTNodePtr &MemberExpressionNode::getObject() const {
		return object;
	}

	const std::string &MemberExpressionNode::getMember() const {
		return member;
	}

	// CommaExpressionNode implementation
	CommaExpressionNode::CommaExpressionNode(
			std::vector<ASTNodePtr> expressions,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), expressions(std::move(expressions)) {
	}

	const std::vector<ASTNodePtr> &CommaExpressionNode::getExpressions() const {
		return expressions;
	}

	/* ===== Statement Nodes ===== */

	// BlockStatementNode implementation
	BlockStatementNode::BlockStatementNode(
			std::vector<ASTNodePtr> statements,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), statements(std::move(statements)) {
	}

	const std::vector<ASTNodePtr> &BlockStatementNode::getStatements() const {
		return statements;
	}

	// ExpressionStatementNode implementation
	ExpressionStatementNode::ExpressionStatementNode(
			ASTNodePtr expression,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), expression(std::move(expression)) {
	}

	const ASTNodePtr &ExpressionStatementNode::getExpression() const {
		return expression;
	}

	// IfStatementNode implementation
	IfStatementNode::IfStatementNode(
			ASTNodePtr condition,
			ASTNodePtr thenBranch,
			ASTNodePtr elseBranch,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)),
		condition(std::move(condition)),
		thenBranch(std::move(thenBranch)),
		elseBranch(std::move(elseBranch)) {
	}

	const ASTNodePtr &IfStatementNode::getCondition() const {
		return condition;
	}

	const ASTNodePtr &IfStatementNode::getThenBranch() const {
		return thenBranch;
	}

	bool IfStatementNode::hasElseBranch() const {
		return elseBranch != nullptr;
	}

	const ASTNodePtr &IfStatementNode::getElseBranch() const {
		return elseBranch;
	}

	// WhileStatementNode implementation
	WhileStatementNode::WhileStatementNode(
			ASTNodePtr condition,
			ASTNodePtr body,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), condition(std::move(condition)), body(std::move(body)) {
	}

	const ASTNodePtr &WhileStatementNode::getCondition() const {
		return condition;
	}

	const ASTNodePtr &WhileStatementNode::getBody() const {
		return body;
	}

	// DoWhileStatementNode implementation
	DoWhileStatementNode::DoWhileStatementNode(
			ASTNodePtr body,
			ASTNodePtr condition,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), body(std::move(body)), condition(std::move(condition)) {
	}

	const ASTNodePtr &DoWhileStatementNode::getBody() const {
		return body;
	}

	const ASTNodePtr &DoWhileStatementNode::getCondition() const {
		return condition;
	}

	// ForStatementNode implementation
	ForStatementNode::ForStatementNode(
			ASTNodePtr initialization,
			ASTNodePtr condition,
			ASTNodePtr update,
			ASTNodePtr body,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)),
		initialization(std::move(initialization)),
		condition(std::move(condition)),
		update(std::move(update)),
		body(std::move(body)) {
	}

	bool ForStatementNode::hasInitialization() const {
		return initialization != nullptr;
	}

	const ASTNodePtr &ForStatementNode::getInitialization() const {
		return initialization;
	}

	bool ForStatementNode::hasCondition() const {
		return condition != nullptr;
	}

	const ASTNodePtr &ForStatementNode::getCondition() const {
		return condition;
	}

	bool ForStatementNode::hasUpdate() const {
		return update != nullptr;
	}

	const ASTNodePtr &ForStatementNode::getUpdate() const {
		return update;
	}

	const ASTNodePtr &ForStatementNode::getBody() const {
		return body;
	}

	// SwitchStatementNode implementation
	SwitchStatementNode::SwitchStatementNode(
			ASTNodePtr expression,
			std::vector<Case> cases,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), expression(std::move(expression)), cases(std::move(cases)) {
	}

	const ASTNodePtr &SwitchStatementNode::getExpression() const {
		return expression;
	}

	const std::vector<SwitchStatementNode::Case> &SwitchStatementNode::getCases() const {
		return cases;
	}

	// ReturnStatementNode implementation
	ReturnStatementNode::ReturnStatementNode(
			ASTNodePtr expression,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), expression(std::move(expression)) {
	}

	bool ReturnStatementNode::hasValue() const {
		return expression != nullptr;
	}

	const ASTNodePtr &ReturnStatementNode::getExpression() const {
		return expression;
	}

	/* ===== Declaration Nodes ===== */

	// VariableNode implementation
	const ASTNodePtr &VariableNode::getType() const {
		return type;
	}

	bool VariableNode::isArray() const {
		return arraySize != nullptr;
	}

	const ASTNodePtr &VariableNode::getArraySize() const {
		return arraySize;
	}

	bool VariableNode::hasInitializer() const {
		return initializer != nullptr;
	}

	const ASTNodePtr &VariableNode::getInitializer() const {
		return initializer;
	}

	// MultipleDeclarationNode implementation
	const std::vector<ASTNodePtr> &MultipleDeclarationNode::getDeclarations() const {
		return declarations;
	}

	// ParameterNode implementation
	const ASTNodePtr &ParameterNode::getType() const {
		return type;
	}

	// FunctionDeclarationNode implementation
	FunctionDeclarationNode::FunctionDeclarationNode(
			ASTNodePtr returnType,
			std::string identifier,
			std::vector<ASTNodePtr> parameters,
			ASTNodePtr body,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)),
		returnType(std::move(returnType)),
		identifier(std::move(identifier)),
		parameters(std::move(parameters)),
		body(std::move(body)) {
	}

	const ASTNodePtr &FunctionDeclarationNode::getReturnType() const {
		return returnType;
	}

	const std::string &FunctionDeclarationNode::getIdentifier() const {
		return identifier;
	}

	const std::vector<ASTNodePtr> &FunctionDeclarationNode::getParameters() const {
		return parameters;
	}

	bool FunctionDeclarationNode::isDefinition() const {
		return body != nullptr;
	}

	const ASTNodePtr &FunctionDeclarationNode::getBody() const {
		return body;
	}

	// StructDeclarationNode implementation
	StructDeclarationNode::StructDeclarationNode(
			std::string identifier,
			std::vector<ASTNodePtr> fields,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)), identifier(std::move(identifier)), fields(std::move(fields)) {
	}

	const std::string &StructDeclarationNode::getIdentifier() const {
		return identifier;
	}

	bool StructDeclarationNode::isDefinition() const {
		return !fields.empty();
	}

	const std::vector<ASTNodePtr> &StructDeclarationNode::getFields() const {
		return fields;
	}

	// FunctionPointerDeclarationNode implementation
	FunctionPointerDeclarationNode::FunctionPointerDeclarationNode(
			ASTNodePtr returnType,
			std::string identifier,
			std::vector<ASTNodePtr> parameterTypes,
			lexer::SourceLocation location
	) : ASTNode(std::move(location)),
		returnType(std::move(returnType)),
		identifier(std::move(identifier)),
		parameterTypes(std::move(parameterTypes)) {
	}

	const ASTNodePtr &FunctionPointerDeclarationNode::getReturnType() const {
		return returnType;
	}

	const std::string &FunctionPointerDeclarationNode::getIdentifier() const {
		return identifier;
	}

	const std::vector<ASTNodePtr> &FunctionPointerDeclarationNode::getParameterTypes() const {
		return parameterTypes;
	}

	// ProgramNode implementation
	ProgramNode::ProgramNode(std::string sourceName) : ASTNode(lexer::SourceLocation(std::move(sourceName), 0, 0)) {
	}

	void ProgramNode::addDeclaration(ASTNodePtr declaration) {
		declarations.push_back(std::move(declaration));
	}

	const std::vector<ASTNodePtr> &ProgramNode::getDeclarations() const {
		return declarations;
	}
} // namespace tinyc::ast