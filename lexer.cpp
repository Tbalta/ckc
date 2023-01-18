#include <string>
#include <map>
#include "lexer.hpp"
#include <algorithm>

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
        {"if", Token(TokenType::KEYWORD, "if")},
        {"then", Token(TokenType::KEYWORD, "then")},
        {"else", Token(TokenType::KEYWORD, "else")},
        {"fi", Token(TokenType::KEYWORD, "fi")},
        {"goto", Token(TokenType::KEYWORD, "goto")},
        {"+", Token(TokenType::OPERATOR, "+")},
        {"-", Token(TokenType::OPERATOR, "-")},
        {"*", Token(TokenType::OPERATOR, "*")},
        {"/", Token(TokenType::OPERATOR, "/")},
        {"(", Token(TokenType::PARENTHESIS, "(")},
        {")", Token(TokenType::PARENTHESIS, ")")}};

    static std::map<std::string, Token> typeToken{
        {"uint8", Token(TokenType::KEYWORD, "uint8")},
        {"uint16", Token(TokenType::KEYWORD, "uint16")},
        {"uint32", Token(TokenType::KEYWORD, "uint32")},
        {"uint64", Token(TokenType::KEYWORD, "uint64")},
        {"int8", Token(TokenType::KEYWORD, "int8")},
        {"int16", Token(TokenType::KEYWORD, "int16")},
        {"int32", Token(TokenType::KEYWORD, "int32")},
        {"int64", Token(TokenType::KEYWORD, "int64")},
    };

    bool isNumber(const std::string &str)
    {
        std::string::const_iterator start = str.begin();
        if (*start == '-')
            start++;
        return std::all_of(start, str.end(), ::isdigit);
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

}