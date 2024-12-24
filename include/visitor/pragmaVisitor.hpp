/**
 * @file pragmaVisitor.hpp
 * @brief Set the correct node attributes from pragma
 * @version 0.1
 * @date 2024-12-24
 */
#pragma once

#include "../genericContext.hpp"
#include "visitor/baseVisitor.hpp"

namespace visitor
{

    class pragmaVisitor : public baseVisitor
    {
    public:
        void visitNodeIf(Parser::NodeIf &node) override;
        void visitNodeGoto(Parser::NodeGoto &node) override;
        void visitNodeBinOperator(Parser::NodeBinOperator &node) override;
        void visitNode(Parser::Node &node) override;
        void visitNodeNumber(Parser::NodeNumber &node) override;
        void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node) override;
        void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node) override;
        void visitNodeBlockModifier(Parser::NodeBlockModifier &node) override;
        void visitNodeText(Parser::NodeText &node) override;
        void visitNodeReturn(Parser::NodeReturn &node) override;
        void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node) override;
        void visitNodeFunction(Parser::NodeFunction &node) override;
        void visitNodeFunctionCall(Parser::NodeFunctionCall &node) override;
        void visitNodePragma(Parser::NodePragma &node) override;
        void visitNodeCast(Parser::NodeCast &node) override;
        void visitNodePartial(Parser::NodePartial &node) override;
        
    private:
        genericContext<std::string, Parser::Node *> pragmaContext;
    };
}