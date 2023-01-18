#pragma once
#include "lexer.hpp"
#include <optional>
#include <memory>
namespace Parser
{
    class Node;
    class NodeIf;
    class NodeGoto;
    class NodeBinOperator;
    class NodeNumber;

    class Visitor
    {
    public:
        virtual void visitNodeIf(Parser::NodeIf &node) = 0;
        virtual void visitNodeGoto(Parser::NodeGoto &node) = 0;
        virtual void visitBinOperator(Parser::NodeBinOperator &node) = 0;
        virtual void visitNode(Parser::Node &node){};
        virtual void visitNodeNumber(Parser::NodeNumber &node) = 0;
    };

    class Node
    {
    public:
        Lexer::Token token;
        std::string value;
        Node(){};
        Node(std::string value) : value(value){};
        virtual void accept(Visitor &v){};
    };

    class NodeExpression : public Node
    {
    public:
        NodeExpression(){};
        virtual void accept(Visitor &v) override
        {
            v.visitNode(*this);
        };
    };

    class NodeStatement : public Node
    {
    public:
        virtual ~NodeStatement() = default;
        virtual void accept(Visitor &v) = 0;
    };

    class NodeIf : public NodeStatement
    {
    public:
        std::unique_ptr<NodeExpression> condition;
        std::unique_ptr<NodeStatement> thenStatement;
        std::optional<std::unique_ptr<NodeStatement>> elseStatement;
        NodeIf(std::unique_ptr<NodeExpression> condition, std::unique_ptr<NodeStatement> thenStatement, std::optional<std::unique_ptr<NodeStatement>> elseStatement) : condition(std::move(condition)), thenStatement(std::move(thenStatement)), elseStatement(std::move(elseStatement)){};
        void accept(Visitor &v) override
        {
            v.visitNodeIf(*this);
        };
    };

    class NodeGoto : public NodeStatement
    {
    public:
        std::string label;
        NodeGoto(std::string label) : label(label){};
        void accept(Visitor &v) override
        {
            v.visitNodeGoto(*this);
        };
    };

    class NodeBinOperator : public NodeExpression
    {
    public:
        std::unique_ptr<NodeExpression> left;
        std::unique_ptr<NodeExpression> right;
        std::string op;
        NodeBinOperator(std::unique_ptr<NodeExpression> left, std::unique_ptr<NodeExpression> right, std::string op) : left(std::move(left)), right(std::move(right)), op(op){};
        virtual void accept(Visitor &v) override
        {
            v.visitBinOperator(*this);
        };
    };

    class NodeNumber : public NodeExpression
    {
    public:
        int value;
        NodeNumber(int value) : value(value){};
        virtual void accept(Visitor &v) override
        {
            v.visitNodeNumber(*this);
        };
    };

    std::unique_ptr<NodeStatement> parseStatement(Lexer::TokenStream &ts);
    std::unique_ptr<NodeExpression> parseExpression(Lexer::TokenStream &ts);

}