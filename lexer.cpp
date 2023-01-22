#include <string>
#include <map>
#include "lexer.hpp"
#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <llvm/Support/raw_ostream.h>
namespace Lexer
{
    std::string tokenTypeToString(TokenType type)
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
        case TokenType::OPERATOR_ADD:
            return "+";
        case TokenType::OPERATOR_SUB:
            return "-";
        case TokenType::OPERATOR_MUL:
            return "*";
        case TokenType::OPERATOR_DIV:
            return "/";
        case TokenType::OPERATOR_LT:
            return "<";
        case TokenType::OPERATOR_LE:
            return "<=";
        case TokenType::OPERATOR_GT:
            return ">";
        case TokenType::OPERATOR_GE:
            return ">=";
        case TokenType::KEYWORD_HASHTAG:
            return "#";
        case TokenType::KEYWORD_IF:
            return "if";
        case TokenType::KEYWORD_GOTO:
            return "goto";
        case TokenType::KEYWORD_RETURN:
            return "return";
        case TokenType::KEYWORD_ELSE:
            return "else";
        case TokenType::KEYWORD_THEN:
            return "then";
        case TokenType::KEYWORD_FI:
            return "fi";
        case TokenType::TYPE:
            return "TYPE";
        case TokenType::SEMICOLON:
            return ";";
        default:
            return "UNKNOWN";
        }
        return "UNKNOWN";
    }

    // List of tokens should not be modified after initialization
    const static std::map<std::string, TokenType> builtinToken{
        {"if", TokenType::KEYWORD_IF},
        {"then", TokenType::KEYWORD_THEN},
        {"else", TokenType::KEYWORD_ELSE},
        {"fi", TokenType::KEYWORD_FI},
        {"goto", TokenType::KEYWORD_GOTO},
        {"return", TokenType::KEYWORD_RETURN},
        {";", TokenType::SEMICOLON},
        {"+", TokenType::OPERATOR_ADD},
        {"-", TokenType::OPERATOR_SUB},
        {"*", TokenType::OPERATOR_MUL},
        {"/", TokenType::OPERATOR_DIV},
        {"<", TokenType::OPERATOR_LT},
        {"<=", TokenType::OPERATOR_LE},
        {">", TokenType::OPERATOR_GT},
        {">=", TokenType::OPERATOR_GE},
        {"==", TokenType::OPERATOR_EQ},
        {"!=", TokenType::OPERATOR_NE},
        {"=", TokenType::OPERATOR_ASSIGN},
        {"(", TokenType::PARENTHESIS},
        {"#", TokenType::KEYWORD_HASHTAG},
        {")", TokenType::PARENTHESIS},
        {"", TokenType::TOKEN_EOF}};

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
        char c;
        while (input.get(c))
        {
            column++;
            if (c == '\n')
            {
                line++;
                column = 1;
                lines.push_back(getCurrentLine(input));
            }
            if (!(c == ' ' || c == '\t' || c == '\n' || c == '\r'))
                token += c;
            else if (token.size() > 0)
            {
                // lines[lines.size() - 1] += c + token;
                moveHead();
                return token;
            }
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
        if (builtinToken.find(token) != builtinToken.end())
        {
            type = builtinToken.at(token);
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

}