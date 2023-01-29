#include "lexer_test.hpp"
#include "parser.hpp"
#include <gtest/gtest.h>

using namespace testing;
TEST(ParserTest, missingSemicolon) {
    auto stream = std::stringstream("int64 i = 0");
    MockTokenStream ts(stream);
    EXPECT_CALL(ts, unexpectedToken(Token(TokenType::TOKEN_EOF, std::string(""), 1, sizeof("int64 i = 0")), std::optional<TokenType>(TokenType::SEMICOLON)))
        .Times(1);
    Parser::parseBlock(ts);
}