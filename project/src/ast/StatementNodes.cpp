#include "tinyc/ast/StatementNodes.h"
#include <sstream>
#include <utility>


namespace tinyc::ast {

	// BlockStatementNode implementation
	BlockStatementNode::BlockStatementNode(
			std::vector<ASTNodePtr> statements,
			const lexer::SourceLocation &location) : StatementNode(location), statements(std::move(statements)) {
	}

	const std::vector<ASTNodePtr> &BlockStatementNode::getStatements() const {
		return statements;
	}

	void BlockStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "BlockStatement:" << std::endl;

		for (const auto &stmt: statements) {
			stmt->dump(os, indent + 1);
		}
	}

	std::string BlockStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "BlockStatement", )"
			 << "\"statements\": [";

		for (size_t i = 0; i < statements.size(); ++i) {
			json << statements[i]->toJSON();
			if (i < statements.size() - 1) {
				json << ", ";
			}
		}

		json << "]"
			 << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// ExpressionStatementNode implementation
	ExpressionStatementNode::ExpressionStatementNode(
			ASTNodePtr expression,
			const lexer::SourceLocation &location) : StatementNode(location), expression(std::move(expression)) {
	}

	ASTNodePtr ExpressionStatementNode::getExpression() const {
		return expression;
	}

	void ExpressionStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "ExpressionStatement:" << std::endl;
		expression->dump(os, indent + 1);
	}

	std::string ExpressionStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "ExpressionStatement", )"
			 << "\"expression\": " << expression->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// IfStatementNode implementation
	IfStatementNode::IfStatementNode(
			ASTNodePtr condition,
			ASTNodePtr thenBranch,
			ASTNodePtr elseBranch,
			const lexer::SourceLocation &location) : StatementNode(location), condition(std::move(condition)),
													 thenBranch(std::move(thenBranch)),
													 elseBranch(std::move(elseBranch)) {
	}

	ASTNodePtr IfStatementNode::getCondition() const {
		return condition;
	}

	ASTNodePtr IfStatementNode::getThenBranch() const {
		return thenBranch;
	}

	bool IfStatementNode::hasElseBranch() const {
		return elseBranch != nullptr;
	}

	ASTNodePtr IfStatementNode::getElseBranch() const {
		return elseBranch;
	}

	void IfStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "IfStatement:" << std::endl;

		os << getIndent(indent + 1) << "Condition:" << std::endl;
		condition->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Then:" << std::endl;
		thenBranch->dump(os, indent + 2);

		if (hasElseBranch()) {
			os << getIndent(indent + 1) << "Else:" << std::endl;
			elseBranch->dump(os, indent + 2);
		}
	}

	std::string IfStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "IfStatement", )"
			 << "\"condition\": " << condition->toJSON() << ", "
			 << "\"thenBranch\": " << thenBranch->toJSON();

		if (hasElseBranch()) {
			json << ", \"elseBranch\": " << elseBranch->toJSON();
		}

		json << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// WhileStatementNode implementation
	WhileStatementNode::WhileStatementNode(
			ASTNodePtr condition,
			ASTNodePtr body,
			const lexer::SourceLocation &location) : StatementNode(location), condition(std::move(condition)),
													 body(std::move(body)) {
	}

	ASTNodePtr WhileStatementNode::getCondition() const {
		return condition;
	}

	ASTNodePtr WhileStatementNode::getBody() const {
		return body;
	}

	void WhileStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "WhileStatement:" << std::endl;

		os << getIndent(indent + 1) << "Condition:" << std::endl;
		condition->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Body:" << std::endl;
		body->dump(os, indent + 2);
	}

	std::string WhileStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "WhileStatement", )"
			 << "\"condition\": " << condition->toJSON() << ", "
			 << "\"body\": " << body->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// DoWhileStatementNode implementation
	DoWhileStatementNode::DoWhileStatementNode(
			ASTNodePtr body,
			ASTNodePtr condition,
			const lexer::SourceLocation &location) : StatementNode(location), body(std::move(body)),
													 condition(std::move(condition)) {
	}

	ASTNodePtr DoWhileStatementNode::getBody() const {
		return body;
	}

	ASTNodePtr DoWhileStatementNode::getCondition() const {
		return condition;
	}

	void DoWhileStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "DoWhileStatement:" << std::endl;

		os << getIndent(indent + 1) << "Body:" << std::endl;
		body->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Condition:" << std::endl;
		condition->dump(os, indent + 2);
	}

	std::string DoWhileStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "DoWhileStatement", )"
			 << "\"body\": " << body->toJSON() << ", "
			 << "\"condition\": " << condition->toJSON() << ", "
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// ForStatementNode implementation
	ForStatementNode::ForStatementNode(
			ASTNodePtr initialization,
			ASTNodePtr condition,
			ASTNodePtr update,
			ASTNodePtr body,
			const lexer::SourceLocation &location) : StatementNode(location), initialization(std::move(initialization)),
													 condition(std::move(condition)), update(std::move(update)),
													 body(std::move(body)) {
	}

	bool ForStatementNode::hasInitialization() const {
		return initialization != nullptr;
	}

	ASTNodePtr ForStatementNode::getInitialization() const {
		return initialization;
	}

	bool ForStatementNode::hasCondition() const {
		return condition != nullptr;
	}

	ASTNodePtr ForStatementNode::getCondition() const {
		return condition;
	}

	bool ForStatementNode::hasUpdate() const {
		return update != nullptr;
	}

	ASTNodePtr ForStatementNode::getUpdate() const {
		return update;
	}

	ASTNodePtr ForStatementNode::getBody() const {
		return body;
	}

	void ForStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "ForStatement:" << std::endl;

		if (hasInitialization()) {
			os << getIndent(indent + 1) << "Initialization:" << std::endl;
			initialization->dump(os, indent + 2);
		}

		if (hasCondition()) {
			os << getIndent(indent + 1) << "Condition:" << std::endl;
			condition->dump(os, indent + 2);
		}

		if (hasUpdate()) {
			os << getIndent(indent + 1) << "Update:" << std::endl;
			update->dump(os, indent + 2);
		}

		os << getIndent(indent + 1) << "Body:" << std::endl;
		body->dump(os, indent + 2);
	}

	std::string ForStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "ForStatement")";

		if (hasInitialization()) {
			json << ", \"initialization\": " << initialization->toJSON();
		}

		if (hasCondition()) {
			json << ", \"condition\": " << condition->toJSON();
		}

		if (hasUpdate()) {
			json << ", \"update\": " << update->toJSON();
		}

		json << ", \"body\": " << body->toJSON()
			 << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// SwitchStatementNode implementation
	SwitchStatementNode::SwitchStatementNode(
			ASTNodePtr expression,
			std::vector<Case> cases,
			const lexer::SourceLocation &location) : StatementNode(location), expression(std::move(expression)),
													 cases(std::move(cases)) {
	}

	ASTNodePtr SwitchStatementNode::getExpression() const {
		return expression;
	}

	const std::vector<SwitchStatementNode::Case> &SwitchStatementNode::getCases() const {
		return cases;
	}

	void SwitchStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "SwitchStatement:" << std::endl;

		os << getIndent(indent + 1) << "Expression:" << std::endl;
		expression->dump(os, indent + 2);

		os << getIndent(indent + 1) << "Cases:" << std::endl;
		for (const auto &caseItem: cases) {
			if (caseItem.isDefault) {
				os << getIndent(indent + 2) << "DefaultCase:" << std::endl;
			} else {
				os << getIndent(indent + 2) << "Case: " << caseItem.value << std::endl;
			}

			for (const auto &stmt: caseItem.body) {
				stmt->dump(os, indent + 3);
			}
		}
	}

	std::string SwitchStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "SwitchStatement", )"
			 << "\"expression\": " << expression->toJSON() << ", "
			 << "\"cases\": [";

		for (size_t i = 0; i < cases.size(); ++i) {
			const auto &caseItem = cases[i];

			json << "{"
				 << "\"isDefault\": " << (caseItem.isDefault ? "true" : "false");

			if (!caseItem.isDefault) {
				json << ", \"value\": " << caseItem.value;
			}

			json << ", \"body\": [";

			for (size_t j = 0; j < caseItem.body.size(); ++j) {
				json << caseItem.body[j]->toJSON();
				if (j < caseItem.body.size() - 1) {
					json << ", ";
				}
			}

			json << "]}";

			if (i < cases.size() - 1) {
				json << ", ";
			}
		}

		json << "]"
			 << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// BreakStatementNode implementation
	BreakStatementNode::BreakStatementNode(const lexer::SourceLocation &location)
			: StatementNode(location) {
	}

	void BreakStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "BreakStatement" << std::endl;
	}

	std::string BreakStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "BreakStatement", )"
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// ContinueStatementNode implementation
	ContinueStatementNode::ContinueStatementNode(const lexer::SourceLocation &location)
			: StatementNode(location) {
	}

	void ContinueStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "ContinueStatement" << std::endl;
	}

	std::string ContinueStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "ContinueStatement", )"
			 << R"("location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

	// ReturnStatementNode implementation
	ReturnStatementNode::ReturnStatementNode(
			ASTNodePtr expression,
			const lexer::SourceLocation &location) : StatementNode(location), expression(std::move(expression)) {
	}

	bool ReturnStatementNode::hasValue() const {
		return expression != nullptr;
	}

	ASTNodePtr ReturnStatementNode::getExpression() const {
		return expression;
	}

	void ReturnStatementNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "ReturnStatement:" << std::endl;

		if (hasValue()) {
			expression->dump(os, indent + 1);
		} else {
			os << getIndent(indent + 1) << "<void>" << std::endl;
		}
	}

	std::string ReturnStatementNode::toJSON() const {
		std::ostringstream json;

		json << "{"
			 << R"("nodeType": "ReturnStatement")";

		if (hasValue()) {
			json << ", \"expression\": " << expression->toJSON();
		}

		json << R"(, "location": ")" << getLocation() << "\""
			 << "}";

		return json.str();
	}

} // namespace tinyc::ast
