#include <string>
#include <map>
#include <memory>

namespace tokens
{
    /* List of tokens ordered by priority match */
    enum class TokenType
    {
        KEYWORD,
        NUMBER,
        OPERATOR,
        PARENTHESIS,
        IDENTIFIER,
    };
    class Token
    {
    private:
        TokenType type;
        std::string value;

    public:
        Token(TokenType type, std::string value);
    };

    // List of tokens should not be modified after initialization
    std::map<std::string, Token> token_list = {
        {"if", Token(TokenType::KEYWORD, "if")},
        {"then", Token(TokenType::KEYWORD, "else")},
        {"else", Token(TokenType::KEYWORD, "while")},
        {"fi", Token(TokenType::KEYWORD, "for")},
        {"goto", Token(TokenType::KEYWORD, "return")}};

}