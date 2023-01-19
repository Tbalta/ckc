#include <string>
#include <map>
#include "lexer.hpp"
#include <algorithm>
#include <unordered_set>
namespace Lexer
{
    static std::string tokenTypeToString(TokenType type)
    {
        switch (type)
        {
        case TokenType::KEYWORD:
            return "KEYWORD";
        case TokenType::OPERATOR:
            return "OPERATOR";
        case TokenType::PARENTHESIS:
            return "PARENTHESIS";
        case TokenType::NUMBER:
            return "NUMBER";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        }
        return "UNKNOWN";
    }

    // List of tokens should not be modified after initialization
    const static std::map<std::string, Token> builtinToken{
        {"if", Token(TokenType::KEYWORD_IF, "if")},
        {"then", Token(TokenType::KEYWORD, "then")},
        {"else", Token(TokenType::KEYWORD_ELSE, "else")},
        {"fi", Token(TokenType::KEYWORD_FI, "fi")},
        {"goto", Token(TokenType::KEYWORD_GOTO, "goto")},
        {"+", Token(TokenType::OPERATOR_ADD, "+")},
        {"-", Token(TokenType::OPERATOR_SUB, "-")},
        {"*", Token(TokenType::OPERATOR_MUL, "*")},
        {"/", Token(TokenType::OPERATOR_DIV, "/")},
        {"<", Token(TokenType::OPERATOR_LT, "<")},
        {"<=", Token(TokenType::OPERATOR_LE, "<=")},
        {">", Token(TokenType::OPERATOR_GT, ">")},
        {">=", Token(TokenType::OPERATOR_GE, ">=")},
        {"(", Token(TokenType::PARENTHESIS, "(")},
        {"#", Token(TokenType::KEYWORD_HASHTAG, "#")},
        {")", Token(TokenType::PARENTHESIS, ")")}};

    static std::map<std::string, Token> typeToken{
        {"uint8", Token(TokenType::TYPE, "uint8")},
        {"uint16", Token(TokenType::TYPE, "uint16")},
        {"uint32", Token(TokenType::TYPE, "uint32")},
        {"uint64", Token(TokenType::TYPE, "uint64")},
        {"int8", Token(TokenType::TYPE, "int8")},
        {"int16", Token(TokenType::TYPE, "int16")},
        {"int32", Token(TokenType::TYPE, "int32")},
        {"int64", Token(TokenType::TYPE, "int64")},
    };

    bool isNumber(const std::string &str)
    {
        std::string::const_iterator start = str.begin();
        if (*start == '-')
            start++;
        return std::all_of(start, str.end(), ::isdigit);
    }

    int Token::getPrecedence()
    {
        if (tokenPrecedence.find(type) != tokenPrecedence.end())
        {
            return tokenPrecedence.at(type);
        }
        return -1;
    }

    std::string getNextToken(std::istream &input)
    {
        std::string token;
        char c;
        while (input.get(c))
        {
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
            {
                if (token.size() > 0)
                {
                    return token;
                }
                continue;
            }
            token += c;
        }
    }

    Token TokenStream::get()
    {
        if (buffer.has_value())
        {
            Token t = buffer.value();
            buffer.reset();
            return t;
        }
        std::string token = getNextToken(input);

        if (builtinToken.find(token) != builtinToken.end())
        {
            return builtinToken.at(token);
        }
        if (typeToken.find(token) != typeToken.end())
        {
            return typeToken.at(token);
        }
        if (isNumber(token))
        {
            return Token(TokenType::NUMBER, token);
        }
        return Token(TokenType::IDENTIFIER, token);
    }

    Token TokenStream::peek()
    {
        if (buffer.has_value())
        {
            return buffer.value();
        }
        buffer = get();
        return buffer.value();
    }

    bool TokenStream::isEmpty()
    {
        this->peek();
        return input.eof();
    }

    bool Token::isEndMultiBlock()
    {
        std::unordered_set<TokenType> endMultiBlock{
            TokenType::KEYWORD_ELSE,
            TokenType::KEYWORD_FI};
        return endMultiBlock.find(type) != endMultiBlock.end();
    }

}