#include "visitor/baseVisitor.hpp"

namespace visitor
{

    void baseVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        node.condition->accept(*this);
        node.thenStatement->accept(*this);
        if (node.elseStatement.has_value())
            node.elseStatement.value()->accept(*this);
    }

    void baseVisitor::visitNodeGoto(Parser::NodeGoto &node) {}
    void baseVisitor::visitNodeBinOperator(Parser::NodeBinOperator &node)
    {
        node.left->accept(*this);
        node.right->accept(*this);
    }
    void baseVisitor::visitNode(Parser::Node &node) {}
    void baseVisitor::visitNodeNumber(Parser::NodeNumber &node) {}
    void baseVisitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
    {
        if (node.value.has_value())
            node.value.value()->accept(*this);
    }

    void baseVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
    {
        node.value->accept(*this);
    }

    void baseVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node) {}
    void baseVisitor::visitNodeText(Parser::NodeText &node) {}
    void baseVisitor::visitNodeReturn(Parser::NodeReturn &node)
    {
        if (node.value.has_value())
            node.value.value()->accept(*this);
    }

    void baseVisitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node)
    {
        node.right->accept(*this);
    }

    void baseVisitor::visitNodeFunction(Parser::NodeFunction &node)
    {
        if (node.body.has_value())
            node.body.value()->accept(*this);
    }

    void baseVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        for (auto &arg : node.arguments)
        {
            arg.get()->accept(*this);
        }
    }

    void baseVisitor::visitNodePragma(Parser::NodePragma &node) {}
    void baseVisitor::enterNode(Parser::Node &node) {}
    void baseVisitor::visitNodeCast(Parser::NodeCast &node)
    {
        node.value->accept(*this);
    }
    void baseVisitor::visitNodePartial(Parser::NodePartial &node)
    {
    }

    void baseVisitor::visitNodeMultiBlock(Parser::NodeMultiBlock &node)
    {
        for (auto &block : node.blocks)
        {
            block.get()->accept(*this);
        }
    }

    void baseVisitor::visitNodeFor(Parser::NodeFor &node)
    {
        if (node.initialiser.has_value())
            node.initialiser.value()->accept(*this);
        if (node.condition.has_value())
            node.condition.value()->accept(*this);
        if (node.increment.has_value())
            node.increment.value()->accept(*this);
        node.body->accept(*this);
    }

    void baseVisitor::visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node)
    {
        for (auto &block : node.blocks)
        {
            block.get()->accept(*this);
        }
    }
}