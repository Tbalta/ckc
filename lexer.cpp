#include <string>
#include <map>
#include "lexer.hpp"
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <llvm/Support/raw_ostream.h>
#include <cctype>
namespace Lexer
{
    std::string tokenTypeToString(TokenType type)
    {
        auto it = tokenToStringMap.find(type);
        if (it != tokenToStringMap.end())
        {
            return it->second;
        }
        return "UNKNOWN";
    }



    std::ostream &operator<<(std::ostream &os, TokenType const &tok)
    {
        return os << tokenTypeToString(tok);
    }

    static std::map<std::string, TokenType> typeToken{
        {"uint8", TokenType::TYPE},
        {"uint16", TokenType::TYPE},
        {"uint32", TokenType::TYPE},
        {"uint64", TokenType::TYPE},
        {"int8", TokenType::TYPE},
        {"int16", TokenType::TYPE},
        {"int32", TokenType::TYPE},
        {"int64", TokenType::TYPE},
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

    bool Token::operator==(const Token &other) const
    {
        return type == other.type && value == other.value && line == other.line && column == other.column;
    }

    std::string getCurrentLine(std::istream &input)
    {
        int current = input.tellg();
        std::string line;
        std::getline(input, line);
        input.seekg(current);
        return line;
    }
    void TokenStream::moveHead()
    {
        int c = input.peek();
        while (c != EOF && (c == ' ' || c == '\t' || c == '\n' || c == '\r'))
        {
            input.get();
            if (c == '\n')
            {
                lines.push_back(getCurrentLine(input));
                line++;
                column = 0;
            }
            column++;
            c = input.peek();
        }
    }

    // Get the next token and move the head at the beginning of the next token
    std::string TokenStream::getNextToken()
    {
        std::string token;
        char c = input.peek();
        if (std::isalnum(c) || c == '_')
        {
            while ((std::isalnum(c) || c == '_') && c != EOF)
            {
                token += input.get();
                column++;
                c = input.peek();
            }
            moveHead();
            return token;
        }
        while (!std::isalnum(c) && c != ' ' && c != '\n' && c != EOF)
        {
            token += input.get();
            column++;
            c = input.peek();
        }
        moveHead();
        return token;
    }

    Token TokenStream::get()
    {
        if (buffer.has_value())
        {
            Token t = buffer.value();
            buffer.reset();
            return t;
        }
        int currentLine = line;
        int currentColumn = column;
        std::string token = getNextToken();
        Lexer::TokenType type = TokenType::IDENTIFIER;
        if (stringToTokenMap.find(token) != stringToTokenMap.end())
        {
            type = stringToTokenMap.at(token);
        }
        else if (typeToken.find(token) != typeToken.end())
        {
            type = typeToken.at(token);
        }
        else if (isNumber(token))
        {
            type = TokenType::NUMBER;
        }
        return Token(type, token, currentLine, currentColumn);
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
        return input.eof();
    }

    bool Token::isEndMultiBlock()
    {
        std::unordered_set<TokenType> endMultiBlock{
            TokenType::KEYWORD_ELSE,
            TokenType::KEYWORD_FI,
            TokenType::TOKEN_EOF};
        return endMultiBlock.find(type) != endMultiBlock.end();
    }

    std::string TokenStream::getLine(int line)
    {
        if (line - 1 < lines.size())
        {
            return lines[line - 1];
        }
        return "";
    }
#define RESET_COL "\033[0m"
#define RED_COL "\033[31m"
    void TokenStream::unexpectedToken(Token t, std::optional<TokenType> expected)
    {
        std::cerr << RED_COL << "[ERROR] " << RESET_COL << "Unexpected token type: " << t.value;
        if (expected.has_value())
        {
            std::cerr << ", expected: " << Lexer::tokenTypeToString(expected.value());
        }
        std::cerr << std::endl;
        std::cerr << filename << ":" << t.line << ":" << t.column << std::endl;
        std::cerr << std::setfill(' ') << std::setw(4) << t.line << std::left << std::setw(5) << " |" << getLine(t.line) << std::endl;
        std::cerr << std::setw(t.column - 1 + 9) << "" << RED_COL << std::setw(t.value.size()) << std::setfill('^') << "" << RESET_COL << std::setfill(' ') << std::endl;
        std::cerr << std::setfill(' ') << std::right;
    }
    std::vector<Token> TokenStream::toList()
    {
        std::vector<Token> tokens;
        while (!isEmpty())
        {
            tokens.push_back(get());
        }
        return tokens;
    }

    bool Token::isUnaryOperator()
    {
        std::unordered_set<TokenType> unaryOperators{
            TokenType::OPERATOR_NOT,
            TokenType::LOGICAL_NOT,
            TokenType::OPERATOR_SUB};
        return unaryOperators.find(type) != unaryOperators.end();
    }

}