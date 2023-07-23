#pragma once
#include <string>
#include <istream>
#include <optional>
#include <map>
#include <vector>
#include <optional>
#include <fstream>
#include "util.hpp"
#include "contextProvider.hpp"
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
        OPERATOR_MOD,
        OPERATOR_LT,
        OPERATOR_LE,
        OPERATOR_GT,
        OPERATOR_GE,
        OPERATOR_EQ,
        OPERATOR_NE,
        BINARY_AND,
        BINARY_OR,
        OPERATOR_LSHIFT,
        BINARY_RSHIFT,
        OPERATOR_ASSIGN,
        PARENTHESIS_OPEN,
        PARENTHESIS_CLOSE,
        KEYWORD_HASHTAG,
        NUMBER,
        IDENTIFIER,
        KEYWORD_IF,
        KEYWORD_GOTO,
        KEYWORD_RETURN,
        KEYWORD_FI,
        KEYWORD_ELSE,
        KEYWORD_THEN,
        KEYWORD_IS,
        KEYWORD_FUNCTION,
        KEYWORD_PRAGMA,
        SYMBOL_NAME,
        KEYWORD_ENDFUNCTION,
        TYPE,
        SEMICOLON,
        TOKEN_EOF,
        OPERATOR_NOT,
        LOGICAL_NOT,
        LOGICAL_AND,
        LOGICAL_OR,
        LOGICAL_XOR,
        COMMA,
        // User defined types
        FUNCTION_NAME,
        VARIABLE_NAME
    };

    template <typename K, typename V>
#ifdef PROD
    constexpr
#endif
        std::map<V, K>
        reverseMap(std::map<K, V> const &map)
    {
        std::map<V, K> rMap;
        for (auto const &[k, v] : map)
        {
            rMap[v] = k;
        }
        return rMap;
    }

    // List of tokens should not be modified after initialization
    const static std::map<TokenType, std::string> tokenToStringMap{
        // Control flow
        {TokenType::KEYWORD_IF, "if"},
        {TokenType::KEYWORD_THEN, "then"},
        {TokenType::KEYWORD_ELSE, "else"},
        {TokenType::KEYWORD_FI, "fi"},
        {TokenType::KEYWORD_GOTO, "goto"},
        {TokenType::KEYWORD_RETURN, "return"},
        {TokenType::KEYWORD_FUNCTION, "function"},
        {TokenType::KEYWORD_ENDFUNCTION, "endfunction"},

        // File structure
        {TokenType::TOKEN_EOF, ""},
        {TokenType::SEMICOLON, ";"},
        {TokenType::COMMA, ","},
        {TokenType::KEYWORD_IS, "is"},

        // Pragma
        {TokenType::KEYWORD_PRAGMA, "pragma"},
        {TokenType::SYMBOL_NAME, "symbol_name"},
        
        // Comparison operators
        {TokenType::OPERATOR_LT, "<"},
        {TokenType::OPERATOR_GT, ">"},
        {TokenType::OPERATOR_EQ, "="},
        {TokenType::OPERATOR_LE, "<="},
        {TokenType::OPERATOR_GE, ">="},
        {TokenType::OPERATOR_NE, "!="},

        // Arithmetic operators
        {TokenType::OPERATOR_MOD, "%"},
        {TokenType::OPERATOR_ADD, "+"},
        {TokenType::OPERATOR_SUB, "-"},
        {TokenType::OPERATOR_MUL, "*"},

        // Binary operators
        {TokenType::BINARY_OR, "|"},
        {TokenType::OPERATOR_NOT, "~"},
        {TokenType::OPERATOR_LSHIFT, "<<"},
        {TokenType::BINARY_RSHIFT, ">>"},
        {TokenType::BINARY_AND, "&"},

        // Logical operators
        {TokenType::LOGICAL_OR, "or"},
        {TokenType::LOGICAL_AND, "and"},
        {TokenType::LOGICAL_XOR, "xor"},
        {TokenType::LOGICAL_NOT, "not"},

        // Misc
        {TokenType::OPERATOR_DIV, "/"},
        {TokenType::OPERATOR_ASSIGN, ":="},
        {TokenType::PARENTHESIS_OPEN, "("},
        {TokenType::KEYWORD_HASHTAG, "#"},
        {TokenType::PARENTHESIS_CLOSE, ")"},
    };
    const static std::map<std::string, TokenType> stringToTokenMap = reverseMap(tokenToStringMap);
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
        {TokenType::OPERATOR_MOD, 10},
        {TokenType::OPERATOR_ADD, 20},
        {TokenType::OPERATOR_SUB, 20},
        {TokenType::OPERATOR_LSHIFT, 30},
        {TokenType::BINARY_RSHIFT, 30},
        {TokenType::OPERATOR_LT, 40},
        {TokenType::OPERATOR_LE, 40},
        {TokenType::OPERATOR_GT, 40},
        {TokenType::OPERATOR_GE, 40},
        {TokenType::OPERATOR_EQ, 40},
        {TokenType::OPERATOR_NE, 40},
        {TokenType::BINARY_AND, 50},
        {TokenType::LOGICAL_AND, 50},
        {TokenType::BINARY_OR, 60},
        {TokenType::LOGICAL_OR, 60},
    };
    static std::vector<int> precedenceList{
        10,
        20,
        30,
        40,
        50,
        60,
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
        bool isUnaryOperator();
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
        Context::ContextProvider &contextProvider = Context::ContextProvider::getInstance();

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

    namespace LexerContext
    {

        // LexerContext();
        void pushContext();
        void popContext();
        void init();
        std::optional<TokenType> getTokenType(std::string token);
        void addToken(std::string token, TokenType type);
        extern std::vector<std::map<std::string, TokenType>> contextStack;
    };

}