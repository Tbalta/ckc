#include "parser.hpp"
#include <map>
#include <string>
#include <cassert>
#include <functional>
namespace Parser
{

    std::unique_ptr<NodeIf> parseIf(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        auto condition = parseExpression(ts);
        std::string thenStr = ts.get().value;
        assert(thenStr == "then");
        auto thenStatement = parseStatement(ts);
        std::string fiStr = ts.get().value;
        std::optional<std::unique_ptr<NodeStatement>> elseStatement = std::nullopt;
        if (fiStr == "else")
        {
            elseStatement = parseStatement(ts);
            fiStr = ts.get().value;
        }
        assert(fiStr == "fi");
        return std::make_unique<NodeIf>(std::move(condition), std::move(thenStatement), std::move(elseStatement));
    }

    std::unique_ptr<NodeGoto> parseGoto(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        return std::make_unique<NodeGoto>(ts.get().value);
    }

    std::unique_ptr<NodeNumber> parseNumber(const Lexer::Token &t)
    {
        int value = std::stoi(t.value);
        return std::make_unique<NodeNumber>(value);
    }

    Node parseIdentifier(const Lexer::Token &t)
    {
        Node node;
        node.token = t;
        return node;
    }

    std::unique_ptr<NodeStatement> parseStatement(Lexer::TokenStream &ts)
    {
        const Lexer::Token t = ts.get();
        switch (t.type)
        {
        case Lexer::TokenType::KEYWORD_IF:
            return parseIf(t, ts);
        case Lexer::TokenType::KEYWORD_GOTO:
            return parseGoto(t, ts);
        }
    }

    std::unique_ptr<NodeExpression> parseMul(Lexer::TokenStream &ts)
    {
        Lexer::Token t = ts.peek();
        // mul -> term mul'
        std::unique_ptr<NodeExpression> term;
        if (t.type == Lexer::TokenType::NUMBER)
        {
            term = parseNumber(ts.get());
        }
        // while t is a * or / parseMul
        t = ts.peek();
        while (t.type == Lexer::TokenType::OPERATOR_MUL || t.type == Lexer::TokenType::OPERATOR_DIV)
        {
            std::string op = ts.get().value;
            std::unique_ptr<NodeExpression> right = parseNumber(ts.get());
            term = std::make_unique<NodeBinOperator>(std::move(term), std::move(right), op);
            t = ts.peek();
        }
        return term;
    }

    std::unique_ptr<NodeExpression> parseExpression(Lexer::TokenStream &ts)
    {
        std::unique_ptr<NodeExpression> left = parseMul(ts);
        while (ts.peek().type == Lexer::TokenType::OPERATOR_ADD || ts.peek().type == Lexer::TokenType::OPERATOR_SUB)
        {
            std::string op = ts.get().value;
            std::unique_ptr<NodeExpression> right = parseMul(ts);
            left = std::make_unique<NodeBinOperator>(std::move(left), std::move(right), std::move(op));
        }
        return left;
    }
}