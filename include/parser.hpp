#pragma once
#include "lexer.hpp"
#include <optional>
#include <memory>
#include <unordered_set>
namespace Parser
{
    class Node;
    class NodeIf;
    class NodeGoto;
    class NodeBinOperator;
    class NodeUnaryOperator;
    class NodeNumber;
    class NodeVariableDeclaration;
    class NodeVariableAssignment;
    class NodeBlockModifier;
    class NodeText;
    class NodeMultiBlock;
    class NodeReturn;
    class NodeFunction;
    class NodeFunctionCall;
    class NodePragma;

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
        virtual void visitNodeText(Parser::NodeText &node) = 0;
        virtual void visitNodeReturn(Parser::NodeReturn &node) = 0;
        virtual void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node) = 0;
        virtual void visitNodeFunction(Parser::NodeFunction &node) = 0;
        virtual void visitNodeFunctionCall(Parser::NodeFunctionCall &node) = 0;
        virtual void visitNodePragma(Parser::NodePragma &node) = 0;
        virtual void enterNode(Parser::Node &node){};
    };

    class unexpectedTokenException : public std::exception
    {
    public:
        Lexer::Token token;
        std::optional<Lexer::TokenType> expected;
        unexpectedTokenException(Lexer::Token token, std::optional<Lexer::TokenType> expected = std::nullopt) : token(token), expected(expected){};
        virtual const char *what() const throw()
        {
            std::string expectedString = "";
            if (expected.has_value())
                expectedString = "Expected " + Lexer::tokenTypeToString(expected.value());
            return ("Unexpected token " + Lexer::tokenTypeToString(token.type) + " " + token.value + " at " + std::to_string(token.line) + ":" + std::to_string(token.column) + " " + expectedString).c_str();
        }
    };

    bool hasError();
    using NodeIdentifierIndex = int;
    class NodeIdentifier;

    extern std::map<NodeIdentifierIndex, std::shared_ptr<Node>> nodes;
    class NodeIdentifier
    {
    public:
        NodeIdentifierIndex id;
        NodeIdentifier(NodeIdentifierIndex id) : id(id){};
        NodeIdentifier() : id(-1){};
        template <typename NodeType = Node>
        std::shared_ptr<NodeType> get()
        {
            if (id == -1)
                return nullptr;
            return std::dynamic_pointer_cast<NodeType>(nodes[id]);
        };
        std::shared_ptr<Node> operator->()
        {
            return get();
        }
    };
    // NodeIdentifier addNode(std::shared_ptr<Node> node);

    class Node
    {
    public:
        std::string symbol_name;
        std::optional<Lexer::Token> token;
        NodeIdentifier thisNode;
        Node() : token(std::nullopt) {}
        Node(Lexer::Token token) : token(token){};
        void setSymbolName(std::string symbol_name)
        {
            this->symbol_name = symbol_name;
        }
        virtual void accept(Visitor &v){
            // v.enterNode(*this);
        };
        virtual void enter(Visitor &v)
        {
            v.enterNode(*this);
        };
        virtual ~Node() = default;
    };

    class NodeExpression : public Node
    {
    public:
        std::string type;
        NodeExpression(){};
        NodeExpression(Lexer::Token token) : Node(token){};
        virtual void accept(Visitor &v) override
        {
            Node::enter(v);
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
            Node::enter(v);
            v.visitNodeBlockModifier(*this);
        };
    };

    class NodeBlock : public Node
    {
    public:
        NodeBlock(){};
        NodeBlock(Lexer::Token token) : Node(token){};
        std::optional<NodeIdentifier> modifier;
        virtual void accept(Visitor &v)
        {
            Node::enter(v);
            if (modifier.has_value())
                modifier.value().get()->accept(v);
        };
    };

    class NodeMultiBlock : public NodeBlock
    {
    public:
        std::vector<NodeIdentifier> blocks;
        NodeMultiBlock(std::vector<NodeIdentifier> blocks) : blocks(blocks){};
        virtual void accept(Visitor &v)
        {
            NodeBlock::accept(v);
            for (auto &block : blocks)
            {
                block.get()->accept(v);
            }
        };
    };

    class NodeStatement : public NodeBlock
    {
    public:
        virtual ~NodeStatement() = default;
        NodeStatement(){};
        NodeStatement(Lexer::Token token) : NodeBlock(token){};
        virtual void accept(Visitor &v)
        {
            NodeBlock::accept(v);
        };
    };

    class NodeIf : public NodeBlock
    {
    public:
        NodeIdentifier condition;
        NodeIdentifier thenStatement;
        Lexer::Token fiToken;
        std::optional<NodeIdentifier> elseStatement;
        NodeIf(Lexer::Token token, Lexer::Token fiToken, NodeIdentifier condition, NodeIdentifier thenStatement, std::optional<NodeIdentifier> elseStatement)
            : NodeBlock(token), condition(condition), thenStatement(thenStatement), elseStatement(elseStatement), fiToken(fiToken){};
        void accept(Visitor &v) override
        {
            NodeBlock::accept(v);
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
            NodeStatement::accept(v);
            v.visitNodeGoto(*this);
        };
    };

    class NodeReturn : public NodeStatement
    {
    public:
        std::optional<NodeIdentifier> value;
        NodeReturn(Lexer::Token token, std::optional<NodeIdentifier> value) : NodeStatement(token), value(value){};
        NodeReturn() = default;
        void accept(Visitor &v) override
        {
            NodeStatement::accept(v);
            v.visitNodeReturn(*this);
        };
    };

    class NodeBinOperator : public NodeExpression
    {
    public:
        NodeIdentifier left;
        NodeIdentifier right;
        Lexer::TokenType op;
        NodeBinOperator(NodeIdentifier left, NodeIdentifier right, Lexer::TokenType op) : left(left), right(right), op(op){};
        virtual void accept(Visitor &v) override
        {
            NodeExpression::accept(v);
            v.visitBinOperator(*this);
        };

        bool isLazyOperator()
        {
            return op == Lexer::TokenType::LOGICAL_AND || op == Lexer::TokenType::LOGICAL_OR;
        }

        bool isBooleanOperator()
        {
            std::unordered_set<Lexer::TokenType> booleanOperators{
                Lexer::TokenType::LOGICAL_AND,
                Lexer::TokenType::LOGICAL_OR,
                Lexer::TokenType::LOGICAL_XOR,
                Lexer::TokenType::LOGICAL_NOT};
            return booleanOperators.find(op) != booleanOperators.end();
        }

        bool isComparisonOperator()
        {
            std::unordered_set<Lexer::TokenType> comparisonOperators{
                Lexer::TokenType::OPERATOR_LT,
                Lexer::TokenType::OPERATOR_GT,
                Lexer::TokenType::OPERATOR_EQ,
                Lexer::TokenType::OPERATOR_LE,
                Lexer::TokenType::OPERATOR_GE,
                Lexer::TokenType::OPERATOR_NE};
            return comparisonOperators.find(op) != comparisonOperators.end();
        }
    };

    class NodeUnaryOperator : public NodeExpression
    {
    public:
        NodeIdentifier right;
        Lexer::TokenType op;
        NodeUnaryOperator(Lexer::Token token, NodeIdentifier right, Lexer::TokenType op) : NodeExpression(token), right(right), op(op){};
        virtual void accept(Visitor &v) override
        {
            NodeExpression::accept(v);
            v.visitNodeUnaryOperator(*this);
        };
    };

    class NodeNumber : public NodeExpression
    {
    public:
        int value;
        NodeNumber(int value, Lexer::Token token) : NodeExpression(token), value(value) {}
        virtual void accept(Visitor &v) override
        {
            NodeExpression::accept(v);
            v.visitNodeNumber(*this);
        };
    };

    class NodeText : public NodeExpression
    {
    public:
        std::string name;
        NodeText(std::string name, Lexer::Token token) : NodeExpression(token), name(name){};
        virtual void accept(Visitor &v) override
        {
            v.visitNodeText(*this);
        };
    };

    class NodeVariableDeclaration : public NodeStatement
    {
    public:
        std::string type;
        std::string name;
        std::optional<NodeIdentifier> value;
        NodeVariableDeclaration(Lexer::Token token, std::string type, std::string name, std::optional<NodeIdentifier> value) : NodeStatement(token), type(type), name(name)
        {
            if (value.has_value())
                this->value = std::move(value.value());
        };
        void accept(Visitor &v) override
        {
            NodeStatement::accept(v);
            v.visitNodeVariableDeclaration(*this);
        };
    };

    class NodeVariableAssignment : public NodeStatement
    {
    public:
        std::string name;
        NodeIdentifier value;
        NodeVariableAssignment(Lexer::Token token, std::string name, NodeIdentifier value) : NodeStatement(token), name(name), value(value){};
        void accept(Visitor &v) override
        {
            NodeStatement::accept(v);
            v.visitNodeVariableAssignment(*this);
        };
    };

    class NodeFunctionCall : public NodeExpression
    {
    public:
        std::string name;
        std::vector<NodeIdentifier> arguments;
        NodeFunctionCall(Lexer::Token token, std::string name, std::vector<NodeIdentifier> arguments) : NodeExpression(token), name(name), arguments(arguments){};
        void accept(Visitor &v) override
        {
            v.visitNodeFunctionCall(*this);
        };
    };

    class NodeFunction : public NodeBlock
    {
    public:
        std::string name;
        std::vector<std::pair<std::string, std::string>> arguments;
        std::optional<std::string> returnType;
        std::optional<NodeIdentifier> body;
        NodeFunction(Lexer::Token token, std::string name, std::vector<std::pair<std::string, std::string>> arguments, std::optional<std::string> returnType, std::optional<NodeIdentifier> body) : NodeBlock(token), name(name), arguments(arguments), returnType(returnType)
        {
            this->symbol_name = name;
            this->body = body;
        }

        void accept(Visitor &v) override
        {
            NodeBlock::accept(v);
            v.visitNodeFunction(*this);
        };
    };

    class NodePragma : public NodeBlock
    {
    public:
        Lexer::TokenType pragmaType;
        std::string value;
        Lexer::Token targetObject;
        NodePragma(Lexer::Token token, Lexer::TokenType pragmaType, std::string value, Lexer::Token targetObject) : NodeBlock(token), pragmaType(pragmaType), value(value), targetObject(targetObject){};

        void accept(Visitor &v) override
        {
            NodeBlock::accept(v);
            v.visitNodePragma(*this);
        };
    };

    NodeIdentifier parseStatement(Lexer::TokenStream &ts);
    NodeIdentifier parseExpression(Lexer::TokenStream &ts);
    NodeIdentifier parseBlock(Lexer::TokenStream &ts);
    NodeIdentifier parseVariableDeclaration(Lexer::TokenStream &ts);
    NodeIdentifier parseVariableAssignment(Lexer::TokenStream &ts);
    NodeIdentifier parsePrecedence(Lexer::TokenStream &ts, int precedenceIndex = Lexer::precedenceList.size() - 1);
    NodeIdentifier parseMultiBlock(Lexer::TokenStream &ts);

}