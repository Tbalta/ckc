#pragma once
#include <string>
#include <map>
#include "llvm/IR/Instruction.h"
#include <vector>
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
        std::vector<std::map<std::string, variable>> variables;
        std::vector<std::map<std::string, std::string>> nameTranslation{
            std::map<std::string, std::string>()
        };
        std::map<std::string, llvm::BasicBlock *> namedBlocks;

        ContextProvider() = default;
    public:
        static ContextProvider& getInstance()
        {
            static ContextProvider instance;
            return instance;
        }
        void enterScope();
        void exitScope();
        void addVariable(std::string name, llvm::AllocaInst *value, std::string type);
        variable getVariable(std::string name);
        void addBasicBlock(std::string name, llvm::BasicBlock *block);
        void addNameTranslation(std::string name, std::string translation);
        std::optional<std::string> getNameTranslation(std::string name);
        llvm::BasicBlock *getBasicBlock(std::string name);
        ~ContextProvider() = default;
    };
}