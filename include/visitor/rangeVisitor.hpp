#pragma once

#include "parser.hpp"

#include "../genericContext.hpp"
namespace visitor
{

class rangeVisitor : public Parser::Visitor
{
    public:
        void visitNode(Parser::Node &node) override;
        std::optional<Lexer::Token> firstToken;
        std::optional<Lexer::Token> lastToken;
    private:
        void setMin(Lexer::Token token);
        void setMax(Lexer::Token token);
};
}