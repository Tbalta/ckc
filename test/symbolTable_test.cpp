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
    symbolTable.add("_a_0", 1);
    ASSERT_NE(symbolTable.getUniqueInternalName("a"), "a");
    ASSERT_NE(symbolTable.getUniqueInternalName("a"), "a0");
}

TEST_F(SymbolTableTest, GetUniqueSymbol_SymbolPresentInBroaderScope)
{
    symbolTable.add("a", 1);
    symbolTable.enterScope();
    ASSERT_NE(symbolTable.getUniqueInternalName("a"), "a");
}

TEST_F(SymbolTableTest, GetUniqueSymbol_SymbolNotInScope)
{
    ASSERT_EQ(symbolTable.getUniqueInternalName("a"), "_a_0");
}
