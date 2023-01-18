#pragma once
#include <string>
#include <istream>
#include <optional>
namespace Lexer
{

    enum class TokenType
    {
        KEYWORD,
        OPERATOR,
        OPERATOR_MUL,
        OPERATOR_DIV,
        OPERATOR_ADD,
        OPERATOR_SUB,
        PARENTHESIS,
        NUMBER,
        IDENTIFIER,
        KEYWORD_IF,
        KEYWORD_GOTO,
    };

    class Token
    {
    public:
        TokenType type;
        std::string value;

        Token(TokenType type, std::string value) : type(type), value(value){};
        Token(){};
    };

    class TokenStream
    {
    private:
        std::istream &input;
        std::optional<Token> buffer = std::nullopt;

    public:
        TokenStream(std::istream &input) : input(input){};
        Token get();
        Token peek();
    };
}