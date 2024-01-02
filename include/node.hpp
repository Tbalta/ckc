#pragma once
#include "lexer.hpp"
#include <optional>
#include <memory>
#include <unordered_set>
namespace Parser
{
    using NodeIdentifierIndex = int;

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
    class NodeCast;
    class NodePartial;
    class NodeMultiBlockExpression;

    class Visitor
    {
    public:
        virtual void visitNodeIf(Parser::NodeIf &node);
        virtual void visitNodeGoto(Parser::NodeGoto &node);
        virtual void visitBinOperator(Parser::NodeBinOperator &node);
        virtual void visitNode(Parser::Node &node);
        virtual void visitNodeNumber(Parser::NodeNumber &node);
        virtual void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node);
        virtual void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node);
        virtual void visitNodeBlockModifier(Parser::NodeBlockModifier &node);
        virtual void visitNodeText(Parser::NodeText &node);
        virtual void visitNodeReturn(Parser::NodeReturn &node);
        virtual void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node);
        virtual void visitNodeFunction(Parser::NodeFunction &node);
        virtual void visitNodeFunctionCall(Parser::NodeFunctionCall &node);
        virtual void visitNodePragma(Parser::NodePragma &node);
        virtual void enterNode(Parser::Node &node);
        virtual void visitNodeCast(Parser::NodeCast &node);
        virtual void visitNodePartial(Parser::NodePartial &node);
        virtual void visitNodeMultiBlock(Parser::NodeMultiBlock &node);
        virtual void visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node);
    };

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

    class Node
    {
    public:
        std::optional<std::string> symbol_name;
        std::optional<Lexer::Token> firstToken;
        std::optional<Lexer::Token> lastToken;
        NodeIdentifier thisNode;

        Node() : firstToken(std::nullopt), lastToken(std::nullopt) {}
        Node(Lexer::Token token) : firstToken(token), lastToken(token){};
        Node(Lexer::Token firstToken, Lexer::Token lastToken) : firstToken(firstToken), lastToken(lastToken){};
        void setSymbolName(std::string symbol_name);
        virtual void accept(Visitor &v);
        virtual void enter(Visitor &v);
        virtual ~Node() = default;
        std::shared_ptr<Node> clone() {
            return std::make_shared<Node>(*this);
        }
    };

    class NodeExpression : public Node
    {
    public:
        std::string type;
        NodeExpression(){};
        NodeExpression(Lexer::Token token) : Node(token){};
        NodeExpression(Lexer::Token firstToken, Lexer::Token lastToken) : Node(firstToken, lastToken){};
        virtual void accept(Visitor &v) override;
        std::shared_ptr<Node> clone() {
            return std::make_shared<NodeExpression>(*this);
        }
    };

    class NodeBlockModifier : public Node
    {
    public:
        Lexer::ModifierType modifier_type;
        std::string modifier_value;
        NodeBlockModifier(Lexer::ModifierType modifier_type, std::string modifier_value) : modifier_type(modifier_type), modifier_value(modifier_value){};
        virtual void accept(Visitor &v) override;
        std::shared_ptr<NodeBlockModifier> clone() {
            return std::make_shared<NodeBlockModifier>(*this);
        }

    };

    class NodeBlock : public Node
    {
    public:
        NodeBlock(){};
        NodeBlock(Lexer::Token token) : Node(token){};
        NodeBlock(Lexer::Token firstToken, Lexer::Token lastToken) : Node(firstToken, lastToken){};
        std::optional<NodeIdentifier> modifier;
        virtual void accept(Visitor &v) override;
        std::shared_ptr<Node> clone() {
            return std::make_shared<NodeBlock>(*this);
        }
    };

    class NodeMultiBlock : public NodeBlock
    {
    public:
        std::vector<NodeIdentifier> blocks;
        NodeMultiBlock(std::vector<NodeIdentifier> blocks) : blocks(blocks){};
        virtual void accept(Visitor &v);
        std::shared_ptr<NodeMultiBlock> clone() {
            return std::make_shared<NodeMultiBlock>(*this);
        }
    };

    class NodeStatement : public NodeBlock
    {
    public:
        virtual ~NodeStatement() = default;
        NodeStatement(){};
        NodeStatement(Lexer::Token token) : NodeBlock(token){};
        virtual void accept(Visitor &v);
        std::shared_ptr<Node> clone() {
            return std::make_shared<NodeStatement>(*this);
        }
    };

    class NodeIf : public NodeBlock
    {
    public:
        NodeIdentifier condition;
        NodeIdentifier thenStatement;
        std::optional<NodeIdentifier> elseStatement;
        NodeIf(Lexer::Token token, Lexer::Token fiToken, NodeIdentifier condition, NodeIdentifier thenStatement, std::optional<NodeIdentifier> elseStatement)
            : NodeBlock(token, fiToken), condition(condition), thenStatement(thenStatement), elseStatement(elseStatement){};
        virtual void accept(Visitor &v);
        std::shared_ptr<NodeIf> clone() {
            return std::make_shared<NodeIf>(*this);
        }
    };

    class NodeGoto : public NodeStatement
    {
    public:
        std::string label;
        NodeGoto(std::string label) : label(label){};
        void accept(Visitor &v) override;
        std::shared_ptr<NodeGoto> clone() {
            return std::make_shared<NodeGoto>(*this);
        }
    };

    class NodeReturn : public NodeStatement
    {
    public:
        std::optional<NodeIdentifier> value;
        NodeReturn(Lexer::Token token, std::optional<NodeIdentifier> value) : NodeStatement(token), value(value){};
        NodeReturn() = default;
        void accept(Visitor &v) override;
        std::shared_ptr<NodeReturn> clone() {
            return std::make_shared<NodeReturn>(*this);
        }
    };

    class NodeBinOperator : public NodeExpression
    {
    public:
        NodeIdentifier left;
        NodeIdentifier right;
        Lexer::TokenType op;
        NodeBinOperator(NodeIdentifier left, NodeIdentifier right, Lexer::TokenType op) : left(left), right(right), op(op){};
        virtual void accept(Visitor &v) override;

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
        std::shared_ptr<NodeBinOperator> clone() {
            return std::make_shared<NodeBinOperator>(*this);
        }
    };

    class NodeUnaryOperator : public NodeExpression
    {
    public:
        NodeIdentifier right;
        Lexer::TokenType op;
        NodeUnaryOperator(Lexer::Token token, NodeIdentifier right, Lexer::TokenType op) : NodeExpression(token), right(right), op(op){};
        virtual void accept(Visitor &v) override;
        std::shared_ptr<NodeUnaryOperator> clone() {
            return std::make_shared<NodeUnaryOperator>(*this);
        }
    };

    class NodeNumber : public NodeExpression
    {
    public:
        int value;
        NodeNumber(int value, Lexer::Token token) : NodeExpression(token), value(value) {}
        virtual void accept(Visitor &v) override;
        std::shared_ptr<NodeNumber> clone() {
            return std::make_shared<NodeNumber>(*this);
        }
    };

    class NodeText : public NodeExpression
    {
    public:
        std::string name;
        NodeText(std::string name, Lexer::Token token) : NodeExpression(token), name(name){};
        virtual void accept(Visitor &v) override;
        std::shared_ptr<Node> clone() {
            return std::make_shared<NodeText>(*this);
        }
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
        void accept(Visitor &v) override;
        std::shared_ptr<NodeVariableDeclaration> clone() {
            return std::make_shared<NodeVariableDeclaration>(*this);
        }
    };

    class NodeVariableAssignment : public NodeStatement
    {
    public:
        std::string name;
        NodeIdentifier value;
        NodeVariableAssignment(Lexer::Token token, std::string name, NodeIdentifier value) : NodeStatement(token), name(name), value(value){};
        void accept(Visitor &v) override;
        std::shared_ptr<NodeVariableAssignment> clone() {
            return std::make_shared<NodeVariableAssignment>(*this);
        }
    };

    class NodeFunctionCall : public NodeExpression
    {
    public:
        std::string name;
        std::vector<NodeIdentifier> arguments;
        NodeFunctionCall(Lexer::Token token, std::string name, std::vector<NodeIdentifier> arguments, Lexer::Token closeParen) : NodeExpression(token, closeParen), name(name), arguments(arguments){};
        void accept(Visitor &v) override;
        std::shared_ptr<NodeFunctionCall> clone() {
            return std::make_shared<NodeFunctionCall>(*this);
        }
    };

    class NodeFunction : public NodeBlock
    {
    public:
        std::string name;
        std::vector<std::pair<std::string, std::string>> arguments;
        std::optional<std::string> returnType;
        std::optional<NodeIdentifier> body;
        Lexer::Token endfunctionToken;
        NodeFunction(Lexer::Token token, std::string name, std::vector<std::pair<std::string, std::string>> arguments, std::optional<std::string> returnType, std::optional<NodeIdentifier> body, Lexer::Token endfunctionToken) : NodeBlock(token), name(name), arguments(arguments), returnType(returnType), endfunctionToken(endfunctionToken)
        {
            // this->symbol_name = name;
            this->body = body;
        }

        void accept(Visitor &v) override;
        std::shared_ptr<NodeFunction> clone() {
            return std::make_shared<NodeFunction>(*this);
        }
    };

    class NodePartial : public NodeStatement
    {
    public:
        std::string name;
        std::vector<std::pair<std::string, std::string>> arguments;
        NodeIdentifier linkedFunction;
        NodePartial(Lexer::Token token, std::string name, std::vector<std::pair<std::string, std::string>> arguments, NodeIdentifier linkedFunction) : NodeStatement(token), name(name), arguments(arguments), linkedFunction(linkedFunction){};
        void accept(Visitor &v) override;
        std::shared_ptr<NodePartial> clone() {
            return std::make_shared<NodePartial>(*this);
        }
    };

    class NodeCast : public NodeExpression
    {
    public:
        NodeIdentifier value;
        NodeCast(Lexer::Token token, std::string type, NodeIdentifier value, Lexer::Token closeParen) : NodeExpression(token, closeParen), value(value)
        {
            this->type = type;
        };
        void accept(Visitor &v) override;
        std::shared_ptr<NodeCast> clone() {
            return std::make_shared<NodeCast>(*this);
        }
    };

    class NodePragma : public NodeBlock
    {
    public:
        Lexer::TokenType pragmaType;
        std::string value;
        Lexer::Token targetObject;
        NodePragma(Lexer::Token token, Lexer::TokenType pragmaType, std::string value, Lexer::Token targetObject) : NodeBlock(token), pragmaType(pragmaType), value(value), targetObject(targetObject){};
        void accept(Visitor &v) override;
        std::shared_ptr<NodePragma> clone() {
            return std::make_shared<NodePragma>(*this);
        }
    };

    class NodeMultiBlockExpression : public NodeExpression
    {
    public:
        std::vector<NodeIdentifier> blocks;
        void accept(Visitor &v) override;
        NodeMultiBlockExpression(std::vector<NodeIdentifier> blocks) : blocks(blocks){};
        std::shared_ptr<NodeMultiBlockExpression> clone() {
            return std::make_shared<NodeMultiBlockExpression>(*this);
        }
    };

}