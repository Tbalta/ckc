#pragma once
#include "node.hpp"

namespace Parser
{
    class unexpectedTokenException : public std::exception
    {
    public:
        Lexer::Token token;
        std::optional<Lexer::TokenType> expected;
        unexpectedTokenException(Lexer::Token token, std::optional<Lexer::TokenType> expected = std::nullopt) : token(token), expected(expected){};
        virtual const char *what() const throw()
        {
            std::string expectedString = "";
            if (expected.has_value())
                expectedString = "Expected " + Lexer::tokenTypeToString(expected.value());
            return ("Unexpected token " + Lexer::tokenTypeToString(token.type) + " " + token.value + " at " + std::to_string(token.line) + ":" + std::to_string(token.column) + " " + expectedString).c_str();
        }
    };

    bool hasError();

    NodeIdentifier parseStatement(Lexer::TokenStream &ts);
    NodeIdentifier parseExpression(Lexer::TokenStream &ts);
    NodeIdentifier parseBlock(Lexer::TokenStream &ts);
    NodeIdentifier parseVariableDeclaration(Lexer::TokenStream &ts);
    NodeIdentifier parseVariableAssignment(Lexer::TokenStream &ts);
    NodeIdentifier parsePrecedence(Lexer::TokenStream &ts, int precedenceIndex = Lexer::precedenceList.size() - 1);
    NodeIdentifier parseMultiBlock(Lexer::TokenStream &ts);
    NodeIdentifier parseFunctionCall(Lexer::TokenStream &ts);
    NodeIdentifier addNode(std::shared_ptr<Node> node);
    void replaceNode(NodeIdentifier node, NodeIdentifier newNode);

}