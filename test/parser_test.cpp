#include "lexer_test.hpp"
#include "parser.hpp"
#include <gtest/gtest.h>

using namespace testing;

class ParserTest : public ::testing::Test {
 protected:
  void SetUp() override {
    Lexer::LexerContext::init();
  }
};

TEST_F(ParserTest, missingSemicolon) {
    auto stream = std::stringstream("int64 i := 0");
    MockTokenStream ts(stream);
    EXPECT_CALL(ts, unexpectedToken(Token(TokenType::TOKEN_EOF, std::string(""), 1, sizeof("int64 i := 0")), std::optional<TokenType>(TokenType::SEMICOLON)))
        .Times(1);
    Parser::parseBlock(ts);
}

TEST_F (ParserTest, parseFunction)
{
    auto stream = std::stringstream("function main() return int32 is return 0; endfunction");
    MockTokenStream ts(stream);
    auto function = Parser::parseBlock(ts);
    ASSERT_THAT(function.get(), NotNull());
    // check if function is of type NodeFunction
    ASSERT_NO_THROW(function.get<Parser::NodeFunction>());
}

TEST_F (ParserTest, parseFunctionWithParameters)
{
    auto stream = std::stringstream("function main(int32 a, int32 b) return int32 is return 0; endfunction");
    MockTokenStream ts(stream);
    auto function = Parser::parseBlock(ts);
    ASSERT_THAT(function.get(), NotNull());
    ASSERT_TRUE(LexerContext::getTokenType("main").has_value());
    // check if function is of type NodeFunction
    ASSERT_NO_THROW(function.get<Parser::NodeFunction>());
}

TEST_F (ParserTest, assertThrowsInGet)
{
    LexerContext::addToken("main", TokenType::FUNCTION_NAME);

    auto stream = std::stringstream("main()");
    MockTokenStream ts(stream);
    auto functionCall = Parser::parseExpression(ts);
    ASSERT_THAT(functionCall.get(), NotNull());
    // check if function is of type NodeFunction
    ASSERT_THAT(functionCall.get<Parser::NodeText>().get(), IsNull());
}

TEST_F (ParserTest, parseFunctionCall)
{
    // Add the symbol "main" to the context
    LexerContext::addToken("main", TokenType::FUNCTION_NAME);

    auto stream = std::stringstream("main()");
    MockTokenStream ts(stream);
    auto functionCall = Parser::parseExpression(ts);
    ASSERT_THAT(functionCall.get(), NotNull());
    // check if function is of type NodeFunction
    ASSERT_THAT(functionCall.get<Parser::NodeFunctionCall>().get(), NotNull());
}