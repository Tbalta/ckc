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
                node.modifier.value().get()->accept(*this);
            std::cout << "if ";
            node.condition.get()->accept(*this);
            std::cout << " then ";
            node.thenStatement.get()->accept(*this);
            if (node.elseStatement)
            {
                std::cout << " else ";
                node.elseStatement.value().get()->accept(*this);
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
            node.left.get()->accept(*this);
            std::cout << " " << Lexer::tokenTypeToString(node.op) << " ";
            node.right.get()->accept(*this);
            std::cout << ")";
        }

        void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node) override
        {
            std::cout << Lexer::tokenTypeToString(node.op) << " ";
            node.right.get()->accept(*this);
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
                node.modifier.value().get()->accept(*this);
            std::cout << node.type << " " << node.name;
            if (node.value)
            {
                std::cout << " = ";
                node.value.value().get()->accept(*this);
            }
        }

        virtual void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
        {
            if (node.modifier.has_value())
                node.modifier.value().get()->accept(*this);
            std::cout << node.name << " = ";
            node.value.get()->accept(*this);
        }
        virtual void visitNodeBlockModifier(Parser::NodeBlockModifier &node)
        {
            std::cout << "named" << " " << node.modifier_value << " ";
        }
        virtual void visitNodeText(Parser::NodeText &node)
        {
            std::cout << node.name;
        }
        
        virtual void visitNodeReturn(Parser::NodeReturn &node)
        {
            std::cout << "return ";
            node.value.get()->accept(*this);
        }

        virtual void visitNodeFunction (Parser::NodeFunction &node)
        {
            std::cout << "function " << node.name << " ";
            // if (node.modifier.has_value())
            //     node.modifier.value()->accept(*this);
            std::cout << "(";
            for (ssize_t i = 0; i < ssize_t(node.arguments.size()-1); i++)
            {
                std::cout << node.arguments[i].first << " " << node.arguments[i].second << ", ";
            }
            if (node.arguments.size() > 0)
                std::cout << (node.arguments.end()-1)->first << " " << (node.arguments.end()-1)->second;
            std::cout << ")";
            if (node.returnType.has_value())
                std::cout << " return " << node.returnType.value() << std::endl;
            if (node.body.has_value())
                node.body.value().get()->accept(*this);
        }

        virtual void visitNodeFunctionCall (Parser::NodeFunctionCall &node)
        {
            std::cout << node.name << "(";
            for (ssize_t i = 0; i < ssize_t (node.arguments.size()-1); i++)
            {
                node.arguments[i].get()->accept(*this);
                std::cout << ", ";
            }
            if (node.arguments.size() > 0)
                (node.arguments.end()-1)->get()->accept(*this);
            
            std::cout << ")";
        }

        virtual void visitNodePragma (Parser::NodePragma &node)
        {
            std::cout << "pragma " << Lexer::tokenTypeToString(node.pragmaType) << " ";
            std::cout << node.value << std::endl;
        }
    };
}