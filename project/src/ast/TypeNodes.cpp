#include "tinyc/ast/TypeNodes.h"
#include <sstream>
#include <utility>


namespace tinyc::ast {

	// PrimitiveTypeNode implementation
	PrimitiveTypeNode::PrimitiveTypeNode(Kind kind, const lexer::SourceLocation &location)
			: TypeNode(location), kind(kind) {
	}

	PrimitiveTypeNode::Kind PrimitiveTypeNode::getKind() const {
		return kind;
	}

	std::string PrimitiveTypeNode::getKindString() const {
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

	void PrimitiveTypeNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "PrimitiveType: " << getKindString() << std::endl;
	}

	std::string PrimitiveTypeNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "PrimitiveType", )"
			 << R"("kind": ")" << getKindString() << "\", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// NamedTypeNode implementation
	NamedTypeNode::NamedTypeNode(std::string name, const lexer::SourceLocation &location)
			: TypeNode(location), name(std::move(name)) {
	}

	const std::string &NamedTypeNode::getName() const {
		return name;
	}

	void NamedTypeNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "NamedType: " << name << std::endl;
	}

	std::string NamedTypeNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "NamedType", )"
			 << R"("name": ")" << name << "\", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// PointerTypeNode implementation
	PointerTypeNode::PointerTypeNode(ASTNodePtr baseType, const lexer::SourceLocation &location)
			: TypeNode(location), baseType(std::move(baseType)) {
	}

	ASTNodePtr PointerTypeNode::getBaseType() const {
		return baseType;
	}

	void PointerTypeNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "PointerType:" << std::endl;
		baseType->dump(os, indent + 1);
	}

	std::string PointerTypeNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "PointerType", )"
			 << "\"baseType\": " << baseType->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

} // namespace tinyc::ast
