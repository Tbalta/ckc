#include "parser.hpp"
#include <map>
#include <string>
#include <cassert>
#include <functional>
namespace Parser
{

    std::unique_ptr<NodeMultiBlock> parseMultiBlock(Lexer::TokenStream &ts)
    {
        std::vector<std::unique_ptr<NodeBlock>> blocks;
        while (!ts.peek().isEndMultiBlock())
        {
            blocks.push_back(parseBlock(ts));
        }
        return std::make_unique<NodeMultiBlock>(std::move(blocks));
    }

    std::unique_ptr<NodeIf> parseIf(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        auto condition = parsePrecedence(ts);
        std::string thenStr = ts.get().value;
        assert(thenStr == "then");
        auto thenStatement = parseMultiBlock(ts);
        std::string fiStr = ts.get().value;
        std::optional<std::unique_ptr<NodeMultiBlock>> elseStatement = std::nullopt;
        if (fiStr == "else")
        {
            elseStatement = parseMultiBlock(ts);
            fiStr = ts.get().value;
        }
        assert(fiStr == "fi");
        return std::make_unique<NodeIf>(std::move(condition), std::move(thenStatement), std::move(elseStatement));
    }

    std::unique_ptr<NodeGoto> parseGoto(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        return std::make_unique<NodeGoto>(ts.get().value);
    }

    std::unique_ptr<NodeReturn> parseReturn(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        return std::make_unique<NodeReturn>(parsePrecedence(ts));
    }

    std::unique_ptr<NodeNumber> parseNumber(const Lexer::Token &t)
    {
        int value = std::stoi(t.value);
        return std::make_unique<NodeNumber>(value);
    }

    std::unique_ptr<NodeIdentifier> parseIdentifier(const Lexer::Token &t)
    {
        return std::make_unique<NodeIdentifier>(t.value);
    }

    std::unique_ptr<NodeStatement> parseStatement(Lexer::TokenStream &ts)
    {
        const Lexer::Token t = ts.peek();
        std::unique_ptr<NodeStatement> statement;
        switch (t.type)
        {
        case Lexer::TokenType::KEYWORD_GOTO:
            statement = parseGoto(ts.get(), ts);
            break;
        case Lexer::TokenType::KEYWORD_RETURN:
            statement = parseReturn(ts.get(), ts);
            break;
        case Lexer::TokenType::IDENTIFIER:
            statement = parseVariableAssignment(ts);
            break;
        case Lexer::TokenType::TYPE:
            statement = parseVariableDeclaration(ts);
            break;
        default:
            throw std::runtime_error("Unexpected token: " + t.value);
            break;
        }
        const std::string semicolon = ts.get().value;
        assert(semicolon == ";");
        return statement;
    }

    std::unique_ptr<NodeBlockModifier> parseBlockModifier(Lexer::TokenStream &ts)
    {
        return std::make_unique<NodeBlockModifier>(Lexer::ModifierType::Named, ts.get().value);
    }

    std::unique_ptr<NodeBlock> parseBlock(Lexer::TokenStream &ts)
    {
        Lexer::Token t = ts.peek();
        std::optional<std::unique_ptr<NodeBlockModifier>> modifier = std::nullopt;
        if (t.type == Lexer::TokenType::KEYWORD_HASHTAG)
        {
            ts.get();
            modifier = parseBlockModifier(ts);
        }
        t = ts.peek();
        std::unique_ptr<NodeBlock> block;
        switch (t.type)
        {
        case Lexer::TokenType::KEYWORD_IF:
            block = parseIf(ts.get(), ts);
            break;
        default:
            block = parseStatement(ts);
            break;
        }
        block->modifier = std::move(modifier);
        return block;
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
        if (t.type == Lexer::TokenType::IDENTIFIER)
        {
            term = parseIdentifier(ts.get());
        }
        // while t is a * or / parseMul
        t = ts.peek();
        while (t.type == Lexer::TokenType::OPERATOR_MUL || t.type == Lexer::TokenType::OPERATOR_DIV)
        {
            auto op = ts.get().type;
            std::unique_ptr<NodeExpression> right = parseNumber(ts.get());
            term = std::make_unique<NodeBinOperator>(std::move(term), std::move(right), op);
            t = ts.peek();
        }
        return term;
    }

    std::unique_ptr<NodeExpression> parsePrecedence(Lexer::TokenStream &ts, int precedenceIndex)
    {
        assert(precedenceIndex >= 0);
        if (precedenceIndex == 0)
            return parseMul(ts);

        const int precedence = Lexer::precedenceList[precedenceIndex];
        std::unique_ptr<NodeExpression> left = parsePrecedence(ts, precedenceIndex - 1);
        Lexer::Token t = ts.peek();
        while (t.getPrecedence() == precedence)
        {
            auto op = ts.get().type;
            std::unique_ptr<NodeExpression> right = parsePrecedence(ts, precedence - 1);
            left = std::make_unique<NodeBinOperator>(std::move(left), std::move(right), op);
            t = ts.peek();
        }
        return left;
    }

    std::unique_ptr<NodeExpression> parseExpression(Lexer::TokenStream &ts)
    {
        std::unique_ptr<NodeExpression> left = parseMul(ts);
        while (ts.peek().getPrecedence() == 20)
        {
            auto op = ts.get().type;
            std::unique_ptr<NodeExpression> right = parseMul(ts);
            left = std::make_unique<NodeBinOperator>(std::move(left), std::move(right), op);
        }
        return left;
    }

    std::unique_ptr<NodeVariableDeclaration> parseVariableDeclaration(Lexer::TokenStream &ts)
    {
        const std::string type = ts.get().value;
        const std::string identifier = ts.get().value;
        const std::string equal = ts.get().value;
        std::optional<std::unique_ptr<NodeExpression>> expression = std::nullopt;
        if (equal == "=")
        {
            expression = parseExpression(ts);
        }
        return std::make_unique<NodeVariableDeclaration>(type, identifier, std::move(expression));
    }

    std::unique_ptr<NodeVariableAssignment> parseVariableAssignment(Lexer::TokenStream &ts)
    {
        const std::string identifier = ts.get().value;
        const std::string equal = ts.get().value;
        assert(equal == "=");
        std::unique_ptr<NodeExpression> expression = parseExpression(ts);
        return std::make_unique<NodeVariableAssignment>(identifier, std::move(expression));
    }
}