#pragma once

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include <llvm/IR/LLVMContext.h>

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
        llvmVisitor(std::shared_ptr<LLVMContext> context, std::shared_ptr<IRBuilder<>> Builder, std::shared_ptr<Module> module, Context::ContextProvider &contextProvider) : context(context), Builder(Builder), TheModule(module), contextProvider(contextProvider){};
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
    };

}