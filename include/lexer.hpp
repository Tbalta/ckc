#pragma once
#include <string>
#include <istream>
#include <optional>
#include <map>
#include <vector>
#include <optional>
#include <fstream>
#include "util.hpp"
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
        OPERATOR_EQ,
        OPERATOR_NE,
        OPERATOR_ASSIGN,
        PARENTHESIS,
        KEYWORD_HASHTAG,
        NUMBER,
        IDENTIFIER,
        KEYWORD_IF,
        KEYWORD_GOTO,
        KEYWORD_RETURN,
        KEYWORD_FI,
        KEYWORD_ELSE,
        KEYWORD_THEN,
        TYPE,
        SEMICOLON,
        TOKEN_EOF,
        OPERATOR_PIPE
    };

    enum class ModifierType
    {
        Named
    };

    const static std::map<std::string, ModifierType> modifierTypeMap{
        {"named", ModifierType::Named},
    };

    std::string tokenTypeToString(TokenType type);
    std::string getCurrentLine(std::istream &input);
    std::ostream &operator<<(std::ostream &os, TokenType const &tok);

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
        int line;
        int column;

        Token(TokenType type, std::string value, int line, int column) : type(type), value(value), line(line), column(column){};
        Token(){};
        bool isEndMultiBlock();
        int getPrecedence();
        bool operator==(const Token &other) const;
    };

    class TokenStream
    {
    private:
        std::istream &input;
        std::optional<Token> buffer = std::nullopt;
        std::vector<std::string> lines;
        std::string getNextToken();
        std::string filename = "";

    public:
        int line = 1;
        int column = 1;
        void moveHead();
        TokenStream(std::istream &input, std::string filename) : input(input), filename(filename)
        {
            lines.push_back(getCurrentLine(input));
            moveHead();
        };
        TokenStream(std::istream &input) : input(input)
        {
            lines.push_back(getCurrentLine(input));
            moveHead();
        };

        Token get();
        Token peek();
        bool isEmpty();
        std::string getLine(int line);
        VIRTUAL void unexpectedToken(Token token, std::optional<TokenType> expected = std::nullopt);
        std::vector<Token> toList();
    };
}