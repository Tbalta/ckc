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
    class NodeVariableDeclaration;
    class NodeVariableAssignment;
    class NodeBlockModifier;
    class NodeIdentifier;
    class NodeMultiBlock;
    class NodeReturn;

    class Visitor
    {
    public:
        virtual void visitNodeIf(Parser::NodeIf &node) = 0;
        virtual void visitNodeGoto(Parser::NodeGoto &node) = 0;
        virtual void visitBinOperator(Parser::NodeBinOperator &node) = 0;
        virtual void visitNode(Parser::Node &node){};
        virtual void visitNodeNumber(Parser::NodeNumber &node) = 0;
        virtual void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node) = 0;
        virtual void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node) = 0;
        virtual void visitNodeBlockModifier(Parser::NodeBlockModifier &node) = 0;
        virtual void visitNodeIdentifier(Parser::NodeIdentifier &node) = 0;
        virtual void visitNodeReturn(Parser::NodeReturn &node) = 0;
    };
    bool hasError();

    class Node
    {
    public:
        Lexer::Token token;
        std::string value;
        Node(){};
        Node(std::string value) : value(value){};
        virtual void accept(Visitor &v){};
        virtual ~Node() = default;
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

    class NodeBlockModifier : public Node
    {
    public:
        Lexer::ModifierType modifier_type;
        std::string modifier_value;
        NodeBlockModifier(Lexer::ModifierType modifier_type, std::string modifier_value) : modifier_type(modifier_type), modifier_value(modifier_value){};
        virtual void accept(Visitor &v)
        {
            v.visitNodeBlockModifier(*this);
        };
    };

    class NodeBlock : public Node
    {
    public:
        std::optional<std::unique_ptr<NodeBlockModifier>> modifier;
        virtual void accept(Visitor &v) = 0;
    };

    class NodeMultiBlock : public NodeBlock
    {
    public:
        std::vector<std::unique_ptr<NodeBlock>> blocks;
        NodeMultiBlock(std::vector<std::unique_ptr<NodeBlock>> blocks) : blocks(std::move(blocks)){};
        virtual void accept(Visitor &v)
        {
            for (auto &block : blocks)
            {
                block->accept(v);
            }
        };
    };

    class NodeStatement : public NodeBlock
    {
    public:
        virtual ~NodeStatement() = default;
        virtual void accept(Visitor &v) = 0;
    };

    class NodeIf : public NodeBlock
    {
    public:
        std::unique_ptr<NodeExpression> condition;
        std::unique_ptr<NodeMultiBlock> thenStatement;
        std::optional<std::unique_ptr<NodeMultiBlock>> elseStatement;
        NodeIf(std::unique_ptr<NodeExpression> condition, std::unique_ptr<NodeMultiBlock> thenStatement, std::optional<std::unique_ptr<NodeMultiBlock>> elseStatement) : condition(std::move(condition)), thenStatement(std::move(thenStatement)), elseStatement(std::move(elseStatement)){};
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

    class NodeReturn : public NodeStatement
    {
    public:
        std::unique_ptr<NodeExpression> value;
        NodeReturn(std::unique_ptr<NodeExpression> value) : value(std::move(value)){};
        void accept(Visitor &v) override
        {
            v.visitNodeReturn(*this);
        };
    };

    class NodeBinOperator : public NodeExpression
    {
    public:
        std::unique_ptr<NodeExpression> left;
        std::unique_ptr<NodeExpression> right;
        Lexer::TokenType op;
        NodeBinOperator(std::unique_ptr<NodeExpression> left, std::unique_ptr<NodeExpression> right, Lexer::TokenType op) : left(std::move(left)), right(std::move(right)), op(op){};
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

    class NodeIdentifier : public NodeExpression
    {
    public:
        std::string name;
        NodeIdentifier(std::string name) : name(name){};
        virtual void accept(Visitor &v) override
        {
            v.visitNodeIdentifier(*this);
        };
    };

    class NodeVariableDeclaration : public NodeStatement
    {
    public:
        std::string type;
        std::string name;
        std::optional<std::unique_ptr<NodeExpression>> value;
        NodeVariableDeclaration(std::string type, std::string name, std::optional<std::unique_ptr<NodeExpression>> value) : type(type), name(name), value(std::move(value)){};
        void accept(Visitor &v) override
        {
            v.visitNodeVariableDeclaration(*this);
        };
    };

    class NodeVariableAssignment : public NodeStatement
    {
    public:
        std::string name;
        std::unique_ptr<NodeExpression> value;
        NodeVariableAssignment(std::string name, std::unique_ptr<NodeExpression> value) : name(name), value(std::move(value)){};
        void accept(Visitor &v) override
        {
            v.visitNodeVariableAssignment(*this);
        };
    };

    std::unique_ptr<NodeStatement> parseStatement(Lexer::TokenStream &ts);
    std::unique_ptr<NodeExpression> parseExpression(Lexer::TokenStream &ts);
    std::unique_ptr<NodeBlock> parseBlock(Lexer::TokenStream &ts);
    std::unique_ptr<NodeVariableDeclaration> parseVariableDeclaration(Lexer::TokenStream &ts);
    std::unique_ptr<NodeVariableAssignment> parseVariableAssignment(Lexer::TokenStream &ts);
    std::unique_ptr<NodeExpression> parsePrecedence(Lexer::TokenStream &ts, int precedenceIndex = Lexer::precedenceList.size() - 1);
}