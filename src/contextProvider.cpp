#include "contextProvider.hpp"
#include "llvm/IR/IRBuilder.h"

namespace Context
{

    void ContextProvider::addVariable(std::string name, llvm::AllocaInst *value, std::string type)
    {
        variables.back()[name] = variable{type, value};
    }

    variable ContextProvider::getVariable(std::string name)
    {
        for (auto it = variables.rbegin(); it != variables.rend(); ++it)
        {
            if (it->find(name) != it->end())
                return it->at(name);
        }
        return variable{"", nullptr};
    }

    void ContextProvider::addBasicBlock(std::string name, llvm::BasicBlock *block)
    {
        namedBlocks[name] = block;
    }

    llvm::BasicBlock *ContextProvider::getBasicBlock(std::string name)
    {
        return namedBlocks[name];
    }

    void ContextProvider::enterScope()
    {
        variables.push_back(std::map<std::string, variable>());
        nameTranslation.push_back(std::map<std::string, std::string>());
    }

    void ContextProvider::exitScope()
    {
        variables.pop_back();
        nameTranslation.pop_back();
    }

            void ContextProvider::addNameTranslation(std::string name, std::string translation)
            {
                nameTranslation.back()[name] = translation;
            }
        std::optional<std::string> ContextProvider::getNameTranslation(std::string name)
        {
            for (auto it = nameTranslation.rbegin(); it != nameTranslation.rend(); ++it)
            {
                if (it->find(name) != it->end())
                    return it->at(name);
            }
            return std::nullopt;
        }

}