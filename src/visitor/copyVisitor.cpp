#include "visitor/copyVisitor.hpp"
#include "parser.hpp"
namespace visitor
{
        void copyVisitor::visitNodeIf(Parser::NodeIf &node)
        {
            auto newNode=node.clone();
            newNode->condition->accept(*this);
            newNode->condition=newCopy;
            newNode->thenStatement->accept(*this);
            newNode->thenStatement=newCopy;
            if (newNode->elseStatement.has_value())
            {
                newNode->elseStatement.value()->accept(*this);
                newNode->elseStatement=newCopy;
            }
            newCopy=Parser::addNode(newNode);
        }

        void copyVisitor::visitNodeGoto(Parser::NodeGoto &node)
        {
            auto newNode=node.clone();
            newCopy=Parser::addNode(newNode);
        }

        void copyVisitor::visitBinOperator(Parser::NodeBinOperator &node)
        {
            auto newNode=node.clone();
            newNode->left->accept(*this);
            newNode->left=newCopy;
            newNode->right->accept(*this);
            newNode->right=newCopy;
            newCopy=Parser::addNode(newNode);
        }

        void copyVisitor::visitNode(Parser::Node &node)
        {
            auto newNode=node.clone();
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeNumber(Parser::NodeNumber &node)
        {
            auto newNode=node.clone();
            newCopy=Parser::addNode(newNode);
        }

        void copyVisitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
        {
            auto newNode=node.clone();
            if (newNode->value.has_value())
            {
                newNode->value.value()->accept(*this);
                newNode->value=newCopy;
            }
            newCopy=Parser::addNode(newNode);
        }

        void copyVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
        {
            auto newNode=node.clone();
            newNode->value->accept(*this);
            newNode->value=newCopy;
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node)
        {
            auto newNode=node.clone();
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeText(Parser::NodeText &node)
        {
            auto newNode=node.clone();
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeReturn(Parser::NodeReturn &node)
        {
            auto newNode=node.clone();
            if (newNode->value.has_value())
            {
                newNode->value.value()->accept(*this);
                newNode->value=newCopy;
            }
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node)
        {
            auto newNode=node.clone();
            newNode->right->accept(*this);
            newNode->right=newCopy;
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeFunction(Parser::NodeFunction &node)
        {
            auto newNode=node.clone();
            newNode->body.value()->accept(*this);
            newNode->body=newCopy;
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
        {
            auto newNode=node.clone();
            for (auto &i : newNode->arguments)
            {
                i->accept(*this);
                i=newCopy;
            }
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodePragma(Parser::NodePragma &node)
        {
            auto newNode=node.clone();
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::enterNode(Parser::Node &node)
        {
        }
        void copyVisitor::visitNodeCast(Parser::NodeCast &node)
        {
            auto newNode=node.clone();
            newNode->value->accept(*this);
            newNode->value=newCopy;
            newCopy=Parser::addNode(newNode);

        }
        void copyVisitor::visitNodePartial(Parser::NodePartial &node)
        {
            auto newNode=node.clone();
            newNode->linkedFunction->accept(*this);
            newNode->linkedFunction=newCopy;
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeMultiBlock(Parser::NodeMultiBlock &node)
        {
            auto newNode=node.clone();
            for (auto &i : newNode->blocks)
            {
                i->accept(*this);
                i=newCopy;
            }
            newCopy=Parser::addNode(newNode);
        }
        void copyVisitor::visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node)
        {
            auto newNode=node.clone();
            for (auto &i : newNode->blocks)
            {
                i->accept(*this);
                i=newCopy;
            }
            newCopy=Parser::addNode(newNode);
        }
}