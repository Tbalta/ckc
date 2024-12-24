/**
 * @file rangeVisitor.hpp
 * @brief Return the first and last token of an AST
 * @version 0.1
 * @date 2024-12-24
 */
#pragma once

#include "../genericContext.hpp"
#include "visitor/baseVisitor.hpp"

namespace visitor
{

class rangeVisitor : public baseVisitor
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