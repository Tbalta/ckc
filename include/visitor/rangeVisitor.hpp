#pragma once

#include "parser.hpp"

#include "../genericContext.hpp"
namespace visitor
{

class rangeVisitor : public Parser::Visitor
{
    public:
        void visitNodeIf(Parser::NodeIf &node) override;
        void visitNodeGoto(Parser::NodeGoto &node) override;
        void visitBinOperator(Parser::NodeBinOperator &node) override;
        void visitNode(Parser::Node &node) override;
        void visitNodeNumber(Parser::NodeNumber &node) override;
        void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node) override;
        void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node) override;
        void visitNodeBlockModifier(Parser::NodeBlockModifier &node) override;
        void visitNodeText(Parser::NodeText &node) override;
        void visitNodeReturn(Parser::NodeReturn &node) override;
        void visitNodeUnaryOperator (Parser::NodeUnaryOperator &node) override;
        void visitNodeFunction(Parser::NodeFunction &node) override;
        void visitNodeFunctionCall(Parser::NodeFunctionCall &node) override;
        void visitNodePragma(Parser::NodePragma &node) override;
        std::optional<Lexer::Token> firstToken;
        std::optional<Lexer::Token> lastToken;
    private:
        void setMin(Lexer::Token token);
        void setMax(Lexer::Token token);
};
}