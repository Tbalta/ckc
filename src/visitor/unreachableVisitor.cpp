#include "visitor/unreachableVisitor.hpp"

namespace visitor
{
    void unreachableVisitor::visitNodeMultiBlock(Parser::NodeMultiBlock &node)
    {

        size_t i = 0;
        for (; i < node.blocks.size(); i++)
        {
            node.blocks[i]->accept(*this);
            if (node.blocks[i]->breakFlowControl)
            {
                node.breakFlowControl = true;
                break;
            }
        }
        for (i++; i < node.blocks.size(); i++)
        {
            unreachableNodes.push_back(node.blocks[i]);
        }
    }

    void unreachableVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        
        node.thenStatement.get()->accept(*this);

        if (node.elseStatement.has_value())
        {
            node.elseStatement.value().get()->accept(*this);
            node.breakFlowControl = node.thenStatement->breakFlowControl && node.elseStatement.value()->breakFlowControl;
        }

    }

    void unreachableVisitor::visitNodeReturn(Parser::NodeReturn &node)
    {
        node.breakFlowControl = true;
    }

    void unreachableVisitor::visitNodeGoto(Parser::NodeGoto &node)
    {
        node.breakFlowControl = true;
    }
}