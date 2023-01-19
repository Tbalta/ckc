#pragma once
#include <string>
#include <istream>
#include <optional>
#include <map>
#include <vector>
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
        OPERATOR_LT,
        OPERATOR_LE,
        OPERATOR_GT,
        OPERATOR_GE,
        PARENTHESIS,
        KEYWORD_HASHTAG,
        NUMBER,
        IDENTIFIER,
        KEYWORD_IF,
        KEYWORD_GOTO,
        KEYWORD_ELSE,
        KEYWORD_FI,
        TYPE
    };

    static std::map<TokenType, int> tokenPrecedence{
        {TokenType::OPERATOR_MUL, 10},
        {TokenType::OPERATOR_DIV, 10},
        {TokenType::OPERATOR_ADD, 20},
        {TokenType::OPERATOR_SUB, 20},
        {TokenType::OPERATOR_LT, 30},
        {TokenType::OPERATOR_LE, 30},
        {TokenType::OPERATOR_GT, 30},
        {TokenType::OPERATOR_GE, 30},
    };
    static std::vector<int> precedenceList{
        10,
        20,
        30,
    };

    class Token
    {
    public:
        TokenType type;
        std::string value;

        Token(TokenType type, std::string value) : type(type), value(value){};
        Token(){};
        int getPrecedence();
        bool isEndMultiBlock();
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
        bool isEmpty();
    };
}