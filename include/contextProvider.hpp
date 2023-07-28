#pragma once
#include <string>
#include <map>
#include "llvm/IR/Instruction.h"
#include <vector>
// builder
#include "llvm/IR/IRBuilder.h"
#include "genericContext.hpp"
namespace Context
{
    class variable
    {
    public:
        std::string type;
        llvm::AllocaInst *value;
    };

    class functionType : public genericContext<int, std::vector<std::string>>
    {
    public:
        std::string functionName;
        std::optional<std::string> returnType;
        functionType(std::string functionName, std::optional<std::string> returnType) : functionName(functionName), returnType(returnType){};
        functionType() : functionName(""), returnType(std::nullopt) {}

        void add(std::vector<std::string> types)
        {
            genericContext::add(types.size(), types);
        }
        bool compare(std::vector<std::string> a, std::vector<std::string> b)
        {
            return a.size() == b.size() && std::equal(std::begin(a), std::end(a), std::begin(b));
        }

        bool correspondToFunction(std::vector<std::string> types)
        {
            for (auto it = contextStack.rbegin(); it != contextStack.rend(); it++)
            {
                for (auto it2 = it->begin(); it2 != it->end(); it2++)
                {
                    if (compare(it2->second, types))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        std::vector<std::vector<std::string>> getAllTypes()
        {
            std::vector<std::vector<std::string>> types;
            for (auto it = contextStack.rbegin(); it != contextStack.rend(); it++)
            {
                for (auto it2 = it->begin(); it2 != it->end(); it2++)
                {
                    types.push_back(it2->second);
                }
            }
            return types;
        }
    };

    class ContextProvider
    {
    private:
        std::vector<std::map<std::string, variable>> variables;
        std::vector<std::map<std::string, std::string>> nameTranslation{
            std::map<std::string, std::string>()};
        std::map<std::string, llvm::BasicBlock *> namedBlocks;

        ContextProvider() = default;

    public:
        static ContextProvider &getInstance()
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
        std::map<std::string, functionType> functions;

        ~ContextProvider() = default;
    };
}