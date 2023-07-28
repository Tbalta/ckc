#pragma once

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include <llvm/IR/LLVMContext.h>
#include "../genericContext.hpp"
#include "../parser.hpp"
#include "../contextProvider.hpp"

using namespace llvm;
namespace visitor
{
    void LogError(const char *Str);

    class llvmVisitor : public Parser::Visitor
    {
    private:
        std::shared_ptr<LLVMContext> context;
        std::shared_ptr<IRBuilder<>> Builder;
        std::shared_ptr<Module> TheModule;
        std::string currentType;
        Context::ContextProvider &contextProvider;
        genericContext<std::string, std::function<Value *(uint64_t)>> typeWithInitialValueContext;
        genericContext<std::string, std::function<Type *()>> typeNameContext;
        // genericContext<std::string, std::string> ckcToSymbolName;

        void enterBlock()
        {
            contextProvider.enterScope();
            // ckctoSymbolName.enterScope();
        }

        void exitBlock()
        {
            contextProvider.exitScope();
            // ckctoSymbolName.exitScope();
        }

    public:
        ~llvmVisitor() = default;
        Value *lastValue;
        llvmVisitor(std::shared_ptr<LLVMContext> context, std::shared_ptr<IRBuilder<>> Builder, std::shared_ptr<Module> module, Context::ContextProvider &contextProvider) : context(context), Builder(Builder), TheModule(module), contextProvider(contextProvider){
            typeWithInitialValueContext.add("uint8", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(8, val, false)); });
            typeWithInitialValueContext.add("uint16", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(16, val, false)); });
            typeWithInitialValueContext.add("uint32", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(32, val, false)); });
            typeWithInitialValueContext.add("uint64", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(64, val, false)); });
            typeWithInitialValueContext.add("int8", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(8, val, true)); });
            typeWithInitialValueContext.add("int16", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(16, val, true)); });
            typeWithInitialValueContext.add("int32", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(32, val, true)); });
            typeWithInitialValueContext.add("int64", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(64, val, true)); });
            typeWithInitialValueContext.add("", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(32, val, false)); });
            typeWithInitialValueContext.add("bool", [&](uint64_t val){ return ConstantInt::get(*this->context, APInt(1, val, false)); });
        
            typeNameContext.add("uint8", [&](){return Type::getInt8Ty(*this->context);}),
            typeNameContext.add("uint16", [&](){return Type::getInt16Ty(*this->context);}),
            typeNameContext.add("uint32", [&](){return Type::getInt32Ty(*this->context);}),
            typeNameContext.add("uint64", [&](){return Type::getInt64Ty(*this->context);}),
            typeNameContext.add("int8", [&](){return Type::getInt8Ty(*this->context);}),
            typeNameContext.add("int16", [&](){return Type::getInt16Ty(*this->context);}),
            typeNameContext.add("int32", [&](){return Type::getInt32Ty(*this->context);}),
            typeNameContext.add("int64", [&](){return Type::getInt64Ty(*this->context);}),
            typeNameContext.add("bool", [&](){return Type::getInt1Ty(*this->context);});
        };
        void visitNodeIf(Parser::NodeIf &node) override;
        void visitNodeGoto(Parser::NodeGoto &node) override;
        void visitBinOperator(Parser::NodeBinOperator &node) override;
        void visitNode(Parser::Node &node) override;
        void visitNodeNumber(Parser::NodeNumber &node) override;
        void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node) override;
        void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node) override;
        void visitNodeBlockModifier(Parser::NodeBlockModifier &node) override;
        void visitNodeText(Parser::NodeText &node) override;
        void visitNodeReturn(Parser::NodeReturn &node) override;
        void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node) override;
        void visitNodeFunction(Parser::NodeFunction &node) override;
        void visitNodeFunctionCall(Parser::NodeFunctionCall &node) override;
        void visitNodePragma(Parser::NodePragma &node) override;
        void visitLazyBinOperator(Parser::NodeBinOperator &node);
        void visitNodeCast(Parser::NodeCast &node) override;
    };

}