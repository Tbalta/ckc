#include "visitor/macroVisitor.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "lexer.hpp"
#include "parser.hpp"
#include <ostream>
#include <string>

using namespace testing;

class MacroVisitorTest : public ::testing::Test {
 protected:
    visitor::macroVisitor macroVisitor;
    Parser::NodeIdentifier linkedFunctionCall;
    Parser::NodeIdentifier partialFunctionDef;
    Parser::NodeIdentifier partialFunctionCall;
    std::vector<std::pair<std::string, std::string>> functionArguments = {{"int32", "a"}, {"int32", "b"}};
    std::vector<std::pair<std::string, std::string>> partialArguments = {{"int32", "a"}};
    std::vector<Parser::NodeIdentifier> functionParameters;
    std::vector<Parser::NodeIdentifier> partialArgumentsNodes;

    const std::string linkedFunctionName = "linkedFunction";
    const std::string partialFunctionName = "partialFunction";
    void SetUp() override {
        Lexer::LexerContext::init();
        partialArgumentsNodes.push_back(Parser::addNode(std::make_shared<Parser::Node>()));
        functionParameters.push_back(Parser::addNode(std::make_shared<Parser::NodeText>("b", Lexer::Token())));
        functionParameters.push_back(Parser::addNode(std::make_shared<Parser::NodeText>("a", Lexer::Token())));

        linkedFunctionCall = Parser::addNode(std::make_shared<Parser::NodeFunctionCall>(Lexer::Token(), linkedFunctionName, functionParameters, Lexer::Token()));
        partialFunctionDef = Parser::addNode(std::make_shared<Parser::NodePartial>(Lexer::Token(), partialFunctionName, partialArguments, linkedFunctionCall));
        partialFunctionCall = Parser::addNode(std::make_shared<Parser::NodeFunctionCall>(Lexer::Token(), partialFunctionName, partialArgumentsNodes, Lexer::Token()));
    }
};


TEST_F(MacroVisitorTest, createNewBlockFromPartialCall){
    partialFunctionDef->accept(macroVisitor);
    partialFunctionCall->accept(macroVisitor);

    auto newBlock = macroVisitor.newNode.get<Parser::NodeMultiBlockExpression>();
    ASSERT_NE(newBlock, nullptr);
    ASSERT_EQ(newBlock->blocks.size(), 2);
    ASSERT_NE(newBlock->blocks[0].get<Parser::NodeVariableDeclaration>(), nullptr);
    ASSERT_NE(newBlock->blocks[1].get<Parser::NodeFunctionCall>(), nullptr);

    auto functionCall = newBlock->blocks[1].get<Parser::NodeFunctionCall>();
    ASSERT_EQ(functionCall->name, linkedFunctionName);
}

TEST_F(MacroVisitorTest, visitNodeFunctionCall_nodeIsPartial){
    partialFunctionDef->accept(macroVisitor);
    partialFunctionCall->accept(macroVisitor);
    ASSERT_FALSE(Parser::compareNodes(partialFunctionCall, macroVisitor.newNode));
}

TEST_F(MacroVisitorTest, visitNodeFunctionCall_nodeIsNotPartial){
    linkedFunctionCall->accept(macroVisitor);
    ASSERT_TRUE(Parser::compareNodes(linkedFunctionCall, macroVisitor.newNode));
}

TEST_F(MacroVisitorTest, visitNodePartial){
    partialFunctionDef->accept(macroVisitor);
    
    auto newBlock = macroVisitor.newNode.get<Parser::NodeMultiBlock>();
    ASSERT_NE(newBlock, nullptr);
    ASSERT_EQ(newBlock->blocks.size(), 1);
    
    auto variableDeclaration = newBlock->blocks[0].get<Parser::NodeVariableDeclaration>();
    ASSERT_NE(variableDeclaration, nullptr);
}