#pragma once
#include <string>
#include <map>
#include <llvm/IR/Instruction.h>
#include <vector>
// builder
#include <llvm/IR/IRBuilder.h>
#include "genericContext.hpp"
#include "node.hpp"
namespace Context
{
    class variable
    {
    public:
        std::string type;
        llvm::AllocaInst *value;
    };

    class functionType // : public genericContext<int, std::pair<std::string, std::vector<std::string>>>
    {
    public:
        class functionInfo
        {
            public:
            std::vector<std::string> types;
            std::string returnType;
            Parser::NodeIdentifier node;
            int id;
            functionInfo(std::vector<std::string> types, std::string returnType, Parser::NodeIdentifier node, int id) : types(types), returnType(returnType), node(node), id(id){};
        };
        std::vector<functionInfo> overloads;

        std::string functionName;
        std::optional<std::string> returnType;
        functionType(std::string functionName, std::optional<std::string> returnType) : functionName(functionName), returnType(returnType){};
        functionType() : functionName(""), returnType(std::nullopt) {}
        int overloadCount = 0;

        
        size_t getOverloadCount()
        {
            return overloads.size();
        }
        
        bool hasOverload(std::vector<std::string> types)
        {
            for (auto overload : this->overloads)
            {
                if (compare(overload.types, types))
                    return true;
            }
            return false;
        }

        std::optional<functionInfo> getDefinition(std::vector<std::string> types)
        {
            for (auto overload : this->overloads)
            {
                if (compare(overload.types, types))
                    return overload;
            }
            return std::nullopt;
        }


        int add(std::vector<std::string> types, std::string returnType, Parser::NodeIdentifier node)
        {
            // genericContext::add(0, types);
            this->overloads.push_back({types, returnType, node, overloadCount});
            return overloadCount++;
        }

        bool compare(std::vector<std::string> a, std::vector<std::string> b)
        {
            return a.size() == b.size() && std::equal(std::begin(a), std::end(a), std::begin(b));
        }

        std::vector<functionInfo> getAllTypes()
        {
            return overloads;
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
        bool removeOverload(std::string name, int overload);
        std::optional<std::string> getNameTranslation(std::string name);
        llvm::BasicBlock *getBasicBlock(std::string name);
        std::map<std::string, functionType> functions;

        ~ContextProvider() = default;
    };
}