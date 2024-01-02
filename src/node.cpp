#include "node.hpp"

namespace Parser
{

    void Visitor::visitNodeIf(Parser::NodeIf &node)
    {
        node.condition->accept(*this);
        node.thenStatement->accept(*this);
        if (node.elseStatement.has_value())
            node.elseStatement.value()->accept(*this);
    }

    void Visitor::visitNodeGoto(Parser::NodeGoto &node) {}
    void Visitor::visitBinOperator(Parser::NodeBinOperator &node)
    {
        node.left->accept(*this);
        node.right->accept(*this);
    }
    void Visitor::visitNode(Parser::Node &node) {}
    void Visitor::visitNodeNumber(Parser::NodeNumber &node) {}
    void Visitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
    {
        if (node.value.has_value())
            node.value.value()->accept(*this);
    }

    void Visitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
    {
        node.value->accept(*this);
    }

    void Visitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node) {}
    void Visitor::visitNodeText(Parser::NodeText &node) {}
    void Visitor::visitNodeReturn(Parser::NodeReturn &node)
    {
        if (node.value.has_value())
            node.value.value()->accept(*this);
    }

    void Visitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node)
    {
        node.right->accept(*this);
    }

    void Visitor::visitNodeFunction(Parser::NodeFunction &node)
    {
        if (node.body.has_value())
            node.body.value()->accept(*this);
    }

    void Visitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        for (auto &arg : node.arguments)
        {
            arg.get()->accept(*this);
        }
    }

    void Visitor::visitNodePragma(Parser::NodePragma &node) {}
    void Visitor::enterNode(Parser::Node &node) {}
    void Visitor::visitNodeCast(Parser::NodeCast &node)
    {
        node.value->accept(*this);
    }
    void Visitor::visitNodePartial(Parser::NodePartial &node)
    {
    }

    void Visitor::visitNodeMultiBlock(Parser::NodeMultiBlock &node)
    {
        for (auto &block : node.blocks)
        {
            block.get()->accept(*this);
        }
    }

    void Visitor::visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node)
    {
        for (auto &block : node.blocks)
        {
            block.get()->accept(*this);
        }
    }

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
        v.visitBinOperator(*this);
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