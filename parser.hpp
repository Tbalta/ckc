#pragma once
#include "lexer.hpp"
#include <optional>

namespace Parser
{
    class Node;
    class NodeIf;
    class NodeGoto;
    class NodeBinOperator;

    class Visitor
    {
    public:
        virtual void visitNodeIf(Parser::NodeIf &node) = 0;
        virtual void visitNodeGoto(Parser::NodeGoto &node) = 0;
        virtual void visitBinOperator(Parser::NodeBinOperator &node) = 0;
        virtual void visitNode(Parser::Node &node) = 0;
    };

    class Node
    {
    public:
        Lexer::Token token;
        std::string value;
        Node(){};
        Node(std::string value) : value(value){};
        virtual void accept(Visitor &v) = 0;
    };

    class NodeIf : public Node
    {
    public:
        Node condition;
        Node thenStatement;
        std::optional<Node> elseStatement;
        NodeIf(){};
        virtual void accept(Visitor &v) override
        {
            v.visitNodeIf(*this);
        };
    };

    class NodeGoto : public Node
    {
    public:
        Node label;
        NodeGoto(){};
        virtual void accept(Visitor &v) override
        {
            v.visitNodeGoto(*this);
        };
    };

    class NodeBinOperator : public Node
    {
    public:
        Node left;
        Node right;
        NodeBinOperator(){};
        virtual void accept(Visitor &v) override
        {
            v.visitBinOperator(*this);
        };
    };

    // class NodeExpression : public Node
    // {
    //     Node value;

    // public:
    //     NodeExpression(){};
    // };

    Node parse(Lexer::TokenStream &ts);

}