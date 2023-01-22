#include <gtest/gtest.h>
#include "lexer.hpp"
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  auto file = std::stringstream("if");
  Lexer::TokenStream ts(file, "test.kc");
  EXPECT_EQ(ts.get().type, Lexer::TokenType::KEYWORD_IF);
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}