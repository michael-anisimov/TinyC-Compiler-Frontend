#include "tinyc/ast/ProgramNode.h"
#include <sstream>


namespace tinyc::ast {

	ProgramNode::ProgramNode() {
		// Initialize with default location
		location = lexer::SourceLocation("<unknown>", 1, 1);
	}

	void ProgramNode::addDeclaration(const ASTNodePtr &declaration) {
		// Update location to first declaration if this is the first one
		if (declarations.empty()) {
			location = declaration->getLocation();
		}

		declarations.push_back(declaration);
	}

	const std::vector<ASTNodePtr> &ProgramNode::getDeclarations() const {
		return declarations;
	}

	lexer::SourceLocation ProgramNode::getLocation() const {
		return location;
	}

	void ProgramNode::dump(std::ostream &os, int indent) const {
		os << getIndent(indent) << "Program" << std::endl;

		for (const auto &decl: declarations) {
			decl->dump(os, indent + 1);
		}
	}

	std::string ProgramNode::toJSON() const {
		std::ostringstream json;

		json << "{\n";
		json << "  \"nodeType\": \"Program\",\n";
		json << R"(  "location": ")" << location << "\",\n";
		json << "  \"declarations\": [\n\n";

		for (size_t i = 0; i < declarations.size(); ++i) {
			json << "    " << declarations[i]->toJSON();
			if (i < declarations.size() - 1) {
				json << ",";
			}
			json << "\n\n";
		}

		json << "  ]\n";
		json << "}";

		return json.str();
	}

} // namespace tinyc::ast
