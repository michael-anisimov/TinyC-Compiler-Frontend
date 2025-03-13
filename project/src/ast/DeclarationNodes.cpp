#include "tinyc/ast/DeclarationNodes.h"
#include <sstream>
#include <utility>


namespace tinyc::ast {

	// VariableDeclarationNode implementation
	VariableDeclarationNode::VariableDeclarationNode(
			ASTNodePtr type,
			const std::string &name,
			ASTNodePtr arraySize,
			ASTNodePtr initializer,
			const lexer::SourceLocation &location) : DeclarationNode(name, location), type(std::move(type)),
													 arraySize(std::move(arraySize)),
													 initializer(std::move(initializer)) {
	}

	ASTNodePtr VariableDeclarationNode::getType() const {
		return type;
	}

	bool VariableDeclarationNode::isArray() const {
		return arraySize != nullptr;
	}

	ASTNodePtr VariableDeclarationNode::getArraySize() const {
		return arraySize;
	}

	bool VariableDeclarationNode::hasInitializer() const {
		return initializer != nullptr;
	}

	ASTNodePtr VariableDeclarationNode::getInitializer() const {
		return initializer;
	}

	void VariableDeclarationNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "VariableDeclaration: " << name << std::endl;

		os << getIndent(indent + 1) << "Type:" << std::endl;
		type->dump(os, indent + 2);

		if (isArray()) {
			os << getIndent(indent + 1) << "Array Size:" << std::endl;
			arraySize->dump(os, indent + 2);
		}

		if (hasInitializer()) {
			os << getIndent(indent + 1) << "Initializer:" << std::endl;
			initializer->dump(os, indent + 2);
		}
	}

	std::string VariableDeclarationNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "VariableDeclaration", )"
			 << R"("name": ")" << name << "\", "
			 << "\"type\": " << type->toJSON() << ", ";

		if (isArray()) {
			json << "\"arraySize\": " << arraySize->toJSON() << ", ";
		}

		if (hasInitializer()) {
			json << "\"initializer\": " << initializer->toJSON() << ", ";
		}

		json << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// ParameterNode implementation
	ParameterNode::ParameterNode(
			ASTNodePtr type,
			const std::string &name,
			const lexer::SourceLocation &location) : DeclarationNode(name, location), type(std::move(type)) {
	}

	ASTNodePtr ParameterNode::getType() const {
		return type;
	}

	void ParameterNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "Parameter: " << name << std::endl;

		os << getIndent(indent + 1) << "Type:" << std::endl;
		type->dump(os, indent + 2);
	}

	std::string ParameterNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "Parameter", )"
			 << R"("name": ")" << name << "\", "
			 << "\"type\": " << type->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// FunctionDeclarationNode implementation
	FunctionDeclarationNode::FunctionDeclarationNode(
			ASTNodePtr returnType,
			const std::string &name,
			std::vector<ASTNodePtr> parameters,
			ASTNodePtr body,
			const lexer::SourceLocation &location) : DeclarationNode(name, location), returnType(std::move(returnType)),
													 parameters(std::move(parameters)), body(std::move(body)) {
	}

	ASTNodePtr FunctionDeclarationNode::getReturnType() const {
		return returnType;
	}

	const std::vector<ASTNodePtr> &FunctionDeclarationNode::getParameters() const {
		return parameters;
	}

	bool FunctionDeclarationNode::isDefinition() const {
		return body != nullptr;
	}

	ASTNodePtr FunctionDeclarationNode::getBody() const {
		return body;
	}

	void FunctionDeclarationNode::dump(std::ostream &os, int indent) const {
		if (isDefinition()) {
			os << getIndent(indent) << "FunctionDefinition: " << name << std::endl;
		} else {
			os << getIndent(indent) << "FunctionDeclaration: " << name << std::endl;
		}

		os << getIndent(indent + 1) << "Return Type:" << std::endl;
		returnType->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Parameters:" << std::endl;
		for (const auto &param: parameters) {
			param->dump(os, indent + 2);
		}

		if (isDefinition()) {
			os << getIndent(indent + 1) << "Body:" << std::endl;
			body->dump(os, indent + 2);
		}
	}

	std::string FunctionDeclarationNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": ")" << (isDefinition() ? "FunctionDefinition" : "FunctionDeclaration") << "\", "
			 << R"("name": ")" << name << "\", "
			 << "\"returnType\": " << returnType->toJSON() << ", "
			 << "\"parameters\": [";

		for (size_t i = 0; i < parameters.size(); ++i) {
			json << parameters[i]->toJSON();
			if (i < parameters.size() - 1) {
				json << ", ";
			}
		}

		json << "]";

		if (isDefinition()) {
			json << ", \"body\": " << body->toJSON();
		}

		json << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// StructDeclarationNode implementation
	StructDeclarationNode::StructDeclarationNode(
			const std::string &name,
			std::vector<ASTNodePtr> fields,
			const lexer::SourceLocation &location) : DeclarationNode(name, location), fields(std::move(fields)) {
	}

	bool StructDeclarationNode::isDefinition() const {
		return !fields.empty();
	}

	const std::vector<ASTNodePtr> &StructDeclarationNode::getFields() const {
		return fields;
	}

	void StructDeclarationNode::dump(std::ostream &os, int indent) const {
		if (isDefinition()) {
			os << getIndent(indent) << "StructDefinition: " << name << std::endl;

			os << getIndent(indent + 1) << "Fields:" << std::endl;
			for (const auto &field: fields) {
				field->dump(os, indent + 2);
			}
		} else {
			os << getIndent(indent) << "StructDeclaration: " << name << std::endl;
		}
	}

	std::string StructDeclarationNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": ")" << (isDefinition() ? "StructDefinition" : "StructDeclaration") << "\", "
			 << R"("name": ")" << name << "\"";

		if (isDefinition()) {
			json << ", \"fields\": [";

			for (size_t i = 0; i < fields.size(); ++i) {
				json << fields[i]->toJSON();
				if (i < fields.size() - 1) {
					json << ", ";
				}
			}

			json << "]";
		}

		json << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// FunctionPointerDeclarationNode implementation
	FunctionPointerDeclarationNode::FunctionPointerDeclarationNode(
			ASTNodePtr returnType,
			const std::string &name,
			std::vector<ASTNodePtr> parameterTypes,
			const lexer::SourceLocation &location) : DeclarationNode(name, location), returnType(std::move(returnType)),
													 parameterTypes(std::move(parameterTypes)) {
	}

	ASTNodePtr FunctionPointerDeclarationNode::getReturnType() const {
		return returnType;
	}

	const std::vector<ASTNodePtr> &FunctionPointerDeclarationNode::getParameterTypes() const {
		return parameterTypes;
	}

	void FunctionPointerDeclarationNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "FunctionPointerDeclaration: " << name << std::endl;

		os << getIndent(indent + 1) << "Return Type:" << std::endl;
		returnType->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Parameter Types:" << std::endl;
		for (const auto &type: parameterTypes) {
			type->dump(os, indent + 2);
		}
	}

	std::string FunctionPointerDeclarationNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "FunctionPointerDeclaration", )"
			 << R"("name": ")" << name << "\", "
			 << "\"returnType\": " << returnType->toJSON() << ", "
			 << "\"parameterTypes\": [";

		for (size_t i = 0; i < parameterTypes.size(); ++i) {
			json << parameterTypes[i]->toJSON();
			if (i < parameterTypes.size() - 1) {
				json << ", ";
			}
		}

		json << "]"
			 << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

} // namespace tinyc::ast
