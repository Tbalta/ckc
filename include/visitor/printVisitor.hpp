#pragma once
#include "../parser.hpp"
#include <iostream>
namespace visitor
{
    class PrintVisitor : public Parser::Visitor
    {
    public:
        void visitNodeIf(Parser::NodeIf &node) override
        {
            if (node.modifier.has_value())
                node.modifier.value()->accept(*this);
            std::cout << "if ";
            node.condition->accept(*this);
            std::cout << " then ";
            node.thenStatement->accept(*this);
            if (node.elseStatement)
            {
                std::cout << " else ";
                node.elseStatement.value()->accept(*this);
            }
            std::cout << " fi";
        }
        void visitNodeGoto(Parser::NodeGoto &node) override
        {
            std::cout << "goto " << node.label;
        }
        void visitBinOperator(Parser::NodeBinOperator &node) override
        {
            std::cout << "(";
            node.left->accept(*this);
            std::cout << " " << Lexer::tokenTypeToString(node.op) << " ";
            node.right->accept(*this);
            std::cout << ")";
        }

        void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node) override
        {
            std::cout << Lexer::tokenTypeToString(node.op) << " ";
            node.right->accept(*this);
        }

        void visitNode(Parser::Node &node) override
        {
            std::cout << node.value;
        }
        void visitNodeNumber(Parser::NodeNumber &node) override
        {
            std::cout << node.value;
        }
        virtual void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
        {
            if (node.modifier.has_value())
                node.modifier.value()->accept(*this);
            std::cout << node.type << " " << node.name;
            if (node.value)
            {
                std::cout << " = ";
                node.value.value()->accept(*this);
            }
        }

        virtual void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
        {
            if (node.modifier.has_value())
                node.modifier.value()->accept(*this);
            std::cout << node.name << " = ";
            node.value->accept(*this);
        }
        virtual void visitNodeBlockModifier(Parser::NodeBlockModifier &node)
        {
            std::cout << "named" << " " << node.modifier_value << " ";
        }
        virtual void visitNodeIdentifier(Parser::NodeIdentifier &node)
        {
            std::cout << node.name;
        }
        
        virtual void visitNodeReturn(Parser::NodeReturn &node)
        {
            std::cout << "return ";
            node.value->accept(*this);
        }
    };
}