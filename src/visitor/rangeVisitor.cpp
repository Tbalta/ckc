#include "visitor/rangeVisitor.hpp"

namespace visitor
{
    void rangeVisitor::setMin(Lexer::Token token)
    {
        if (!firstToken.has_value())
        {
            firstToken = token;
            return;
        }
        if (firstToken.value().line == token.line)
        {
            if (firstToken.value().column > token.column)
                firstToken = token;
        }
        else if (firstToken.value().line > token.line)
            firstToken = token;
    }
    void rangeVisitor::setMax(Lexer::Token token)
    {
        if (!lastToken.has_value())
        {
            lastToken = token;
            return;
        }
        if (lastToken.value().line == token.line)
        {
            if (lastToken.value().column < token.column)
                lastToken = token;
        }
        else if (lastToken.value().line < token.line)
            lastToken = token;
    }

    void rangeVisitor::visitNode(Parser::Node &node)
    {
        setMin(node.firstToken.value());
        setMax(node.lastToken.value());
    }
}