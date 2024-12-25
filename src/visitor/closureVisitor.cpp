#include "visitor/closureVisitor.hpp"

namespace visitor
{
    std::set<Parser::NodeIdentifier> closureVisitor::getExpressionsInClosure()
    {
        return closureExpressions;
    }

    bool closureVisitor::isInClosure(Parser::NodeIdentifier &node)
    {
        this->inClosure = false;
        node->accept(*this);
        bool inClosure = this->inClosure;
        this->inClosure = false;
        return inClosure;
    }

    void closureVisitor::visit(Parser::NodeIdentifier &node)
    {
        if (isInClosure(node))
            closureExpressions.insert(node);
    }

    void closureVisitor::visitNodeBinOperator(Parser::NodeBinOperator &node)
    {
        auto leftClosure = isInClosure(node.left);
        auto rightClosure = isInClosure(node.right);
        inClosure = leftClosure && rightClosure;
        if (inClosure)
            return;
        if (leftClosure)
            closureExpressions.insert(node.left);
        if (rightClosure)
            closureExpressions.insert(node.right);
    }

    void closureVisitor::visitNodeText(Parser::NodeText &node)
    {
        inClosure = constantVariables.find(node.name) == constantVariables.end();
    }

    void closureVisitor::visitNodeNumber(Parser::NodeNumber &node)
    {
        inClosure = false;
    }

    void closureVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        std::vector<Parser::NodeIdentifier> argsInClosure;
        for (auto &arg : node.arguments)
        {
            if (isInClosure(arg))
            {
                closureExpressions.insert(arg);
            }
        }
        
        inClosure = false;
    }
}