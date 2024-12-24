/**
 * @file mapVisitor.hpp
 * @brief Replace each node by the result of mapFunction
 * @version 0.1
 * @date 2024-12-24
 */
#pragma once

#include <functional>
#include <vector>
#include "visitor/baseVisitor.hpp"


namespace visitor
{
    class mapVisitor : public Parser::Visitor
    {
    protected:
        Parser::NodeIdentifier newNode;
        std::function<Parser::NodeIdentifier(Parser::Node &)> mapFunction = [](Parser::Node &node)
        { return node.thisNode; };

    public:
        mapVisitor() = default;
        mapVisitor(std::function<Parser::NodeIdentifier(Parser::Node &)> mapFunction) : mapFunction(mapFunction) {};
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
        void enterNode(Parser::Node &node) override;
        void visitNodeCast(Parser::NodeCast &node) override;
        void visitNodeMultiBlock(Parser::NodeMultiBlock &node) override;
        void visitNodeFor(Parser::NodeFor &node) override;
        void visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node) override;
        void visitNodePartial(Parser::NodePartial &node) override;
    };
}