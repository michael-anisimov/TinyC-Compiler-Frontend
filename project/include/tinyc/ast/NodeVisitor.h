#ifndef TINYC_AST_NODE_VISITOR_H
#define TINYC_AST_NODE_VISITOR_H

namespace tinyc::ast {

// Forward declarations of all AST node types
class ProgramNode;
class VariableNode;
class MultipleDeclarationNode;
class ParameterNode;
class FunctionDeclarationNode;
class StructDeclarationNode;
class FunctionPointerDeclarationNode;
class PrimitiveTypeNode;
class NamedTypeNode;
class PointerTypeNode;
class LiteralNode;
class IdentifierNode;
class BinaryExpressionNode;
class UnaryExpressionNode;
class CastExpressionNode;
class CallExpressionNode;
class IndexExpressionNode;
class MemberExpressionNode;
class CommaExpressionNode;
class BlockStatementNode;
class ExpressionStatementNode;
class IfStatementNode;
class WhileStatementNode;
class DoWhileStatementNode;
class ForStatementNode;
class SwitchStatementNode;
class BreakStatementNode;
class ContinueStatementNode;
class ReturnStatementNode;

/**
 * @brief The Visitor interface for AST nodes
 * 
 * This abstract class defines the interface for all node visitors.
 * Each concrete visitor should implement these methods to perform
 * specific operations on each type of AST node.
 */
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;

    // Program nodes
    virtual void visit(const ProgramNode& node) = 0;
    
    // Declaration nodes
    virtual void visit(const VariableNode& node) = 0;
	virtual void visit(const MultipleDeclarationNode& node) = 0;
    virtual void visit(const ParameterNode& node) = 0;
    virtual void visit(const FunctionDeclarationNode& node) = 0;
    virtual void visit(const StructDeclarationNode& node) = 0;
    virtual void visit(const FunctionPointerDeclarationNode& node) = 0;
    
    // Type nodes
    virtual void visit(const PrimitiveTypeNode& node) = 0;
    virtual void visit(const NamedTypeNode& node) = 0;
    virtual void visit(const PointerTypeNode& node) = 0;
    
    // Expression nodes
    virtual void visit(const LiteralNode& node) = 0;
    virtual void visit(const IdentifierNode& node) = 0;
    virtual void visit(const BinaryExpressionNode& node) = 0;
    virtual void visit(const UnaryExpressionNode& node) = 0;
    virtual void visit(const CastExpressionNode& node) = 0;
    virtual void visit(const CallExpressionNode& node) = 0;
    virtual void visit(const IndexExpressionNode& node) = 0;
    virtual void visit(const MemberExpressionNode& node) = 0;
    virtual void visit(const CommaExpressionNode& node) = 0;
    
    // Statement nodes
    virtual void visit(const BlockStatementNode& node) = 0;
    virtual void visit(const ExpressionStatementNode& node) = 0;
    virtual void visit(const IfStatementNode& node) = 0;
    virtual void visit(const WhileStatementNode& node) = 0;
    virtual void visit(const DoWhileStatementNode& node) = 0;
    virtual void visit(const ForStatementNode& node) = 0;
    virtual void visit(const SwitchStatementNode& node) = 0;
    virtual void visit(const BreakStatementNode& node) = 0;
    virtual void visit(const ContinueStatementNode& node) = 0;
    virtual void visit(const ReturnStatementNode& node) = 0;
};

} // namespace tinyc::ast

#endif // TINYC_AST_NODE_VISITOR_H