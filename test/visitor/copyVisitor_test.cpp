#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "lexer.hpp"
#include "parser.hpp"
#include <ostream>
#include <string>
#include "visitor/copyVisitor.hpp"

using namespace testing;

class CopyVisitorTest : public ::testing::Test {
 protected:
    visitor::copyVisitor copyVisitor;
    void SetUp() override {
        Lexer::LexerContext::init();
    }
};


TEST_F(CopyVisitorTest, CopyNodeIf){
    auto nodeCondition = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeThen = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeElse = Parser::addNode(std::make_shared<Parser::Node>());

    auto nodeIf = Parser::addNode(std::make_shared<Parser::NodeIf>(Lexer::Token(), Lexer::Token(), nodeCondition, nodeThen, nodeElse));
    
    nodeIf->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeIf, copyVisitor.newCopy));
    ASSERT_NE(nodeIf->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeGoto){
    auto nodeGoto = Parser::addNode(std::make_shared<Parser::NodeGoto>("label"));
    nodeGoto->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeGoto, copyVisitor.newCopy));
    ASSERT_NE(nodeGoto->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeBinOperator){
    auto nodeLeft = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeRight = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeBinOperator = Parser::addNode(std::make_shared<Parser::NodeBinOperator>(nodeLeft, nodeRight, Lexer::TokenType::OPERATOR_ADD));
    nodeBinOperator->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeBinOperator, copyVisitor.newCopy));
    ASSERT_NE(nodeBinOperator->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeUnaryOperator){
    auto nodeRight = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeUnaryOperator = Parser::addNode(std::make_shared<Parser::NodeUnaryOperator>(Lexer::Token(), nodeRight, Lexer::TokenType::OPERATOR_ADD));
    nodeUnaryOperator->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeUnaryOperator, copyVisitor.newCopy));
    ASSERT_NE(nodeUnaryOperator->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeNumber){
    auto nodeNumber = Parser::addNode(std::make_shared<Parser::NodeNumber>(1, Lexer::Token()));
    nodeNumber->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeNumber, copyVisitor.newCopy));
    ASSERT_NE(nodeNumber->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeVariableDeclaration){
    auto nodeValue = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeVariableDeclaration = Parser::addNode(std::make_shared<Parser::NodeVariableDeclaration>(Lexer::Token(), "int", "name", nodeValue));
    nodeVariableDeclaration->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeVariableDeclaration, copyVisitor.newCopy));
    ASSERT_NE(nodeVariableDeclaration->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeVariableAssignment){
    auto nodeValue = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeVariableAssignment = Parser::addNode(std::make_shared<Parser::NodeVariableAssignment>(Lexer::Token(), "name", nodeValue));
    nodeVariableAssignment->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeVariableAssignment, copyVisitor.newCopy));
    ASSERT_NE(nodeVariableAssignment->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeBlockModifier){
    auto nodeBlockModifier = Parser::addNode(std::make_shared<Parser::NodeBlockModifier>(Lexer::ModifierType::Named, "name"));
    nodeBlockModifier->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeBlockModifier, copyVisitor.newCopy));
    ASSERT_NE(nodeBlockModifier->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeText){
    auto nodeText = Parser::addNode(std::make_shared<Parser::NodeText>("name", Lexer::Token()));
    nodeText->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeText, copyVisitor.newCopy));
    ASSERT_NE(nodeText->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeMultiBlock){
    auto nodeBlock = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeMultiBlock = Parser::addNode(std::make_shared<Parser::NodeMultiBlock>(std::vector<Parser::NodeIdentifier>{nodeBlock}));
    nodeMultiBlock->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeMultiBlock, copyVisitor.newCopy));
    ASSERT_NE(nodeMultiBlock->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeReturn){
    auto nodeValue = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeReturn = Parser::addNode(std::make_shared<Parser::NodeReturn>(Lexer::Token(), nodeValue));
    nodeReturn->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeReturn, copyVisitor.newCopy));
    ASSERT_NE(nodeReturn->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeFunction){
    auto nodeBody = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeFunction = Parser::addNode(std::make_shared<Parser::NodeFunction>(Lexer::Token(), "name", std::vector<std::pair<std::string, std::string>>(),"type", nodeBody, Lexer::Token()));
    nodeFunction->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeFunction, copyVisitor.newCopy));
    ASSERT_NE(nodeFunction->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeFunctionCall){
    auto nodeArgument = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeFunctionCall = Parser::addNode(std::make_shared<Parser::NodeFunctionCall>(Lexer::Token(), "name", std::vector<Parser::NodeIdentifier>{nodeArgument}, Lexer::Token()));
    nodeFunctionCall->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeFunctionCall, copyVisitor.newCopy));
    ASSERT_NE(nodeFunctionCall->thisNode.id, copyVisitor.newCopy.id);
}
TEST_F(CopyVisitorTest, CopyNodePragma){
    auto nodePragma = Parser::addNode(std::make_shared<Parser::NodePragma>(Lexer::Token(), Lexer::TokenType::SYMBOL_NAME, "name", Lexer::Token()));
    nodePragma->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodePragma, copyVisitor.newCopy));
    ASSERT_NE(nodePragma->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeCast){
    auto nodeValue = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeCast = Parser::addNode(std::make_shared<Parser::NodeCast>(Lexer::Token(), "type", nodeValue, Lexer::Token()));
    nodeCast->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeCast, copyVisitor.newCopy));
    ASSERT_NE(nodeCast->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeFor){
    auto initializer = Parser::addNode(std::make_shared<Parser::Node>());
    auto condition = Parser::addNode(std::make_shared<Parser::Node>());
    auto increment = Parser::addNode(std::make_shared<Parser::Node>());
    auto body = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeFor = Parser::addNode(std::make_shared<Parser::NodeFor>(Lexer::Token(), initializer, condition, increment, body));
    nodeFor->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeFor, copyVisitor.newCopy));
    ASSERT_NE(nodeFor->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodePartial){
    auto linkedFunction = Parser::addNode(std::make_shared<Parser::NodeFunctionCall>(Lexer::Token(), "name", std::vector<Parser::NodeIdentifier>(), Lexer::Token()));
    auto nodePartial = Parser::addNode(std::make_shared<Parser::NodePartial>(Lexer::Token(), "name", std::vector<std::pair<std::string, std::string>>(), linkedFunction));
    nodePartial->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodePartial, copyVisitor.newCopy));
    ASSERT_NE(nodePartial->thisNode.id, copyVisitor.newCopy.id);
}

TEST_F(CopyVisitorTest, CopyNodeMultiBlockExpression){
    auto nodeBlock = Parser::addNode(std::make_shared<Parser::Node>());
    auto nodeMultiBlockExpression = Parser::addNode(std::make_shared<Parser::NodeMultiBlockExpression>(std::vector<Parser::NodeIdentifier>{nodeBlock}));
    nodeMultiBlockExpression->accept(copyVisitor);
    ASSERT_TRUE(Parser::compareNodes(nodeMultiBlockExpression, copyVisitor.newCopy));
    ASSERT_NE(nodeMultiBlockExpression->thisNode.id, copyVisitor.newCopy.id);
}