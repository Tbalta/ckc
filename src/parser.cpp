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
    std::map<NodeIdentifierIndex, std::shared_ptr<Node>> nodes;

    NodeIdentifier addNode(std::shared_ptr<Node> node)
    {
        static NodeIdentifierIndex id = 0;
        nodes[id] = node;
        auto identifier = NodeIdentifier{id++};
        node->thisNode = identifier;
        return identifier;
    }

    bool hasError()
    {
        return parserError;
    }

#define CHECK_TOKEN_AND_RETURN(t, reference, ts) \
    {                                            \
        if (!checkToken(t, reference, ts))       \
            return NodeIdentifier(-1);           \
    }

    // Check if the token is the expected one.
    // Return true if token is the expected or if a recovery occured.
    bool checkToken(const Lexer::Token &t, Lexer::TokenType reference, Lexer::TokenStream &ts)
    {
        if (t.type == reference)
            return true;
        parserError = true;
        ts.unexpectedToken(t, reference);
        throw std::runtime_error("Unexpected token");
        // while (ts.peek().type != Lexer::TokenType::TOKEN_EOF && ts.get().type != reference)
        //     continue;
        // return ts.peek().type != Lexer::TokenType::TOKEN_EOF;
    }

    NodeIdentifier parseMultiBlock(Lexer::TokenStream &ts)
    {
        std::vector<NodeIdentifier> blocks;
        Lexer::LexerContext::pushContext();
        while (!ts.peek().isEndMultiBlock())
        {
            try
            {
                blocks.push_back(parseBlock(ts));
            }
            catch (const std::exception &e)
            {
                while (!ts.peek().isEndMultiBlock())
                    ts.get();
            }
        }
        Lexer::LexerContext::popContext();
        auto multiBlock = std::make_shared<NodeMultiBlock>(blocks);
        return addNode(multiBlock);
    }

    NodeIdentifier parseIf(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        auto condition = parsePrecedence(ts);
        checkToken(ts.get(), Lexer::TokenType::KEYWORD_THEN, ts);
        auto thenStatement = parseMultiBlock(ts);
        auto fiToken = ts.get();
        std::optional<NodeIdentifier> elseStatement = std::nullopt;
        if (fiToken.value == "else")
        {
            elseStatement = parseMultiBlock(ts);
            fiToken = ts.get();
        }
        checkToken(fiToken, Lexer::TokenType::KEYWORD_FI, ts);
        auto node = std::make_shared<NodeIf>(t, fiToken, condition, thenStatement, elseStatement);
        return addNode(node);
    }

    // Parse a function call.
    // ts shall be at the function name.
    NodeIdentifier parseFunction(Lexer::TokenStream &ts)
    {
        auto tokenFunction = ts.get();
        // Parse the function name.
        CHECK_TOKEN_AND_RETURN(ts.peek(), Lexer::TokenType::IDENTIFIER, ts);
        const std::string name = ts.get().value;
        Lexer::LexerContext::addToken(name, Lexer::TokenType::FUNCTION_NAME);
        Lexer::LexerContext::pushContext();

        // Parse the parameters.
        CHECK_TOKEN_AND_RETURN(ts.get(), Lexer::TokenType::PARENTHESIS_OPEN, ts);
        std::vector<std::pair<std::string, std::string>> parameters; // <type, name>
        while (!ts.isEmpty() && ts.peek().type != Lexer::TokenType::PARENTHESIS_CLOSE)
        {
            checkToken(ts.peek(), Lexer::TokenType::TYPE, ts);
            const std::string type = ts.get().value;
            checkToken(ts.peek(), Lexer::TokenType::IDENTIFIER, ts);
            const std::string identifier = ts.get().value;
            Lexer::LexerContext::addToken(identifier, Lexer::TokenType::VARIABLE_NAME);
            parameters.push_back({type, identifier});
            if (ts.peek().type == Lexer::TokenType::COMMA)
                ts.get();
            else
                checkToken(ts.peek(), Lexer::TokenType::PARENTHESIS_CLOSE, ts);
        }
        CHECK_TOKEN_AND_RETURN(ts.get(), Lexer::TokenType::PARENTHESIS_CLOSE, ts);

        // Optional return type.
        std::optional<std::string> returnType = std::nullopt;
        if (ts.peek().type == Lexer::TokenType::KEYWORD_RETURN)
        {
            ts.get();
            CHECK_TOKEN_AND_RETURN(ts.peek(), Lexer::TokenType::TYPE, ts);
            returnType = ts.get().value;
        }

        // Check for the function body.
        std::optional<NodeIdentifier> body = std::nullopt;
        if (ts.peek().type == Lexer::TokenType::KEYWORD_IS)
        {
            ts.get();
            body = parseMultiBlock(ts);
            Lexer::LexerContext::popContext();
            CHECK_TOKEN_AND_RETURN(ts.get(), Lexer::TokenType::KEYWORD_ENDFUNCTION, ts);
            if (body.value().get() == nullptr)
                return NodeIdentifier(-1);
        }
        else
        {
            CHECK_TOKEN_AND_RETURN(ts.get(), Lexer::TokenType::SEMICOLON, ts);
        }

        // Create the function.
        auto node = std::make_shared<NodeFunction>(tokenFunction, name, parameters, returnType, std::move(body));
        return addNode(node);
    }

    NodeIdentifier parseGoto(const Lexer::Token &t, Lexer::TokenStream &ts)
    {
        auto node = std::make_shared<NodeGoto>(ts.get().value);
        return addNode(node);
    }

    NodeIdentifier parseReturn(Lexer::TokenStream &ts)
    {
        auto returnToken = ts.get();
        if (ts.peek().type == Lexer::TokenType::SEMICOLON)
        {
            ts.get();
            auto node = std::make_shared<NodeReturn>(returnToken, std::nullopt);
            return addNode(node);
        }
        auto node = std::make_shared<NodeReturn>(returnToken, parseExpression(ts));
        return addNode(node);
    }

    NodeIdentifier parseNumber(const Lexer::Token &t)
    {
        int value = std::stoi(t.value);
        auto node = std::make_shared<NodeNumber>(value, t);
        return addNode(node);
    }

    NodeIdentifier parseIdentifier(const Lexer::Token &t)
    {
        auto node = std::make_shared<NodeText>(t.value, t);
        return addNode(node);
    }

    NodeIdentifier parseStatement(Lexer::TokenStream &ts)
    {
        const Lexer::Token t = ts.peek();
        NodeIdentifier statement;
        switch (t.type)
        {
        case Lexer::TokenType::KEYWORD_GOTO:
            statement = parseGoto(ts.get(), ts);
            break;
        case Lexer::TokenType::KEYWORD_RETURN:
            statement = parseReturn(ts);
            break;
        case Lexer::TokenType::VARIABLE_NAME:
            statement = parseVariableAssignment(ts);
            break;
        case Lexer::TokenType::TYPE:
            statement = parseVariableDeclaration(ts);
            break;
        case Lexer::TokenType::TOKEN_EOF:
            return statement;
        default:
            ts.unexpectedToken(ts.get());
            break;
        }
        checkToken(ts.get(), Lexer::TokenType::SEMICOLON, ts);
        return statement;
    }

    NodeIdentifier parseBlockModifier(Lexer::TokenStream &ts)
    {
        auto node = std::make_shared<NodeBlockModifier>(Lexer::ModifierType::Named, ts.get().value);
        return addNode(node);
    }

    NodeIdentifier parsePragma(Lexer::TokenStream &ts)
    {
        auto pragmaToken = ts.get();
        auto targetObject = ts.get();
        if (targetObject.type == Lexer::TokenType::IDENTIFIER)
        {
            ts.unexpectedToken(targetObject);
        }
        const Lexer::Token pragmaType = ts.get();
        CHECK_TOKEN_AND_RETURN(ts.get(), Lexer::TokenType::KEYWORD_IS, ts);
        const auto value = ts.get();
        std::string pragmaValue = value.value;
        if (value.type == Lexer::TokenType::STRING)
        {
            pragmaValue = value.value.substr(1, value.value.size() - 2);
        }
        CHECK_TOKEN_AND_RETURN(ts.get(), Lexer::TokenType::SEMICOLON, ts);
        auto node = std::make_shared<NodePragma>(pragmaToken, pragmaType.type, pragmaValue, targetObject);
        return addNode(node);
    }

    NodeIdentifier parseBlock(Lexer::TokenStream &ts)
    {
        Lexer::Token t = ts.peek();
        std::optional<NodeIdentifier> modifier = std::nullopt;
        if (t.type == Lexer::TokenType::KEYWORD_HASHTAG)
        {
            ts.get();
            modifier = parseBlockModifier(ts);
        }
        t = ts.peek();
        NodeIdentifier block;
        switch (t.type)
        {
        case Lexer::TokenType::KEYWORD_IF:
            block = parseIf(ts.get(), ts);
            break;
        case Lexer::TokenType::KEYWORD_FUNCTION:
            block = parseFunction(ts);
            break;
        case Lexer::TokenType::KEYWORD_PRAGMA:
            block = parsePragma(ts);
            break;
        default:
            block = parseStatement(ts);
            break;
        }
        if (block.get() == nullptr)
            return NodeIdentifier(-1);
        block.get<NodeBlock>()->modifier = modifier;
        return block;
    }

    NodeIdentifier parseFunctionCall(Lexer::TokenStream &ts)
    {
        auto functionToken = ts.get();
        const std::string name = functionToken.value;
        checkToken(ts.get(), Lexer::TokenType::PARENTHESIS_OPEN, ts);
        std::vector<NodeIdentifier> parameters;
        while (ts.peek().type != Lexer::TokenType::PARENTHESIS_CLOSE)
        {
            parameters.push_back(parsePrecedence(ts));
            if (ts.peek().type == Lexer::TokenType::COMMA)
                ts.get();
            else
                checkToken(ts.peek(), Lexer::TokenType::PARENTHESIS_CLOSE, ts);
        }
        checkToken(ts.get(), Lexer::TokenType::PARENTHESIS_CLOSE, ts);
        auto node = std::make_shared<NodeFunctionCall>(functionToken, name, std::move(parameters));
        return addNode(node);
    }

    NodeIdentifier parseTerm(Lexer::TokenStream &ts)
    {
        Lexer::Token t = ts.peek();
        if (t.type == Lexer::TokenType::NUMBER)
        {
            return parseNumber(ts.get());
        }
        if (t.type == Lexer::TokenType::FUNCTION_NAME)
        {
            return parseFunctionCall(ts);
        }
        return parseIdentifier(ts.get());
    }

    // NodeIdentifier parseMul(Lexer::TokenStream &ts)
    // {
    //     Lexer::Token t = ts.peek();
    //     // mul -> term mul'
    //     NodeIdentifier term = parseTerm(ts);
    //     // while t is a * or / parseMul
    //     t = ts.peek();
    //     while (t.getPrecedence() == 10)
    //     {
    //         auto op = ts.get().type;
    //         NodeIdentifier right = parseTerm(ts);
    //         auto node = std::make_shared<NodeBinOperator>(std::move(term), std::move(right), op);
    //         term = addNode(node);
    //         t = ts.peek();
    //     }
    //     return term;
    // }

    NodeIdentifier parseUnary(Lexer::TokenStream &ts)
    {
        auto token = ts.get();
        auto op = token.type;
        NodeIdentifier right = parsePrecedence(ts);
        auto node = std::make_shared<NodeUnaryOperator>(token, right, op);
        return addNode(node);
    }

    NodeIdentifier parseCast(Lexer::TokenStream &ts)
    {
        auto typeToken = ts.get();
        const std::string type = typeToken.value;

        checkToken(ts.get(), Lexer::TokenType::PARENTHESIS_OPEN, ts);
        NodeIdentifier expression = parseExpression(ts);
        checkToken(ts.get(), Lexer::TokenType::PARENTHESIS_CLOSE, ts);

        auto node = std::make_shared<NodeCast>(typeToken, type, expression);
        return addNode(node);
    }

    NodeIdentifier parseExpression(Lexer::TokenStream &ts)
    {
        try
        {
            if (ts.peek().type == Lexer::TokenType::TYPE)
                return parseCast(ts);
            return parsePrecedence(ts);
        }
        catch (const std::exception &e)
        {
            // Try to recover on expression.
            while (!ts.peek().isEndExpression() && !ts.peek().isEndMultiBlock())
                ts.get();
            // If we reach an endMultiBlock token we need to recover earlier.
            if (ts.peek().isEndMultiBlock())
                throw e;
            return NodeIdentifier(-1);
        }
    }

    NodeIdentifier parseParenthesis(Lexer::TokenStream &ts)
    {
        checkToken(ts.get(), Lexer::TokenType::PARENTHESIS_OPEN, ts);
        NodeIdentifier expression = parseExpression(ts);
        checkToken(ts.get(), Lexer::TokenType::PARENTHESIS_CLOSE, ts);
        return expression;
    }

    // Parse expressions.
    NodeIdentifier parsePrecedence(Lexer::TokenStream &ts, int precedenceIndex)
    {
        assert(precedenceIndex >= 0 && std::size_t(precedenceIndex) < Lexer::precedenceList.size());
        const int precedence = Lexer::precedenceList[precedenceIndex];

        if (ts.peek().isUnaryOperator())
            return parseUnary(ts);

        NodeIdentifier left;
        if (ts.peek().type == Lexer::TokenType::PARENTHESIS_OPEN && precedenceIndex == 0)
        {
            left = parseParenthesis(ts);
        }
        else if (precedenceIndex > 0)
        {
            left = parsePrecedence(ts, precedenceIndex - 1);
        }
        else
        {
            left = parseTerm(ts);
        }
        Lexer::Token t = ts.peek();
        while (t.getPrecedence() == precedence)
        {
            auto op = ts.get().type;
            NodeIdentifier right = parsePrecedence(ts, precedenceIndex > 0 ? precedenceIndex - 1 : 0);
            auto node = std::make_shared<NodeBinOperator>(std::move(left), std::move(right), op);
            left = addNode(node);
            t = ts.peek();
        }
        return left;
    }

    // NodeIdentifier parseExpression(Lexer::TokenStream &ts)
    // {
    //     NodeIdentifier left = parseMul(ts);
    //     while (ts.peek().getPrecedence() == 20)
    //     {
    //         auto op = ts.get().type;
    //         NodeIdentifier right = parseMul(ts);
    //         auto node = std::make_shared<NodeBinOperator>(left, right, op);
    //         left = addNode(node);
    //     }
    //     return left;
    // }

    NodeIdentifier parseVariableDeclaration(Lexer::TokenStream &ts)
    {
        auto typeToken = ts.get();
        const std::string type = typeToken.value;
        const std::string identifier = ts.get().value;
        const std::string equal = ts.peek().value;
        std::optional<NodeIdentifier> expression = std::nullopt;
        if (equal == Lexer::tokenTypeToString(Lexer::TokenType::OPERATOR_ASSIGN))
        {
            ts.get();
            expression = parseExpression(ts);
        }
        Lexer::LexerContext::addToken(identifier, Lexer::TokenType::VARIABLE_NAME);
        auto node = std::make_shared<NodeVariableDeclaration>(typeToken, type, identifier, std::move(expression));
        return addNode(node);
    }

    NodeIdentifier parseVariableAssignment(Lexer::TokenStream &ts)
    {
        auto identifierToken = ts.get();
        const std::string identifier = identifierToken.value;
        checkToken(ts.get(), Lexer::TokenType::OPERATOR_ASSIGN, ts);
        NodeIdentifier expression = parseExpression(ts);
        auto node = std::make_shared<NodeVariableAssignment>(identifierToken, identifier, std::move(expression));
        return addNode(node);
    }

}