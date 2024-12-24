/**
 * @file baseVisitor.hpp
 * @brief Default visit pattern for all nodes
 * @version 0.1
 * @date 2024-12-24
 */
#pragma once

#include "node.hpp"

namespace visitor
{

    class baseVisitor : public Parser::Visitor {
    public:
        virtual void visitNodeIf(Parser::NodeIf &node);
        virtual void visitNodeGoto(Parser::NodeGoto &node);
        virtual void visitNodeBinOperator(Parser::NodeBinOperator &node);
        virtual void visitNode(Parser::Node &node);
        virtual void visitNodeNumber(Parser::NodeNumber &node);
        virtual void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node);
        virtual void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node);
        virtual void visitNodeBlockModifier(Parser::NodeBlockModifier &node);
        virtual void visitNodeText(Parser::NodeText &node);
        virtual void visitNodeReturn(Parser::NodeReturn &node);
        virtual void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node);
        virtual void visitNodeFunction(Parser::NodeFunction &node);
        virtual void visitNodeFunctionCall(Parser::NodeFunctionCall &node);
        virtual void visitNodePragma(Parser::NodePragma &node);
        virtual void enterNode(Parser::Node &node);
        virtual void visitNodeCast(Parser::NodeCast &node);
        virtual void visitNodePartial(Parser::NodePartial &node);
        virtual void visitNodeMultiBlock(Parser::NodeMultiBlock &node);
        virtual void visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node);
        virtual void visitNodeFor(Parser::NodeFor &node);
    };
}