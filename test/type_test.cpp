#include "lexer_test.hpp"
#include "parser.hpp"
#include <gtest/gtest.h>
#include "visitor/typeVisitor.hpp"
#include "exception/type_error.hpp"
#include "lexer_test.hpp"


using namespace testing;

class TypeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    Lexer::LexerContext::init();
  }
};

TEST_F(TypeTest, wrongAddition) {
    auto stream = std::stringstream("int64 i := 0; int32 j := 0; int32 z := i + j;");
    MockTokenStream ts(stream);
    Parser::NodeIdentifier node = Parser::parseMultiBlock(ts);
    visitor::typeVisitor visitor;
    EXPECT_THAT([&](){node->accept(visitor);}, ThrowsMessage<different_type_error>("Operation between different types: int64 and int32 (missing cast?)"));
}

TEST_F(TypeTest, wrongAssignment) {
    auto stream = std::stringstream("int64 i := 0; int64 j := 0; int32 z := i + j;");
    MockTokenStream ts(stream);
    Parser::NodeIdentifier node = Parser::parseMultiBlock(ts);
    visitor::typeVisitor visitor;
    EXPECT_THAT([&](){node->accept(visitor);}, ThrowsMessage<type_error>("Type error: expected int32 but got int64"));
}

TEST_F(TypeTest, wrongReturn_Message)
{
    auto stream = std::stringstream("function foo() return int32 is return uint32(0); endfunction");
    MockTokenStream ts(stream);
    Parser::NodeIdentifier node = Parser::parseMultiBlock(ts);
    visitor::typeVisitor visitor;
    EXPECT_THAT([&](){node->accept(visitor);}, ThrowsMessage<type_error>("Type error: expected int32 but got uint32"));
}