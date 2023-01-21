#include "contextProvider.hpp"
#include "llvm/IR/IRBuilder.h"

namespace Context
{

    void ContextProvider::addVariable(std::string name, llvm::AllocaInst *value, std::string type)
    {
        variables[name] = variable{type, value};
    }

    variable ContextProvider::getVariable(std::string name)
    {
        return variables[name];
    }

    void ContextProvider::addBasicBlock(std::string name, llvm::BasicBlock *block)
    {
        namedBlocks[name] = block;
    }

    llvm::BasicBlock *ContextProvider::getBasicBlock(std::string name)
    {
        return namedBlocks[name];
    }
}