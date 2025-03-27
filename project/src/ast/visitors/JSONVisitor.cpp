#include "tinyc/ast/visitors/JSONVisitor.h"
#include "tinyc/ast/ASTNode.h"
#include <sstream>

namespace tinyc::ast {

	JSONVisitor::JSONVisitor() : indentLevel(0), prettyPrint(true) {}

	std::string JSONVisitor::getJSON() const {
		return json.str();
	}

	std::string JSONVisitor::getIndent() const {
		if (!prettyPrint) {
			return "";
		}
		return std::string(indentLevel * 2, ' ');
	}

	void JSONVisitor::increaseIndent() {
		indentLevel++;
	}

	void JSONVisitor::decreaseIndent() {
		if (indentLevel > 0) {
			indentLevel--;
		}
	}

	void JSONVisitor::startObject() {
		json << "{";
		if (prettyPrint) {
			json << "\n";
		}
		increaseIndent();
	}

	void JSONVisitor::endObject() {
		decreaseIndent();
		if (prettyPrint) {
			json << "\n" << getIndent();
		}
		json << "}";
	}

	void JSONVisitor::startArray(const std::string &name) {
		json << getIndent() << "\"" << name << "\": [";
		if (prettyPrint) {
			json << "\n";
		}
		increaseIndent();
	}

	void JSONVisitor::endArray(bool isLast) {
		decreaseIndent();
		if (prettyPrint) {
			json << getIndent();
		}
		json << "]";
		if (!isLast) {
			json << ",";
		}
		if (prettyPrint) {
			json << "\n";
		}
	}

	void JSONVisitor::addField(const std::string &name, const std::string &value, bool isLast) {
		json << getIndent() << "\"" << name << "\": \"" << escapeString(value) << "\"";
		if (!isLast) {
			json << ",";
		}
		if (prettyPrint) {
			json << "\n";
		}
	}

	void JSONVisitor::addNodeField(const std::string &name, const ASTNode &node, bool isLast) {
		json << getIndent() << "\"" << name << "\": ";
		node.accept(*this);
		if (!isLast) {
			json << ",";
		}
		if (prettyPrint) {
			json << "\n";
		}
	}

	template<typename T>
	void JSONVisitor::addNodeArrayField(const std::string &name, const std::vector<T> &nodes, bool isLast) {
		startArray(name);

		for (size_t i = 0; i < nodes.size(); ++i) {
			if (prettyPrint) {
				json << getIndent();
			}
			nodes[i]->accept(*this);
			if (i < nodes.size() - 1) {
				json << ",";
			}
			if (prettyPrint) {
				json << "\n";
			}
		}

		endArray(isLast);
	}

	std::string JSONVisitor::escapeString(const std::string &s) {
		std::string result;
		result.reserve(s.length());

		for (char c: s) {
			switch (c) {
				case '\"':
					result += "\\\"";
					break;
				case '\\':
					result += "\\\\";
					break;
				case '\b':
					result += "\\b";
					break;
				case '\f':
					result += "\\f";
					break;
				case '\n':
					result += "\\n";
					break;
				case '\r':
					result += "\\r";
					break;
				case '\t':
					result += "\\t";
					break;
				default:
					if (c < 32) {
						char buffer[7];
						snprintf(buffer, sizeof(buffer), "\\u%04x", c);
						result += buffer;
					} else {
						result += c;
					}
					break;
			}
		}

		return result;
	}

	void JSONVisitor::addLocationField(const std::string &name, const lexer::SourceLocation &location, bool isLast) {
		json << getIndent() << "\"" << name << "\": {";
		if (prettyPrint) {
			json << "\n";
		}
		increaseIndent();

		// Add filename
		json << getIndent() << "\"filename\": \"" << escapeString(location.filename) << "\",";
		if (prettyPrint) {
			json << "\n";
		}

		// Add line
		json << getIndent() << "\"line\": " << location.line << ",";
		if (prettyPrint) {
			json << "\n";
		}

		// Add column
		json << getIndent() << "\"column\": " << location.column;
		if (prettyPrint) {
			json << "\n";
		}

		decreaseIndent();
		json << getIndent() << "}";
		if (!isLast) {
			json << ",";
		}
	}

	// Program node
	void JSONVisitor::visit(const ProgramNode &node) {
		startObject();

		addField("nodeType", "Program", false);
		lexer::SourceLocation programLocation;
		if (!node.getDeclarations().empty()) {
			programLocation.filename = node.getDeclarations()[0]->getLocation().filename;
			programLocation.line = 0;      // 0 indicates whole file
			programLocation.column = 0;    // 0 indicates whole file
		}

		addLocationField("location", programLocation, false);

		// Process declarations
		if (!node.getDeclarations().empty()) {
			startArray("declarations");

			for (size_t i = 0; i < node.getDeclarations().size(); ++i) {
				if (prettyPrint) {
					json << getIndent();
				}
				node.getDeclarations()[i]->accept(*this);
				if (i < node.getDeclarations().size() - 1) {
					json << ",";
				}
				if (prettyPrint) {
					json << "\n";
				}
			}

			endArray(true);
		} else {
			addField("declarations", "[]", true);
		}

		endObject();
	}

	// Declaration nodes
	void JSONVisitor::visit(const VariableNode &node) {
		startObject();

		addField("nodeType", "VariableDeclaration", false);
		addField("identifier", node.getIdentifier(), false);

		// Add type
		addNodeField("type", *node.getType(), false);

		// Add array size if present
		if (node.isArray()) {
			addNodeField("arraySize", *node.getArraySize(), false);
		}

		// Add initializer if present
		if (node.hasInitializer()) {
			addNodeField("initializer", *node.getInitializer(), false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const ParameterNode &node) {
		startObject();

		addField("nodeType", "Parameter", false);
		addField("identifier", node.getIdentifier(), false);
		addNodeField("type", *node.getType(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const FunctionDeclarationNode &node) {
		startObject();

		addField("nodeType", node.isDefinition() ? "FunctionDefinition" : "FunctionDeclaration", false);
		addField("identifier", node.getIdentifier(), false);

		// Return type
		addNodeField("returnType", *node.getReturnType(), false);

		// Parameters
		if (!node.getParameters().empty()) {
			startArray("parameters");

			for (size_t i = 0; i < node.getParameters().size(); ++i) {
				if (prettyPrint) {
					json << getIndent();
				}
				node.getParameters()[i]->accept(*this);
				if (i < node.getParameters().size() - 1) {
					json << ",";
				}
				if (prettyPrint) {
					json << "\n";
				}
			}

			endArray(false);
		} else {
			addField("parameters", "[]", node.isDefinition() ? false : true);
		}

		// Body (for definitions)
		if (node.isDefinition()) {
			addNodeField("body", *node.getBody(), false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const StructDeclarationNode &node) {
		startObject();

		addField("nodeType", node.isDefinition() ? "StructDefinition" : "StructDeclaration", false);
		addField("identifier", node.getIdentifier(), false);

		// Fields (for definitions)
		if (node.isDefinition() && !node.getFields().empty()) {
			startArray("fields");

			for (size_t i = 0; i < node.getFields().size(); ++i) {
				if (prettyPrint) {
					json << getIndent();
				}
				node.getFields()[i]->accept(*this);
				if (i < node.getFields().size() - 1) {
					json << ",";
				}
				if (prettyPrint) {
					json << "\n";
				}
			}

			endArray(false);
		} else if (node.isDefinition()) {
			addField("fields", "[]", false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const FunctionPointerDeclarationNode &node) {
		startObject();

		addField("nodeType", "FunctionPointerDeclaration", false);
		addField("identifier", node.getIdentifier(), false);

		// Return type
		addNodeField("returnType", *node.getReturnType(), false);

		// Parameter types
		if (!node.getParameterTypes().empty()) {
			startArray("parameterTypes");

			for (size_t i = 0; i < node.getParameterTypes().size(); ++i) {
				if (prettyPrint) {
					json << getIndent();
				}
				node.getParameterTypes()[i]->accept(*this);
				if (i < node.getParameterTypes().size() - 1) {
					json << ",";
				}
				if (prettyPrint) {
					json << "\n";
				}
			}

			endArray(false);
		} else {
			addField("parameterTypes", "[]", false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	// Type nodes
	void JSONVisitor::visit(const PrimitiveTypeNode &node) {
		startObject();

		addField("nodeType", "PrimitiveType", false);
		addField("kind", node.getKindString(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const NamedTypeNode &node) {
		startObject();

		addField("nodeType", "NamedType", false);
		addField("identifier", node.getIdentifier(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const PointerTypeNode &node) {
		startObject();

		addField("nodeType", "PointerType", false);
		addNodeField("baseType", *node.getBaseType(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	// Expression nodes
	void JSONVisitor::visit(const LiteralNode &node) {
		startObject();

		addField("nodeType", "Literal", false);
		addField("kind", node.getKindString(), false);
		addField("value", node.getValue(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const IdentifierNode &node) {
		startObject();

		addField("nodeType", "Identifier", false);
		addField("identifier", node.getIdentifier(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const BinaryExpressionNode &node) {
		startObject();

		addField("nodeType", "BinaryExpression", false);
		addField("operator", node.getOperatorString(), false);
		addNodeField("left", *node.getLeft(), false);
		addNodeField("right", *node.getRight(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const UnaryExpressionNode &node) {
		startObject();

		addField("nodeType", "UnaryExpression", false);
		addField("operator", node.getOperatorString(), false);
		addField("prefix", node.isPrefix() ? "true" : "false", false);
		addNodeField("operand", *node.getOperand(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const CastExpressionNode &node) {
		startObject();

		addField("nodeType", "CastExpression", false);
		addNodeField("targetType", *node.getTargetType(), false);
		addNodeField("expression", *node.getExpression(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const CallExpressionNode &node) {
		startObject();

		addField("nodeType", "CallExpression", false);
		addNodeField("callee", *node.getCallee(), false);

		// Arguments
		if (!node.getArguments().empty()) {
			startArray("arguments");

			for (size_t i = 0; i < node.getArguments().size(); ++i) {
				if (prettyPrint) {
					json << getIndent();
				}
				node.getArguments()[i]->accept(*this);
				if (i < node.getArguments().size() - 1) {
					json << ",";
				}
				if (prettyPrint) {
					json << "\n";
				}
			}

			endArray(false);
		} else {
			addField("arguments", "[]", false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const IndexExpressionNode &node) {
		startObject();

		addField("nodeType", "IndexExpression", false);
		addNodeField("array", *node.getArray(), false);
		addNodeField("index", *node.getIndex(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const MemberExpressionNode &node) {
		startObject();

		addField("nodeType", "MemberExpression", false);
		addField("kind", node.getKind() == MemberExpressionNode::Kind::DOT ? "dot" : "arrow", false);
		addNodeField("object", *node.getObject(), false);
		addField("member", node.getMember(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const CommaExpressionNode &node) {
		startObject();

		addField("nodeType", "CommaExpression", false);

		// Expressions
		if (!node.getExpressions().empty()) {
			startArray("expressions");

			for (size_t i = 0; i < node.getExpressions().size(); ++i) {
				if (prettyPrint) {
					json << getIndent();
				}
				node.getExpressions()[i]->accept(*this);
				if (i < node.getExpressions().size() - 1) {
					json << ",";
				}
				if (prettyPrint) {
					json << "\n";
				}
			}

			endArray(false);
		} else {
			addField("expressions", "[]", false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	// Statement nodes
	void JSONVisitor::visit(const BlockStatementNode &node) {
		startObject();

		addField("nodeType", "BlockStatement", false);

		// Statements
		if (!node.getStatements().empty()) {
			startArray("statements");

			for (size_t i = 0; i < node.getStatements().size(); ++i) {
				if (prettyPrint) {
					json << getIndent();
				}
				node.getStatements()[i]->accept(*this);
				if (i < node.getStatements().size() - 1) {
					json << ",";
				}
				if (prettyPrint) {
					json << "\n";
				}
			}

			endArray(false);
		} else {
			addField("statements", "[]", false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const ExpressionStatementNode &node) {
		startObject();

		addField("nodeType", "ExpressionStatement", false);
		addNodeField("expression", *node.getExpression(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const IfStatementNode &node) {
		startObject();

		addField("nodeType", "IfStatement", false);
		addNodeField("condition", *node.getCondition(), false);
		addNodeField("thenBranch", *node.getThenBranch(), false);

		if (node.hasElseBranch()) {
			addNodeField("elseBranch", *node.getElseBranch(), false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const WhileStatementNode &node) {
		startObject();

		addField("nodeType", "WhileStatement", false);
		addNodeField("condition", *node.getCondition(), false);
		addNodeField("body", *node.getBody(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const DoWhileStatementNode &node) {
		startObject();

		addField("nodeType", "DoWhileStatement", false);
		addNodeField("body", *node.getBody(), false);
		addNodeField("condition", *node.getCondition(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const ForStatementNode &node) {
		startObject();

		addField("nodeType", "ForStatement", false);

		if (node.hasInitialization()) {
			addNodeField("initialization", *node.getInitialization(), false);
		}

		if (node.hasCondition()) {
			addNodeField("condition", *node.getCondition(), false);
		}

		if (node.hasUpdate()) {
			addNodeField("update", *node.getUpdate(), false);
		}

		addNodeField("body", *node.getBody(), false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const SwitchStatementNode &node) {
		startObject();

		addField("nodeType", "SwitchStatement", false);
		addNodeField("expression", *node.getExpression(), false);

		// Cases
		if (!node.getCases().empty()) {
			json << getIndent() << "\"cases\": [";
			if (prettyPrint) {
				json << "\n";
			}
			increaseIndent();

			for (size_t i = 0; i < node.getCases().size(); ++i) {
				const auto &caseItem = node.getCases()[i];

				json << getIndent() << "{";
				if (prettyPrint) {
					json << "\n";
				}
				increaseIndent();

				json << getIndent() << "\"isDefault\": " << (caseItem.isDefault ? "true" : "false");

				if (!caseItem.isDefault) {
					json << ",";
					if (prettyPrint) {
						json << "\n";
					}
					json << getIndent() << "\"value\": " << caseItem.value;
				}

				json << ",";
				if (prettyPrint) {
					json << "\n";
				}

				// Case body
				if (!caseItem.body.empty()) {
					json << getIndent() << "\"body\": [";
					if (prettyPrint) {
						json << "\n";
					}
					increaseIndent();

					for (size_t j = 0; j < caseItem.body.size(); ++j) {
						if (prettyPrint) {
							json << getIndent();
						}
						caseItem.body[j]->accept(*this);
						if (j < caseItem.body.size() - 1) {
							json << ",";
						}
						if (prettyPrint) {
							json << "\n";
						}
					}

					decreaseIndent();
					json << getIndent() << "]";
				} else {
					json << getIndent() << "\"body\": []";
				}

				decreaseIndent();
				if (prettyPrint) {
					json << "\n" << getIndent();
				}
				json << "}";

				if (i < node.getCases().size() - 1) {
					json << ",";
				}
				if (prettyPrint) {
					json << "\n";
				}
			}

			decreaseIndent();
			json << getIndent() << "],";
			if (prettyPrint) {
				json << "\n";
			}
		} else {
			addField("cases", "[]", false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const BreakStatementNode &node) {
		startObject();

		addField("nodeType", "BreakStatement", false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const ContinueStatementNode &node) {
		startObject();

		addField("nodeType", "ContinueStatement", false);
		addLocationField("location", node.getLocation(), true);

		endObject();
	}

	void JSONVisitor::visit(const ReturnStatementNode &node) {
		startObject();

		addField("nodeType", "ReturnStatement", false);

		if (node.hasValue()) {
			addNodeField("expression", *node.getExpression(), false);
		}

		addLocationField("location", node.getLocation(), true);

		endObject();
	}

} // namespace tinyc::ast