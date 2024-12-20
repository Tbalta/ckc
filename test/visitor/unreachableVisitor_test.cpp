#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "lexer.hpp"
#include "parser.hpp"
#include <ostream>
#include <string>
#include "visitor/unreachableVisitor.hpp"
using namespace Lexer;
using namespace testing;

class UnreachableVisitorTest : public ::testing::Test {
 protected:
  void SetUp() override {
    Lexer::LexerContext::init();
  }
};

TEST_F(UnreachableVisitorTest, noUnreachableCode) {
  auto stream = std::stringstream("function main () return int32 is return 0; endfunction");
  Lexer::TokenStream ts(stream);
  auto node = Parser::parseBlock(ts);
  visitor::unreachableVisitor uv;
  node.get()->accept(uv);
  ASSERT_EQ(uv.unreachableNodes.size(), 0);
}

TEST_F(UnreachableVisitorTest, unreachableCodeInFunc) {
  auto stream = std::stringstream("function main () return int32 is return 0; return 1; endfunction");
  Lexer::TokenStream ts(stream);
  auto node = Parser::parseBlock(ts);
  visitor::unreachableVisitor uv;
  node.get()->accept(uv);
  ASSERT_EQ(uv.unreachableNodes.size(), 1);
}

TEST_F(UnreachableVisitorTest, reachableCodeAfterIf) {
  auto stream = std::stringstream("function main () return int32 is if x = 1 then return 0; fi  return 1; endfunction");
  Lexer::TokenStream ts(stream);
  auto node = Parser::parseBlock(ts);
  visitor::unreachableVisitor uv;
  node.get()->accept(uv);
  ASSERT_EQ(uv.unreachableNodes.size(), 0);
}

TEST_F(UnreachableVisitorTest, unreachableCodeAfterIf) {
  auto stream = std::stringstream("function main () return int32 is if x = 1 then return 0; else return 1; fi  return 2; endfunction");
  Lexer::TokenStream ts(stream);
  auto node = Parser::parseBlock(ts);
  visitor::unreachableVisitor uv;
  node.get()->accept(uv);
  ASSERT_EQ(uv.unreachableNodes.size(), 1);
}