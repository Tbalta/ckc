#include "node.hpp"

namespace Parser
{

    bool compareNodes(const NodeIdentifier &a, const NodeIdentifier &b)
    {
        if (a.get() == nullptr || b.get() == nullptr)
            return a.get() == b.get();
        return a->operator==(b);
    }

    static bool compareNodes(const std::optional<NodeIdentifier> &a, const std::optional<NodeIdentifier> &b)
    {
        if (a.has_value() != b.has_value())
            return false;
        if (!a.has_value())
            return true;
        return a.value()->operator==(b.value());
    }

    void Node::setSymbolName(std::string name)
    {
        this->symbol_name = name;
    }

    void Node::accept(Visitor &v)
    {
        v.visitNode(*this);
    }

    void Node::enter(Visitor &v)
    {
        v.enterNode(*this);
    };

    bool Node::operator==(const NodeIdentifier other) const
    {
        return firstToken == other->firstToken &&
               lastToken == other->lastToken;
    }

    void NodeExpression::accept(Visitor &v)
    {
        Node::enter(v);
        v.visitNode(*this);
    };

    bool NodeExpression::operator==(const NodeIdentifier other) const
    {
        auto otherExpression = other.get<NodeExpression>();
        if (otherExpression == nullptr)
            return false;

        return Node::operator==(other) &&
               type == otherExpression->type;
    }

    void NodeBlockModifier::accept(Visitor &v)
    {
        Node::enter(v);
        v.visitNodeBlockModifier(*this);
    };

    bool NodeBlockModifier::operator==(const NodeIdentifier other) const
    {
        auto otherBlockModifier = other.get<NodeBlockModifier>();
        if (otherBlockModifier == nullptr)
            return false;

        return modifier_type == otherBlockModifier->modifier_type &&
               modifier_value == otherBlockModifier->modifier_value;
    }

    void NodeBlock::accept(Visitor &v)
    {
        Node::enter(v);
        if (modifier.has_value())
            modifier.value().get()->accept(v);
    };

    bool NodeBlock::operator==(const NodeIdentifier other) const
    {
        auto otherBlock = other.get<NodeBlock>();
        if (otherBlock == nullptr)
            return false;

        return Node::operator==(other) &&
               modifier.has_value() == otherBlock->modifier.has_value() &&
               (!modifier.has_value() || modifier.value()->operator==(otherBlock->modifier.value()->thisNode));
    }

    void NodeMultiBlock::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodeMultiBlock(*this);
    };

    bool NodeMultiBlock::operator==(const NodeIdentifier other) const
    {
        auto otherMultiBlock = other.get<NodeMultiBlock>();
        if (otherMultiBlock == nullptr)
            return false;

        if (!NodeBlock::operator==(other))
            return false;
        for (size_t i = 0; i < blocks.size(); i++)
        {
            if (!blocks[i]->operator==(otherMultiBlock->blocks[i]))
                return false;
        }
        return true;
    }

    void NodeStatement::accept(Visitor &v)
    {
        NodeBlock::accept(v);
    };

    bool NodeStatement::operator==(const NodeIdentifier other) const
    {
        return NodeBlock::operator==(other);
    }

    void NodeIf::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodeIf(*this);
    };

    bool NodeIf::operator==(const NodeIdentifier other) const
    {
        auto otherIf = other.get<NodeIf>();
        if (otherIf == nullptr)
            return false;

        return NodeBlock::operator==(other) &&
               fiToken == otherIf->fiToken &&
               compareNodes(condition, otherIf->condition) &&
               compareNodes(thenStatement, otherIf->thenStatement) &&
               compareNodes(elseStatement, otherIf->elseStatement);
    }

    void NodeGoto::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodeGoto(*this);
    };

    bool NodeGoto::operator==(const NodeIdentifier other) const
    {
        auto otherGoto = other.get<NodeGoto>();
        if (otherGoto == nullptr)
            return false;

        return NodeStatement::operator==(other) &&
               label == otherGoto->label;
    }

    void NodeReturn::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodeReturn(*this);
    };

    bool NodeReturn::operator==(const NodeIdentifier other) const
    {
        auto otherReturn = other.get<NodeReturn>();
        if (otherReturn == nullptr)
            return false;

        return NodeStatement::operator==(other) &&
               compareNodes(value, otherReturn->value);
    }

    void NodeBinOperator::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeBinOperator(*this);
    };

    bool NodeBinOperator::operator==(const NodeIdentifier other) const
    {
        auto otherBinOperator = other.get<NodeBinOperator>();
        if (otherBinOperator == nullptr)
            return false;

        return NodeExpression::operator==(other) &&
               op == otherBinOperator->op &&
               compareNodes(left, otherBinOperator->left) &&
               compareNodes(right, otherBinOperator->right);
    }

    void NodeUnaryOperator::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeUnaryOperator(*this);
    };

    bool NodeUnaryOperator::operator==(const NodeIdentifier other) const
    {
        auto otherUnaryOperator = other.get<NodeUnaryOperator>();
        if (otherUnaryOperator == nullptr)
            return false;

        return NodeExpression::operator==(other) &&
               op == otherUnaryOperator->op &&
               compareNodes(right, otherUnaryOperator->right);
    }

    void NodeNumber::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeNumber(*this);
    };

    bool NodeNumber::operator==(const NodeIdentifier other) const
    {
        auto otherNumber = other.get<NodeNumber>();
        if (otherNumber == nullptr)
            return false;

        return NodeExpression::operator==(other) &&
               value == otherNumber->value;
    }

    void NodeText::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeText(*this);
    };

    bool NodeText::operator==(const NodeIdentifier other) const
    {
        auto otherText = other.get<NodeText>();
        if (otherText == nullptr)
            return false;

        return NodeExpression::operator==(other) &&
               name == otherText->name;
    }

    void NodeVariableDeclaration::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodeVariableDeclaration(*this);
    };

    bool NodeVariableDeclaration::operator==(const NodeIdentifier other) const
    {
        auto otherVariableDeclaration = other.get<NodeVariableDeclaration>();
        if (otherVariableDeclaration == nullptr)
            return false;

        return NodeStatement::operator==(other) &&
               type == otherVariableDeclaration->type &&
               name == otherVariableDeclaration->name &&
               compareNodes(value, otherVariableDeclaration->value);
    }

    void NodeVariableAssignment::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodeVariableAssignment(*this);
    };

    bool NodeVariableAssignment::operator==(const NodeIdentifier other) const
    {
        auto otherVariableAssignment = other.get<NodeVariableAssignment>();
        if (otherVariableAssignment == nullptr)
            return false;

        return NodeStatement::operator==(other) &&
               name == otherVariableAssignment->name &&
               compareNodes(value, otherVariableAssignment->value);
    }

    void NodeFunctionCall::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeFunctionCall(*this);
    };

    bool NodeFunctionCall::operator==(const NodeIdentifier other) const
    {
        auto otherFunctionCall = other.get<NodeFunctionCall>();
        if (otherFunctionCall == nullptr)
            return false;

        if (arguments.size() != otherFunctionCall->arguments.size())
            return false;

        for (size_t i = 0; i < arguments.size(); i++)
        {
            if (!arguments[i]->operator==(otherFunctionCall->arguments[i]))
                return false;
        }

        return NodeExpression::operator==(other) &&
               name == otherFunctionCall->name;
    }

    void NodeFunction::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodeFunction(*this);
    };

    bool NodeFunction::operator==(const NodeIdentifier other) const
    {
        auto otherFunction = other.get<NodeFunction>();
        if (otherFunction == nullptr)
            return false;

        if (arguments.size() != otherFunction->arguments.size())
            return false;

        for (size_t i = 0; i < arguments.size(); i++)
        {
            if (arguments[i].first != otherFunction->arguments[i].first ||
                arguments[i].second != otherFunction->arguments[i].second)
                return false;
        }

        return NodeBlock::operator==(other) &&
               name == otherFunction->name &&
               returnType == otherFunction->returnType &&
               compareNodes(body, otherFunction->body) &&
               endfunctionToken == otherFunction->endfunctionToken;
    }

    void NodePartial::accept(Visitor &v)
    {
        NodeStatement::accept(v);
        v.visitNodePartial(*this);
    };

    bool NodePartial::operator==(const NodeIdentifier other) const
    {
        auto otherPartial = other.get<NodePartial>();
        if (otherPartial == nullptr)
            return false;

        if (arguments.size() != otherPartial->arguments.size())
            return false;

        for (size_t i = 0; i < arguments.size(); i++)
        {
            if (arguments[i].first != otherPartial->arguments[i].first ||
                arguments[i].second != otherPartial->arguments[i].second)
                return false;
        }

        return NodeStatement::operator==(other) &&
               name == otherPartial->name &&
               compareNodes(linkedFunction, otherPartial->linkedFunction);
    }

    void NodeCast::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeCast(*this);
    };

    bool NodeCast::operator==(const NodeIdentifier other) const
    {
        auto otherCast = other.get<NodeCast>();
        if (otherCast == nullptr)
            return false;

        return NodeExpression::operator==(other) &&
               type == otherCast->type &&
               compareNodes(value, otherCast->value);
    }

    void NodePragma::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodePragma(*this);
    };

    bool NodePragma::operator==(const NodeIdentifier other) const
    {
        auto otherPragma = other.get<NodePragma>();
        if (otherPragma == nullptr)
            return false;

        return NodeBlock::operator==(other);
    }

    bool NodeFor::operator==(const NodeIdentifier other) const
    {
        auto otherFor = other.get<NodeFor>();
        if (otherFor == nullptr)
            return false;

        return NodeBlock::operator==(other) &&
               compareNodes(initialiser, otherFor->initialiser) &&
               compareNodes(condition, otherFor->condition) &&
               compareNodes(increment, otherFor->increment);
    }

    void NodeFor::accept(Visitor &v)
    {
        NodeBlock::accept(v);
        v.visitNodeFor(*this);
    };

    void NodeMultiBlockExpression::accept(Visitor &v)
    {
        NodeExpression::accept(v);
        v.visitNodeMultiBlockExpression(*this);
    };

    bool NodeMultiBlockExpression::operator==(const NodeIdentifier other) const
    {
        auto otherMultiBlockExpression = other.get<NodeMultiBlockExpression>();
        if (otherMultiBlockExpression == nullptr)
            return false;

        if (blocks.size() != otherMultiBlockExpression->blocks.size())
            return false;

        for (size_t i = 0; i < blocks.size(); i++)
        {
            if (!blocks[i]->operator==(otherMultiBlockExpression->blocks[i]))
                return false;
        }

        return NodeExpression::operator==(other);
    }
}