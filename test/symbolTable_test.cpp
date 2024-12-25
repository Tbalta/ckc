#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "symbolTable.hpp"
#include <string>

using namespace testing;

class SymbolTableTest : public ::testing::Test {
 protected:
    CKC::SymbolTable symbolTable;
};


TEST_F(SymbolTableTest, GetUniqueSymbol_SymbolPresentInScope)
{
    symbolTable.add("a", 1);
    symbolTable.add("a0", 1);
    ASSERT_NE(symbolTable.getUniqueName("a"), "a");
    ASSERT_NE(symbolTable.getUniqueName("a"), "a0");
}

TEST_F(SymbolTableTest, GetUniqueSymbol_SymbolPresentInBroaderScope)
{
    symbolTable.add("a", 1);
    symbolTable.enterScope();
    ASSERT_NE(symbolTable.getUniqueName("a"), "a");
}

TEST_F(SymbolTableTest, GetUniqueSymbol_SymbolNotInScope)
{
    ASSERT_EQ(symbolTable.getUniqueName("a"), "a");
}
