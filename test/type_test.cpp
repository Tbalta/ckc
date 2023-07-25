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
    ASSERT_THROW(node->accept(visitor), different_type_error);
}

TEST_F(TypeTest, wrongAssignment) {
    auto stream = std::stringstream("int64 i := 0; int64 j := 0; int32 z := i + j;");
    MockTokenStream ts(stream);
    Parser::NodeIdentifier node = Parser::parseMultiBlock(ts);
    visitor::typeVisitor visitor;
    ASSERT_THROW(node->accept(visitor), type_error);
}

