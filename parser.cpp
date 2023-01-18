#include "parser.hpp"
#include <map>
#include <string>
#include <cassert>
#include <functional>
namespace Parser
{

    NodeIf parseIf(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        NodeIf node;
        node.token = t;
        node.condition = parse(ts);
        Node thenStr = parse(ts);
        assert(thenStr.value == "then");
        node.thenStatement = parse(ts);
        Node fiStr = parse(ts);
        if (fiStr.value == "else")
        {
            node.elseStatement = parse(ts);
            fiStr = parse(ts);
        }
        assert(fiStr.value == "fi");
        return node;
    }

    NodeGoto parseGoto(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        NodeGoto node;
        node.token = t;
        node.label = parse(ts);
        return node;
    }

    Node parseKeyword(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        if (t.value == "if")
            return parseIf(t, ts);
        if (t.value == "goto")
            return parseGoto(t, ts);
        return Node(t.value);
    }

    Node parseParenthesis(Lexer::TokenStream &ts)
    {
        Node node = parse(ts);
        Node endParenthesis = parse(ts);
        assert(endParenthesis.value == ")");
        return node;
    }

    NodeBinOperator parseBinOperator(const Lexer::Token &t)
    {
        NodeBinOperator node;
        node.token = t;
        return node;
    }

    Node parseNumber(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        Node node;
        node.token = t;
        if (ts.peek().type == Lexer::TokenType::OPERATOR)
        {
            NodeBinOperator binOp;
            binOp.value = ts.get().value;
            binOp.left = node;
            binOp.right = parse(ts);
            return binOp;
        }
        return node;
    }

    Node parseIdentifier(const Lexer::Token &t)
    {
        Node node;
        node.token = t;
        return node;
    }

    Node parse(Lexer::TokenStream &ts)
    {
        const Lexer::Token t = ts.get();
        switch (t.type)
        {
        case Lexer::TokenType::KEYWORD:
            return parseKeyword(t, ts);
        case Lexer::TokenType::OPERATOR:
            return parseBinOperator(t);
        case Lexer::TokenType::PARENTHESIS:
            return parseParenthesis(ts);
        case Lexer::TokenType::NUMBER:
            return parseNumber(t, ts);
        case Lexer::TokenType::IDENTIFIER:
            return parseIdentifier(t);
        }
    }
}