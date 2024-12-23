#include "visitor/mapVisitor.hpp"

namespace visitor
{
    void mapVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        node.condition.get()->accept(*this);
        node.condition = newNode;

        node.thenStatement.get()->accept(*this);
        node.thenStatement = newNode;

        if (node.elseStatement.has_value())
        {
            node.elseStatement.value().get()->accept(*this);
            node.elseStatement.value() = newNode;
        }

        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeGoto(Parser::NodeGoto &node)
    {
        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeBinOperator(Parser::NodeBinOperator &node)
    {
        node.left.get()->accept(*this);
        node.left = newNode;

        node.right.get()->accept(*this);
        node.right = newNode;

        newNode = mapFunction(node);
    };

    void mapVisitor::visitNode(Parser::Node &node) {};
    void mapVisitor::visitNodeNumber(Parser::NodeNumber &node)
    {
        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
    {
        if (node.value.has_value())
        {
            node.value.value().get()->accept(*this);
            node.value.value() = newNode;
        }

        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
    {
        node.value.get()->accept(*this);
        node.value = newNode;

        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node)
    {
        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeText(Parser::NodeText &node)
    {
        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeReturn(Parser::NodeReturn &node)
    {
        if (node.value.has_value())
        {
            node.value.value().get()->accept(*this);
            node.value.value() = newNode;
        }

        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node)
    {
        node.right.get()->accept(*this);
        node.right = newNode;

        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeFunction(Parser::NodeFunction &node)
    {
        if (node.body.has_value())
        {
            node.body.value().get()->accept(*this);
            node.body.value() = newNode;
        }

        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        for (auto &arg : node.arguments)
        {
            arg.get()->accept(*this);
            arg = newNode;
        }

        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodePragma(Parser::NodePragma &node)
    {
        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeCast(Parser::NodeCast &node)
    {
        newNode = mapFunction(node);
    };

    void mapVisitor::visitNodeMultiBlock(Parser::NodeMultiBlock &node)
    {
        for (auto &block : node.blocks)
        {
            block->accept(*this);
            block = newNode;
        }

        newNode = mapFunction(node);
    }

    void mapVisitor::visitNodeFor(Parser::NodeFor &node)
    {
        if (node.initialiser.has_value())
        {
            node.initialiser.value()->accept(*this);
            node.initialiser.value() = newNode;
        }

        if (node.condition.has_value())
        {
            node.condition.value()->accept(*this);
            node.condition.value() = newNode;
        }

        if (node.increment.has_value())
        {
            node.increment.value()->accept(*this);
            node.increment.value() = newNode;
        }

        node.body->accept(*this);
        node.body = newNode;

        newNode = mapFunction(node);
    }

    void mapVisitor::visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node)
    {
        for (auto &block : node.blocks)
        {
            block->accept(*this);
            block = newNode;
        }

        newNode = mapFunction(node);
    }

    void mapVisitor::enterNode(Parser::Node &node)
    {
    }
}