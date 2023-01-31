#include "parser.hpp"
#include <map>
#include <string>
#include <cassert>
#include <functional>
#include <iostream>
#include <iomanip>
static bool parserError = false;
namespace Parser
{
    bool hasError()
    {
        return parserError;
    }

    void checkToken(const Lexer::Token &t, Lexer::TokenType reference, Lexer::TokenStream &ts)
    {
        if (t.type == reference)
            return;
        parserError = true;
        ts.unexpectedToken(t, reference);
        while (ts.peek().type != Lexer::TokenType::TOKEN_EOF && ts.get().type != reference)
            continue;
    }

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
        checkToken(ts.get(), Lexer::TokenType::KEYWORD_THEN, ts);
        auto thenStatement = parseMultiBlock(ts);
        auto fiToken = ts.get();
        std::optional<std::unique_ptr<NodeMultiBlock>> elseStatement = std::nullopt;
        if (fiToken.value == "else")
        {
            elseStatement = parseMultiBlock(ts);
            fiToken = ts.get();
        }
        checkToken(fiToken, Lexer::TokenType::KEYWORD_FI, ts);
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
        case Lexer::TokenType::TOKEN_EOF:
            return nullptr;
        default:
            ts.unexpectedToken(ts.get());
            break;
        }
        checkToken(ts.get(), Lexer::TokenType::SEMICOLON, ts);
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
        if (block == nullptr)
            return nullptr;
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
        assert(precedenceIndex >= 0 && precedenceIndex < Lexer::precedenceList.size());
        if (precedenceIndex == 0)
            return parseMul(ts);

        const int precedence = Lexer::precedenceList[precedenceIndex];
        std::unique_ptr<NodeExpression> left = parsePrecedence(ts, precedenceIndex - 1);
        Lexer::Token t = ts.peek();
        while (t.getPrecedence() == precedence)
        {
            auto op = ts.get().type;
            std::unique_ptr<NodeExpression> right = parsePrecedence(ts, precedenceIndex - 1);
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
        const std::string equal = ts.peek().value;
        std::optional<std::unique_ptr<NodeExpression>> expression = std::nullopt;
        if (equal == "=")
        {
            ts.get();
            expression = parseExpression(ts);
        }
        return std::make_unique<NodeVariableDeclaration>(type, identifier, std::move(expression));
    }

    std::unique_ptr<NodeVariableAssignment> parseVariableAssignment(Lexer::TokenStream &ts)
    {
        const std::string identifier = ts.get().value;
        checkToken(ts.get(), Lexer::TokenType::OPERATOR_ASSIGN, ts);
        std::unique_ptr<NodeExpression> expression = parseExpression(ts);
        return std::make_unique<NodeVariableAssignment>(identifier, std::move(expression));
    }
}