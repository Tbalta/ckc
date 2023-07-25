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
        switch (type)
        {
        case TokenType::TOKEN_EOF:
            return "EOF";
        case TokenType::TYPE:
            return "TYPE";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        default:
            return "UNKNOWN";
        }
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
        {"bool", TokenType::TYPE}
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
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
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
        // Parse a string
        if (c == '"')
        {
            bool escape = true; // Start with true to escape the first "
            while (c != EOF && (escape || c != '"'))
            {
                token += input.get();
                column++;
                c = input.peek();
                escape = c == '\\';
            }
            if (c == '"')
            {
                token += input.get();
                column++;
            }
            moveHead();
            return token;
        }
        // Parse an identifier
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
        // Special case for parenthesis
        if (c == '(' || c == ')')
        {
            token += input.get();
            column++;
            moveHead();
            return token;
        }
        while (!std::isalnum(c) && c != ' ' && c != '\r' && c != '\n' && c != EOF)
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
        if (token.starts_with('"') && token.ends_with('"'))
        {
            type = TokenType::STRING;
        }
        else if (stringToTokenMap.find(token) != stringToTokenMap.end())
        {
            type = stringToTokenMap.at(token);
        }
        else if (LexerContext::getTokenType(token).has_value())
        {
            type = LexerContext::getTokenType(token).value();
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
        return input.eof() || input.peek() == EOF;
    }

    bool Token::isEndMultiBlock()
    {
        std::unordered_set<TokenType> endMultiBlock{
            TokenType::KEYWORD_ELSE,
            TokenType::KEYWORD_FI,
            TokenType::KEYWORD_ENDFUNCTION,
            TokenType::TOKEN_EOF};
        return endMultiBlock.find(type) != endMultiBlock.end();
    }

    bool Token::isEndExpression()
    {
        std::unordered_set<TokenType> endExpression{
            TokenType::TOKEN_EOF,
            TokenType::SEMICOLON,
            TokenType::KEYWORD_THEN,
            TokenType::PARENTHESIS_CLOSE};
        return endExpression.find(type) != endExpression.end();
    }

    std::string TokenStream::getLine(int line)
    {
        if (line - 1 < int(lines.size()))
        {
            return lines[line - 1];
        }
        return "";
    }
#define RESET_COL "\033[0m"
#define RED_COL "\033[31m"
    void TokenStream::unexpectedToken(Token t, std::optional<TokenType> expected)
    {
        std::cerr << RED_COL << "[ERROR] " << RESET_COL << "Unexpected token: " << t.value;
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

    std::string Token::underline(std::string color)
    {
        return std::string(column - 1, ' ') + color + std::string(value.size(), '^') + RESET_COL;
    }

    // if the token range is on multiple lines, split the token in multiple tokens
    // std::vector<std::pair<Token, Token>> TokenStream::splitTokenOnSameLine(std::pair<Token, Token> token)
    // {
    //     std::vector<std::pair<Token, Token>> tokens;
    //     if (token.first.line == token.second.line)
    //     {
    //         tokens.push_back(token);
    //         return tokens;
    //     }
    //     for (int i = token.first.line; i <= token.second.line; i++)
    //     {
    //         if (i == token.first.line)
    //         {
    //             tokens.push_back({token.first, Token(TokenType::TOKEN_EOF, "", i, token.first.column + token.first.value.size())});
    //         }
    //         else if (i == token.second.line)
    //         {
    //             tokens.push_back({Token(TokenType::TOKEN_EOF, "", token.second.line, token.second.column), token.second});
    //         }
    //         else
    //         {
    //             tokens.push_back({Token(TokenType::TOKEN_EOF, "", i, 1), Token(TokenType::TOKEN_EOF, "", i, getLine(i).size() + 1)});
    //         }
    //     }
    // }

    std::string getWave(std::pair<Token, Token> token)
    {
        if (token.first == token.second)
            return std::string(token.first.value.size(), '^');
        return std::string(token.first.value.size() + (token.second.column - token.first.column) + token.second.value.size() - 2, '^');
    }

    void TokenStream::highlightMultiplesTokens(std::vector<std::pair<Token, Token>> tokens)
    {
        // Sort the tokens by line then column
        std::sort(tokens.begin(), tokens.end(), [](std::pair<Token, Token> a, std::pair<Token, Token> b) {
            if (a.first.line == b.first.line)
            {
                return a.first.column < b.first.column;
            }
            return a.first.line < b.first.line;
        });
        // Find the first and last line
        int firstLine = tokens[0].first.line;
        int lastLine = tokens.back().second.line;
        std::cerr << filename << ":" << firstLine << ":" << tokens[0].first.column << std::endl;
        // Print the lines
        for (int i = firstLine; i <= lastLine; i++)
        {
            std::cerr << std::setfill(' ') << std::setw(4) << i << std::left << std::setw(5) << " |" << std::right;
            std::cerr << getLine(i) << std::endl;
            int offset = -(int(4 + 5 + sizeof(" |") - 3));
            while (tokens.size() > 0 && tokens.front().first.line == i)
            {
                std::string wave = getWave(tokens.front());
                std::cerr << std::string(tokens.front().first.column - 1 - offset, ' ') << RED_COL << wave << RESET_COL;
                offset += wave.size() + tokens.front().first.column - 1 - offset;
                tokens.erase(tokens.begin());
            }
            std::cerr << std::endl;

        }
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

    bool Token::isBooleanOperator()
    {
        std::unordered_set<TokenType> booleanOperators{
            TokenType::LOGICAL_AND,
            TokenType::LOGICAL_OR,
            TokenType::LOGICAL_XOR,
            TokenType::LOGICAL_NOT};
        return booleanOperators.find(type) != booleanOperators.end();
    }

    bool Token::isComparisonOperator()
    {
        std::unordered_set<TokenType> comparisonOperators{
            TokenType::OPERATOR_LT,
            TokenType::OPERATOR_GT,
            TokenType::OPERATOR_EQ,
            TokenType::OPERATOR_LE,
            TokenType::OPERATOR_GE,
            TokenType::OPERATOR_NE};
        return comparisonOperators.find(type) != comparisonOperators.end();
    }

    // std::vector<std::map<std::string, TokenType>> LexerContext::contextStack = std::vector<std::map<std::string, TokenType>>
    // {
    //     std::m
    // };
    // Initialize the context stack with the global context with at least one map
    std::vector<std::map<std::string, TokenType>> LexerContext::contextStack = std::vector<std::map<std::string, TokenType>>{
        std::map<std::string, TokenType>{
            {"uint8", TokenType::TYPE},
            {"uint16", TokenType::TYPE},
            {"uint32", TokenType::TYPE},
            {"uint64", TokenType::TYPE},
            {"int8", TokenType::TYPE},
            {"int16", TokenType::TYPE},
            {"int32", TokenType::TYPE},
            {"int64", TokenType::TYPE},
            {"bool", TokenType::TYPE}}};

    void LexerContext::init()
    {
        LexerContext::contextStack = std::vector<std::map<std::string, TokenType>>{
            std::map<std::string, TokenType>{
                {"uint8", TokenType::TYPE},
                {"uint16", TokenType::TYPE},
                {"uint32", TokenType::TYPE},
                {"uint64", TokenType::TYPE},
                {"int8", TokenType::TYPE},
                {"int16", TokenType::TYPE},
                {"int32", TokenType::TYPE},
                {"int64", TokenType::TYPE},
                {"bool", TokenType::TYPE}}};
    }

    void LexerContext::pushContext()
    {
        LexerContext::contextStack.push_back(std::map<std::string, TokenType>());
    }
    void LexerContext::popContext()
    {
        LexerContext::contextStack.pop_back();
    }
    std::optional<TokenType> LexerContext::getTokenType(std::string token)
    {
        for (auto it = LexerContext::contextStack.rbegin(); it != LexerContext::contextStack.rend(); it++)
        {
            if (it->find(token) != it->end())
            {
                return it->at(token);
            }
        }
        return std::nullopt;
    }

    void LexerContext::addToken(std::string token, TokenType type)
    {
        LexerContext::contextStack.back()[token] = type;
    }

}