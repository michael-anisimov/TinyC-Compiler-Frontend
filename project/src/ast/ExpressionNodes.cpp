#include "tinyc/ast/ExpressionNodes.h"
#include <sstream>
#include <utility>


namespace tinyc::ast {

	// LiteralNode implementation
	LiteralNode::LiteralNode(Kind kind, std::string value, const lexer::SourceLocation &location)
			: ExpressionNode(location), kind(kind), value(std::move(value)) {
	}

	LiteralNode::Kind LiteralNode::getKind() const {
		return kind;
	}

	const std::string &LiteralNode::getValue() const {
		return value;
	}

	std::string LiteralNode::getKindString() const {
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

	void LiteralNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "Literal (" << getKindString() << "): " << value << std::endl;
	}

	std::string LiteralNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "Literal", )"
			 << R"("kind": ")" << getKindString() << "\", "
			 << R"("value": ")" << value << "\", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// IdentifierNode implementation
	IdentifierNode::IdentifierNode(std::string name, const lexer::SourceLocation &location)
			: ExpressionNode(location), name(std::move(name)) {
	}

	const std::string &IdentifierNode::getName() const {
		return name;
	}

	void IdentifierNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "Identifier: " << name << std::endl;
	}

	std::string IdentifierNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "Identifier", )"
			 << R"("name": ")" << name << "\", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// BinaryExpressionNode implementation
	BinaryExpressionNode::BinaryExpressionNode(
			Operator op,
			ASTNodePtr left,
			ASTNodePtr right,
			const lexer::SourceLocation &location) : ExpressionNode(location), op(op), left(std::move(left)),
													 right(std::move(right)) {
	}

	BinaryExpressionNode::Operator BinaryExpressionNode::getOperator() const {
		return op;
	}

	std::string BinaryExpressionNode::getOperatorString() const {
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

	ASTNodePtr BinaryExpressionNode::getLeft() const {
		return left;
	}

	ASTNodePtr BinaryExpressionNode::getRight() const {
		return right;
	}

	void BinaryExpressionNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "BinaryExpression: " << getOperatorString() << std::endl;

		os << getIndent(indent + 1) << "Left:" << std::endl;
		left->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Right:" << std::endl;
		right->dump(os, indent + 2);
	}

	std::string BinaryExpressionNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "BinaryExpression", )"
			 << R"("operator": ")" << getOperatorString() << "\", "
			 << "\"left\": " << left->toJSON() << ", "
			 << "\"right\": " << right->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// UnaryExpressionNode implementation
	UnaryExpressionNode::UnaryExpressionNode(
			Operator op,
			ASTNodePtr operand,
			const lexer::SourceLocation &location) : ExpressionNode(location), op(op), operand(std::move(operand)) {
	}

	UnaryExpressionNode::Operator UnaryExpressionNode::getOperator() const {
		return op;
	}

	std::string UnaryExpressionNode::getOperatorString() const {
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

	bool UnaryExpressionNode::isPrefix() const {
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

	ASTNodePtr UnaryExpressionNode::getOperand() const {
		return operand;
	}

	void UnaryExpressionNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "UnaryExpression: " << getOperatorString() << std::endl;
		operand->dump(os, indent + 1);
	}

	std::string UnaryExpressionNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "UnaryExpression", )"
			 << R"("operator": ")" << getOperatorString() << "\", "
			 << "\"prefix\": " << (isPrefix() ? "true" : "false") << ", "
			 << "\"operand\": " << operand->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// CastExpressionNode implementation
	CastExpressionNode::CastExpressionNode(
			ASTNodePtr targetType,
			ASTNodePtr expression,
			const lexer::SourceLocation &location) : ExpressionNode(location), targetType(std::move(targetType)),
													 expression(std::move(expression)) {
	}

	ASTNodePtr CastExpressionNode::getTargetType() const {
		return targetType;
	}

	ASTNodePtr CastExpressionNode::getExpression() const {
		return expression;
	}

	void CastExpressionNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "CastExpression:" << std::endl;

		os << getIndent(indent + 1) << "Target Type:" << std::endl;
		targetType->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Expression:" << std::endl;
		expression->dump(os, indent + 2);
	}

	std::string CastExpressionNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "CastExpression", )"
			 << "\"targetType\": " << targetType->toJSON() << ", "
			 << "\"expression\": " << expression->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// CallExpressionNode implementation
	CallExpressionNode::CallExpressionNode(
			ASTNodePtr callee,
			std::vector<ASTNodePtr> arguments,
			const lexer::SourceLocation &location) : ExpressionNode(location), callee(std::move(callee)),
													 arguments(std::move(arguments)) {
	}

	ASTNodePtr CallExpressionNode::getCallee() const {
		return callee;
	}

	const std::vector<ASTNodePtr> &CallExpressionNode::getArguments() const {
		return arguments;
	}

	void CallExpressionNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "CallExpression:" << std::endl;

		os << getIndent(indent + 1) << "Callee:" << std::endl;
		callee->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Arguments:" << std::endl;
		for (const auto &arg: arguments) {
			arg->dump(os, indent + 2);
		}
	}

	std::string CallExpressionNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "CallExpression", )"
			 << "\"callee\": " << callee->toJSON() << ", "
			 << "\"arguments\": [";

		for (size_t i = 0; i < arguments.size(); ++i) {
			json << arguments[i]->toJSON();
			if (i < arguments.size() - 1) {
				json << ", ";
			}
		}

		json << "]"
			 << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// IndexExpressionNode implementation
	IndexExpressionNode::IndexExpressionNode(
			ASTNodePtr array,
			ASTNodePtr index,
			const lexer::SourceLocation &location) : ExpressionNode(location), array(std::move(array)),
													 index(std::move(index)) {
	}

	ASTNodePtr IndexExpressionNode::getArray() const {
		return array;
	}

	ASTNodePtr IndexExpressionNode::getIndex() const {
		return index;
	}

	void IndexExpressionNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "IndexExpression:" << std::endl;

		os << getIndent(indent + 1) << "Array:" << std::endl;
		array->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Index:" << std::endl;
		index->dump(os, indent + 2);
	}

	std::string IndexExpressionNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "IndexExpression", )"
			 << "\"array\": " << array->toJSON() << ", "
			 << "\"index\": " << index->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// MemberExpressionNode implementation
	MemberExpressionNode::MemberExpressionNode(
			Kind kind,
			ASTNodePtr object,
			std::string member,
			const lexer::SourceLocation &location) : ExpressionNode(location), kind(kind), object(std::move(object)),
													 member(std::move(member)) {
	}

	MemberExpressionNode::Kind MemberExpressionNode::getKind() const {
		return kind;
	}

	ASTNodePtr MemberExpressionNode::getObject() const {
		return object;
	}

	const std::string &MemberExpressionNode::getMember() const {
		return member;
	}

	void MemberExpressionNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "MemberExpression (" << (kind == Kind::DOT ? "." : "->") << "):" << std::endl;

		os << getIndent(indent + 1) << "Object:" << std::endl;
		object->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Member: " << member << std::endl;
	}

	std::string MemberExpressionNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "MemberExpression", )"
			 << R"("kind": ")" << (kind == Kind::DOT ? "dot" : "arrow") << "\", "
			 << "\"object\": " << object->toJSON() << ", "
			 << R"("member": ")" << member << "\", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// CommaExpressionNode implementation
	CommaExpressionNode::CommaExpressionNode(
			std::vector<ASTNodePtr> expressions,
			const lexer::SourceLocation &location) : ExpressionNode(location), expressions(std::move(expressions)) {
	}

	const std::vector<ASTNodePtr> &CommaExpressionNode::getExpressions() const {
		return expressions;
	}

	void CommaExpressionNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "CommaExpression:" << std::endl;

		for (const auto &expr: expressions) {
			expr->dump(os, indent + 1);
		}
	}

	std::string CommaExpressionNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "CommaExpression", )"
			 << "\"expressions\": [";

		for (size_t i = 0; i < expressions.size(); ++i) {
			json << expressions[i]->toJSON();
			if (i < expressions.size() - 1) {
				json << ", ";
			}
		}

		json << "]"
			 << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

} // namespace tinyc::ast
