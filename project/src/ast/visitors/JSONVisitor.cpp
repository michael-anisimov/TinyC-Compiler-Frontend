#include "tinyc/ast/visitors/JSONVisitor.h"
#include "tinyc/ast/ASTNode.h"
#include <sstream>

namespace tinyc::ast {

	JSONVisitor::JSONVisitor(bool prettyPrint) : indentLevel(0), prettyPrint(prettyPrint) {}

	std::string JSONVisitor::getJSON() const {
		return json.str();
	}

	std::string JSONVisitor::getIndent() const {
		if (!prettyPrint) {
			return "";
		}
		// 2 spaces per indent level
		return std::string(indentLevel * 2, ' ');
	}

	void JSONVisitor::increaseIndent() {
		indentLevel++;
	}

	void JSONVisitor::decreaseIndent() {
		if (indentLevel > 0) indentLevel--;
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
		if (prettyPrint) json << "\n";
		increaseIndent();
	}

	void JSONVisitor::endArray() {
		decreaseIndent();
		if (prettyPrint) {
			json << getIndent();
		}
		json << "]";
		json << ",";
		if (prettyPrint) json << "\n";
	}

	void JSONVisitor::addField(const std::string &name, const std::string &value) {
		json << getIndent() << "\"" << name << "\": \"" << escapeString(value) << "\"";
		json << ",";
		if (prettyPrint) json << "\n";
	}

	void JSONVisitor::addBooleanField(const std::string& name, bool value) {
		json << getIndent() << "\"" << name << "\": " << (value ? "true" : "false");
		json << ",";
		if (prettyPrint) json << "\n";
	}

	void JSONVisitor::addNumberField(const std::string& name, int value) {
		json << getIndent() << "\"" << name << "\": " << value;
		json << ",";
		if (prettyPrint) json << "\n";
	}

	void JSONVisitor::addNumberField(const std::string& name, double value) {
		json << getIndent() << "\"" << name << "\": " << value;
		json << ",";
		if (prettyPrint) json << "\n";
	}

	void JSONVisitor::addNodeField(const std::string &name, const ASTNode &node) {
		json << getIndent() << "\"" << name << "\": ";
		node.accept(*this);
		json << ",";
		if (prettyPrint) json << "\n";
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

	void JSONVisitor::addLocationField(const lexer::SourceLocation &location) {
		json << getIndent() << "\"" << "location" << "\": {";
		if (prettyPrint) json << "\n";
		increaseIndent();

		// Printing location as an JSON object
		json << getIndent() << R"("filename": ")" << escapeString(location.filename) << "\",";
		if (prettyPrint) json << "\n";

		json << getIndent() << "\"line\": " << location.line << ",";
		if (prettyPrint) json << "\n";

		json << getIndent() << "\"column\": " << location.column;
		if (prettyPrint) json << "\n";

		decreaseIndent();
		json << getIndent() << "}";
	}

	// Program node
	void JSONVisitor::visit(const ProgramNode &node) {
		startObject();

		addField("nodeType", "Program");


		// Process declarations
		startArray("declarations");
		if (!node.getDeclarations().empty()) {
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
		}
		endArray();

		// Location
		addLocationField(node.getLocation());

		endObject();
	}

	// Declaration nodes
	void JSONVisitor::visit(const VariableNode &node) {
		startObject();

		addField("nodeType", "VariableDeclaration");
		addField("identifier", node.getIdentifier());

		// Add type
		addNodeField("type", *node.getType());

		// Add array size if present
		if (node.isArray()) {
			addNodeField("arraySize", *node.getArraySize());
		}

		// Add initializer if present
		if (node.hasInitializer()) {
			addNodeField("initializer", *node.getInitializer());
		}

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const MultipleDeclarationNode& node) {
		startObject();

		addField("nodeType", "MultipleDeclaration");

		// Process declarations
		startArray("declarations");
		if (!node.getDeclarations().empty()) {
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
		}
		endArray();

		addLocationField(node.getLocation());

		endObject();
	}


	void JSONVisitor::visit(const ParameterNode &node) {
		startObject();

		addField("nodeType", "Parameter");
		addField("identifier", node.getIdentifier());
		addNodeField("type", *node.getType());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const FunctionDeclarationNode &node) {
		startObject();

		addField("nodeType", node.isDefinition() ? "FunctionDefinition" : "FunctionDeclaration");
		addField("identifier", node.getIdentifier());

		// Return type
		addNodeField("returnType", *node.getReturnType());

		// Parameters
		startArray("parameters");
		if (!node.getParameters().empty()) {
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
		}

		endArray();

		// Body (for definitions)
		if (node.isDefinition()) {
			addNodeField("body", *node.getBody());
		}

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const StructDeclarationNode &node) {
		startObject();

		addField("nodeType", node.isDefinition() ? "StructDefinition" : "StructDeclaration");
		addField("identifier", node.getIdentifier());

		// Fields (for definitions)
		startArray("fields");
		if (node.isDefinition() && !node.getFields().empty()) {
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
		}
		endArray();

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const FunctionPointerDeclarationNode &node) {
		startObject();

		addField("nodeType", "FunctionPointerDeclaration");
		addField("identifier", node.getIdentifier());

		// Return type
		addNodeField("returnType", *node.getReturnType());

		// Parameter types
		startArray("parameterTypes");
		if (!node.getParameterTypes().empty()) {

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
		}
		endArray();

		addLocationField(node.getLocation());

		endObject();
	}

	// Type nodes
	void JSONVisitor::visit(const PrimitiveTypeNode &node) {
		startObject();

		addField("nodeType", "PrimitiveType");
		addField("kind", node.getKindString());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const NamedTypeNode &node) {
		startObject();

		addField("nodeType", "NamedType");
		addField("identifier", node.getIdentifier());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const PointerTypeNode &node) {
		startObject();

		addField("nodeType", "PointerType");
		addNodeField("baseType", *node.getBaseType());
		addLocationField(node.getLocation());

		endObject();
	}

	// Expression nodes
	void JSONVisitor::visit(const LiteralNode &node) {
		startObject();

		addField("nodeType", "Literal");
		addField("kind", node.getKindString());

		if (node.getKind() == LiteralNode::Kind::INTEGER) {
			try {
				int intValue = std::stoi(node.getValue());
				addNumberField("value", intValue);
			} catch (const std::exception&) {
				// If conversion fails, fall back to string representation
				addField("value", node.getValue());
			}
		} else if (node.getKind() == LiteralNode::Kind::DOUBLE) {
			try {
				double doubleValue = std::stod(node.getValue());
				addNumberField("value", doubleValue);
			} catch (const std::exception&) {
				// If conversion fails, fall back to string representation
				addField("value", node.getValue());
			}
		} else {
			// For character and string literals, keep as strings
			addField("value", node.getValue());
		}

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const IdentifierNode &node) {
		startObject();

		addField("nodeType", "Identifier");
		addField("identifier", node.getIdentifier());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const BinaryExpressionNode &node) {
		startObject();

		addField("nodeType", "BinaryExpression");
		addField("operator", node.getOperatorString());
		addNodeField("left", *node.getLeft());
		addNodeField("right", *node.getRight());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const UnaryExpressionNode &node) {
		startObject();

		addField("nodeType", "UnaryExpression");
		// Special handling for increment/decrement operators in JSON
		std::string opStr;
		switch (node.getOperator()) {
			case UnaryExpressionNode::Operator::PRE_INCREMENT:
			case UnaryExpressionNode::Operator::POST_INCREMENT:
				opStr = "++";
				break;
			case UnaryExpressionNode::Operator::PRE_DECREMENT:
			case UnaryExpressionNode::Operator::POST_DECREMENT:
				opStr = "--";
				break;
			default:
				opStr = node.getOperatorString();
				break;
		}

		addField("operator", opStr);
		addBooleanField("prefix", node.isPrefix());
		addNodeField("operand", *node.getOperand());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const CastExpressionNode &node) {
		startObject();

		addField("nodeType", "CastExpression");
		addNodeField("targetType", *node.getTargetType());
		addNodeField("expression", *node.getExpression());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const CallExpressionNode &node) {
		startObject();

		addField("nodeType", "CallExpression");
		addNodeField("callee", *node.getCallee());

		// Arguments
		startArray("arguments");
		if (!node.getArguments().empty()) {

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
		}
		endArray();

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const IndexExpressionNode &node) {
		startObject();

		addField("nodeType", "IndexExpression");
		addNodeField("array", *node.getArray());
		addNodeField("index", *node.getIndex());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const MemberExpressionNode &node) {
		startObject();

		addField("nodeType", "MemberExpression");
		addField("kind", node.getKind() == MemberExpressionNode::Kind::DOT ? "dot" : "arrow");
		addNodeField("object", *node.getObject());
		addField("member", node.getMember());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const CommaExpressionNode &node) {
		startObject();

		addField("nodeType", "CommaExpression");

		// Expressions
		startArray("expressions");
		if (!node.getExpressions().empty()) {

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
		}
		endArray();

		addLocationField(node.getLocation());

		endObject();
	}

	// Statement nodes
	void JSONVisitor::visit(const BlockStatementNode &node) {
		startObject();

		addField("nodeType", "BlockStatement");

		// Statements
		startArray("statements");
		if (!node.getStatements().empty()) {

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
		}
		endArray();

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const ExpressionStatementNode &node) {
		startObject();

		addField("nodeType", "ExpressionStatement");
		addNodeField("expression", *node.getExpression());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const IfStatementNode &node) {
		startObject();

		addField("nodeType", "IfStatement");
		addNodeField("condition", *node.getCondition());
		addNodeField("thenBranch", *node.getThenBranch());

		if (node.hasElseBranch()) {
			addNodeField("elseBranch", *node.getElseBranch());
		}

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const WhileStatementNode &node) {
		startObject();

		addField("nodeType", "WhileStatement");
		addNodeField("condition", *node.getCondition());
		addNodeField("body", *node.getBody());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const DoWhileStatementNode &node) {
		startObject();

		addField("nodeType", "DoWhileStatement");
		addNodeField("body", *node.getBody());
		addNodeField("condition", *node.getCondition());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const ForStatementNode &node) {
		startObject();

		addField("nodeType", "ForStatement");

		if (node.hasInitialization()) {
			addNodeField("initialization", *node.getInitialization());
		}

		if (node.hasCondition()) {
			addNodeField("condition", *node.getCondition());
		}

		if (node.hasUpdate()) {
			addNodeField("update", *node.getUpdate());
		}

		addNodeField("body", *node.getBody());
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const SwitchStatementNode &node) {
		startObject();

		addField("nodeType", "SwitchStatement");
		addNodeField("expression", *node.getExpression());

		// Cases
		startArray("cases");
		if (!node.getCases().empty()) {
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
		}
		endArray();

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const BreakStatementNode &node) {
		startObject();

		addField("nodeType", "BreakStatement");
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const ContinueStatementNode &node) {
		startObject();

		addField("nodeType", "ContinueStatement");
		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const ReturnStatementNode &node) {
		startObject();

		addField("nodeType", "ReturnStatement");

		if (node.hasValue()) {
			addNodeField("expression", *node.getExpression());
		}

		addLocationField(node.getLocation());

		endObject();
	}

	void JSONVisitor::visit(const ErrorProgramNode& node) {
		startObject();

		addField("nodeType", "ErrorProgram");
		addField("errorType", node.getErrorTypeString());
		addField("message", node.getMessage());

		// Process declarations
		startArray("declarations");
		if (!node.getDeclarations().empty()) {
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
		}
		endArray();

		// Standard location field
		addLocationField(node.getLocation());

		endObject();
	}

} // namespace tinyc::ast