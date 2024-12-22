#include "mappingFunction.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

class MappingFunctionTest : public ::testing::Test {
};

TEST_F(MappingFunctionTest, isTypeOf_True) {
    Parser::NodeFor node;
    ASSERT_TRUE(mappingFunction::isTypeOf<Parser::NodeFor>(node));
}

TEST_F(MappingFunctionTest, isTypeOf_False) {
    Parser::NodeFor node;
    ASSERT_FALSE(mappingFunction::isTypeOf<Parser::NodeIf>(node));
}

TEST_F(MappingFunctionTest, desugarFor_ShouldReturnId) {
    auto node = *Parser::addNode(std::make_shared<Parser::NodeIf>()).get().get();
    auto result = mappingFunction::desugarFor(node);
    ASSERT_TRUE(node.thisNode.id == result.id);
}

TEST_F(MappingFunctionTest, desugarFor_ShouldReturnNodeInIf) {
    auto initialiser = Parser::addNode(std::make_shared<Parser::Node>());
    auto increment = Parser::addNode(std::make_shared<Parser::Node>());
    auto condition = Parser::addNode(std::make_shared<Parser::Node>());
    auto body = Parser::addNode(std::make_shared<Parser::NodeMultiBlock>());
    
    auto forNode = Parser::addNode(std::make_shared<Parser::NodeFor>(Lexer::Token(), initialiser, condition, increment, body));
    auto result = mappingFunction::desugarFor(forNode).get<Parser::NodeMultiBlock>();
    ASSERT_TRUE(result != nullptr);

    ASSERT_EQ(2, result->blocks.size());
    ASSERT_TRUE(result->blocks[0]->thisNode.id == initialiser.id);
    auto ifNode = result->blocks[1].get<Parser::NodeIf>();
    ASSERT_TRUE(ifNode != nullptr);
    ASSERT_EQ(2, ifNode->thenStatement.get<Parser::NodeMultiBlock>()->blocks.size());
    ASSERT_TRUE(ifNode->thenStatement.get<Parser::NodeMultiBlock>()->blocks[0].id == increment.id);
}

TEST_F(MappingFunctionTest, desugarFor_ShouldReturnNodeNotInIf) {
    auto initialiser = Parser::addNode(std::make_shared<Parser::Node>());
    auto increment = Parser::addNode(std::make_shared<Parser::Node>());
    auto body = Parser::addNode(std::make_shared<Parser::NodeMultiBlock>());
    
    auto forNode = Parser::addNode(std::make_shared<Parser::NodeFor>(Lexer::Token(), initialiser, std::nullopt, increment, body));
    auto result = mappingFunction::desugarFor(forNode).get<Parser::NodeMultiBlock>();
    ASSERT_TRUE(result != nullptr);

    ASSERT_EQ(2, result->blocks.size());
    ASSERT_TRUE(result->blocks[0]->thisNode.id == initialiser.id);
    ASSERT_TRUE(body.id == result->blocks[1].id);
    ASSERT_EQ(2, result->blocks[1].get<Parser::NodeMultiBlock>()->blocks.size());
    ASSERT_TRUE(result->blocks[1].get<Parser::NodeMultiBlock>()->blocks[0].id == increment.id);
}