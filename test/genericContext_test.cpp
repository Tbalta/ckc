#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "genericContext.hpp"
#include <string>

using namespace testing;

class GenericContextTest : public ::testing::Test {
 protected:
    genericContext<std::string, int> context;

};



TEST_F(GenericContextTest, addBasic){
    context.add("a", 1);
    ASSERT_TRUE(context.has("a"));
    ASSERT_EQ(context.get("a").value(), 1);
}

TEST_F(GenericContextTest, removeBasic){
    context.add("a", 1);
    context.remove("a");
    ASSERT_FALSE(context.has("a"));
}

TEST_F(GenericContextTest, addPopContext){
    context.enterScope();
    context.add("a", 1);
    ASSERT_TRUE(context.has("a"));
    ASSERT_EQ(context.get("a").value(), 1);
    context.exitScope();
    ASSERT_FALSE(context.has("a"));
}

TEST_F(GenericContextTest, exitScopeWithoutEntering){
    context.exitScope();
    ASSERT_FALSE(context.has("a"));
}

TEST_F(GenericContextTest, addInTwoScopes){
    context.enterScope();
    context.add("a", 1);
    context.enterScope();
    context.add("a", 2);
    ASSERT_TRUE(context.has("a"));
    ASSERT_EQ(context.get("a").value(), 2);
    context.exitScope();
    ASSERT_TRUE(context.has("a"));
    ASSERT_EQ(context.get("a").value(), 1);
}
