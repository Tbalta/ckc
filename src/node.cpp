#include "node.hpp"

namespace Parser
{
    void Node::setSymbolName(std::string name)
    {
        this->symbol_name = name;
    }

    void Node::accept(Visitor &visitor)
    {
    }

    void Node::enter(Visitor &v)
    {
        v.enterNode(*this);
    };

    void NodeExpression::accept(Visitor &v)
    {
        Node::enter(v);
        v.visitNode(*this);
    };

    void NodeBlockModifier::accept(Visitor &v)
    {
        Node::enter(v);
        v.visitNodeBlockModifier(*this);
    };

    void NodeBlock::accept(Visitor &v)
    {
        Node::enter(v);
        if (modifier.has_value())
            modifier.value().get()->accept(v);
    };

    void NodeMultiBlock::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodeMultiBlock(*this);
    };

    void NodeStatement::accept(Visitor &v)
    {
        NodeBlock::accept(v);
    };

    void NodeIf::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodeIf(*this);
    };

    void NodeGoto::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodeGoto(*this);
    };

    void NodeReturn::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodeReturn(*this);
    };

    void NodeBinOperator::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeBinOperator(*this);
    };

    void NodeUnaryOperator::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeUnaryOperator(*this);
    };

    void NodeNumber::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeNumber(*this);
    };

    void NodeText::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeText(*this);
    };

    void NodeVariableDeclaration::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodeVariableDeclaration(*this);
    };

    void NodeVariableAssignment::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodeVariableAssignment(*this);
    };

    void NodeFunctionCall::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeFunctionCall(*this);
    };

    void NodeFunction::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodeFunction(*this);
    };

    void NodePartial::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodePartial(*this);
    };

    void NodeCast::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeCast(*this);
    };

    void NodePragma::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodePragma(*this);
    };

    void NodeMultiBlockExpression::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeMultiBlockExpression(*this);
    };
}