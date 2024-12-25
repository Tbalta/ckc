#include "visitor/closureVisitor.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "lexer.hpp"
#include "parser.hpp"
#include <ostream>
#include <string>

using namespace testing;

class ClosureVisitorTest : public ::testing::Test {
 protected:

    Parser::NodeIdentifier variableA;
    Parser::NodeIdentifier variableB;

    Parser::NodeIdentifier nodeBinOperator;
    Parser::NodeIdentifier nodeFunctionCall;

    void SetUp() override {
        variableA = Parser::createNode<Parser::NodeText>("a", Lexer::Token());
        variableB = Parser::createNode<Parser::NodeText>("b", Lexer::Token());
        nodeBinOperator = Parser::createNode<Parser::NodeBinOperator>(variableA, variableB, Lexer::TokenType::OPERATOR_ADD);
        nodeFunctionCall = Parser::createNode<Parser::NodeFunctionCall>(Lexer::Token(), "function", std::vector<Parser::NodeIdentifier>({ variableA, variableB }), Lexer::Token());
    }
};


TEST_F(ClosureVisitorTest, noClosure)
{
    visitor::closureVisitor closureVisitor({ "a", "b" });
    closureVisitor.visit(nodeBinOperator);
    ASSERT_THAT(closureVisitor.getExpressionsInClosure(), IsEmpty());
}

TEST_F(ClosureVisitorTest, closureInLeftExpression)
{
    visitor::closureVisitor closureVisitor({ "b" });
    closureVisitor.visit(nodeBinOperator);
    ASSERT_THAT(closureVisitor.getExpressionsInClosure(), SizeIs(1));
    ASSERT_THAT(closureVisitor.getExpressionsInClosure(), Contains(variableA));
}

TEST_F(ClosureVisitorTest, closureInRightExpression)
{
    visitor::closureVisitor closureVisitor({ "a" });
    closureVisitor.visit(nodeBinOperator);
    ASSERT_THAT(closureVisitor.getExpressionsInClosure(), SizeIs(1));
    ASSERT_THAT(closureVisitor.getExpressionsInClosure(), Contains(variableB));
}

TEST_F(ClosureVisitorTest, closureInBoth)
{
    visitor::closureVisitor closureVisitor;
    closureVisitor.visit(nodeBinOperator);
    ASSERT_THAT(closureVisitor.getExpressionsInClosure(), SizeIs(1));
    ASSERT_THAT(closureVisitor.getExpressionsInClosure(), Contains(nodeBinOperator));
}
