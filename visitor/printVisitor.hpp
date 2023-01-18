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
            std::cout << "if ";
            node.condition.accept(*this);
            std::cout << " then ";
            node.thenStatement.accept(*this);
            if (node.elseStatement)
            {
                std::cout << " else ";
                node.elseStatement.value().accept(*this);
            }
            std::cout << " fi";
        }
        void visitNodeGoto(Parser::NodeGoto &node) override
        {
            std::cout << "goto ";
            node.label.accept(*this);
        }
        void visitBinOperator(Parser::NodeBinOperator &node) override
        {
            node.left.accept(*this);
            std::cout << " " << node.token.value << " ";
            node.right.accept(*this);
        }
        void visitNode(Parser::Node &node) override
        {
            std::cout << node.value;
        }
        void visit(Parser::Node &node)
        {
            node.accept(*this);
        }
    };
}