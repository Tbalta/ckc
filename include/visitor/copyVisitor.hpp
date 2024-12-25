/**
 * @file copyVisitor.hpp
 * @brief Create a deep copy of the AST
 * @version 0.1
 * @date 2024-12-24
 */
#pragma once
#include "parser.hpp"
namespace visitor
{
    class copyVisitor : public Parser::Visitor
    {
    private:
    public:
        Parser::NodeIdentifier newCopy;
        void visitNodeIf(Parser::NodeIf &node);
        void visitNodeGoto(Parser::NodeGoto &node);
        void visitNodeBinOperator(Parser::NodeBinOperator &node);
        void visitNode(Parser::Node &node);
        void visitNodeNumber(Parser::NodeNumber &node);
        void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node);
        void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node);
        void visitNodeBlockModifier(Parser::NodeBlockModifier &node);
        void visitNodeText(Parser::NodeText &node);
        void visitNodeReturn(Parser::NodeReturn &node);
        void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node);
        void visitNodeFunction(Parser::NodeFunction &node);
        void visitNodeFunctionCall(Parser::NodeFunctionCall &node);
        void visitNodePragma(Parser::NodePragma &node);
        void enterNode(Parser::Node &node);
        void visitNodeCast(Parser::NodeCast &node);
        void visitNodePartial(Parser::NodePartial &node);
        void visitNodeMultiBlock(Parser::NodeMultiBlock &node);
        void visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node);
        void visitNodeFor(Parser::NodeFor &node);
        copyVisitor(/* args */) = default;
        ~copyVisitor() = default;
    };
    
   
}