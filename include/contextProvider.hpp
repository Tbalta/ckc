#pragma once
#include <string>
#include <map>
#include "llvm/IR/Instruction.h"
// builder
#include "llvm/IR/IRBuilder.h"
namespace Context
{
    class variable
    {
    public:
        std::string type;
        llvm::AllocaInst *value;
    };

    class ContextProvider
    {
    private:
        std::map<std::string, variable> variables;
        std::map<std::string, llvm::BasicBlock *> namedBlocks;
    public:
        void addVariable(std::string name, llvm::AllocaInst *value, std::string type);
        variable getVariable(std::string name);
        void addBasicBlock(std::string name, llvm::BasicBlock *block);
        llvm::BasicBlock *getBasicBlock(std::string name);
        ~ContextProvider() = default;
    };
}