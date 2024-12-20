#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "lexer.hpp"
#include <ostream>
#include <string>
using namespace Lexer;
using namespace testing;

template<class T, class Func>
auto Map(const std::vector<T>& input_array, Func op)
{
    std::vector<decltype(op(input_array.front()))> result_array;
    std::transform(input_array.begin(), input_array.end(), std::back_inserter(result_array), op);
    return result_array;
}

class LexerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    Lexer::LexerContext::init();
  }
};


#define RESET_COL "\033[0m"
#define RED_COL "\033[31m"
TEST_F(LexerTest, checkUnexpectedTokenMessage_NoExpected)
{
  auto stream = std::stringstream();
  Lexer::TokenStream ts(stream, "myfile.kc");

  std::ostringstream cerr = std::ostringstream();
  ts.unexpectedToken(Token(TokenType::IDENTIFIER, "a", 1, 1), std::nullopt, cerr);

  auto result = cerr.str();
  ASSERT_TRUE(result.find(RED_COL "[ERROR] " RESET_COL "Unexpected token: a") != std::string::npos);
}

TEST_F(LexerTest, checkUnexpectedTokenMessage_WithExpected)
{
  auto stream = std::stringstream();
  Lexer::TokenStream ts(stream, "myfile.kc");

  std::ostringstream cerr = std::ostringstream();
  ts.unexpectedToken(Token(TokenType::IDENTIFIER, "a", 1, 1), std::make_optional<TokenType>(TokenType::SEMICOLON), cerr);

  auto result = cerr.str();
  ASSERT_TRUE(result.find(RED_COL "[ERROR] " RESET_COL "Unexpected token: a expected: ;") != std::string::npos);
}

TEST_F(LexerTest, ifTest) {
  // Expect two strings not to be equal.
  auto stream = std::stringstream("if cond then fi");
  Lexer::TokenStream ts(stream);
  ASSERT_THAT(Map(ts.toList(), [](Token tok) {return tok.type;}), ElementsAre(TokenType::KEYWORD_IF, TokenType::IDENTIFIER, TokenType::KEYWORD_THEN, TokenType::KEYWORD_FI));
  auto streamNested = std::stringstream("if cond then if cond then fi fi");
  Lexer::TokenStream tsNested(streamNested);
  ASSERT_THAT(Map(tsNested.toList(), [](Token tok) {return tok.type;}), ElementsAre(TokenType::KEYWORD_IF, TokenType::IDENTIFIER, TokenType::KEYWORD_THEN, TokenType::KEYWORD_IF, TokenType::IDENTIFIER, TokenType::KEYWORD_THEN, TokenType::KEYWORD_FI, TokenType::KEYWORD_FI));

  auto streamIfCond = std::stringstream("if 1 = 1 then fi");
  Lexer::TokenStream tsIfCond(streamIfCond);
  ASSERT_THAT(Map(tsIfCond.toList(), [](Token tok) {return tok.type;}), ElementsAre(TokenType::KEYWORD_IF, TokenType::NUMBER, TokenType::OPERATOR_EQ, TokenType::NUMBER, TokenType::KEYWORD_THEN, TokenType::KEYWORD_FI));
}

TEST_F(LexerTest, variableAssignmentTest) {
  auto stream = std::stringstream("a := 1");
  Lexer::TokenStream ts(stream);
  ASSERT_THAT(Map(ts.toList(), [](Token tok) {return tok.type;}), ElementsAre(TokenType::IDENTIFIER, TokenType::OPERATOR_ASSIGN, TokenType::NUMBER));

  auto streamExpr = std::stringstream("a:=1+2");
  Lexer::TokenStream tsExpr(streamExpr);
  ASSERT_THAT(Map(tsExpr.toList(), [](Token tok) {return tok.type;}), ElementsAre(TokenType::IDENTIFIER, TokenType::OPERATOR_ASSIGN, TokenType::NUMBER, TokenType::OPERATOR_ADD, TokenType::NUMBER));
}

TEST_F(LexerTest, whileTest) {
  auto stream = std::stringstream("int64 i := 0;");
  Lexer::TokenStream ts(stream);
  ASSERT_THAT(Map(ts.toList(), [](Token tok) {return tok.type;}), ElementsAre(TokenType::TYPE, TokenType::IDENTIFIER, TokenType::OPERATOR_ASSIGN, TokenType::NUMBER, TokenType::SEMICOLON));
}

TEST_F(LexerTest, crlf)
{
  auto stream = std::stringstream("int64 i := 0;\r\nint64 j := 0;\r\n");
  Lexer::TokenStream ts(stream);
  ASSERT_THAT(Map(ts.toList(), [](Token tok) {return tok.type;}), ElementsAre(TokenType::TYPE, TokenType::IDENTIFIER, TokenType::OPERATOR_ASSIGN, TokenType::NUMBER, TokenType::SEMICOLON, TokenType::TYPE, TokenType::IDENTIFIER, TokenType::OPERATOR_ASSIGN, TokenType::NUMBER, TokenType::SEMICOLON));
}

TEST_F(LexerTest, parseFunction)
{
  auto stream = std::stringstream("function main() return int32 is return 0; endfunction");
  Lexer::TokenStream ts(stream);
  ASSERT_THAT(Map(ts.toList(), [](Token tok) {return tok.type;}), ElementsAre(TokenType::KEYWORD_FUNCTION, TokenType::IDENTIFIER, TokenType::PARENTHESIS_OPEN, TokenType::PARENTHESIS_CLOSE, TokenType::KEYWORD_RETURN, TokenType::TYPE, TokenType::KEYWORD_IS, TokenType::KEYWORD_RETURN, TokenType::NUMBER, TokenType::SEMICOLON, TokenType::KEYWORD_ENDFUNCTION));
}