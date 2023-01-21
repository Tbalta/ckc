#include "lexer.hpp"
#include <iostream>
#include <fstream>

#include <llvm/Support/raw_ostream.h>
#include "parser.hpp"
#include "visitor/printVisitor.hpp"
#include "visitor/llvmVisitor.hpp"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

using namespace llvm;
int main(void)
{
    auto file = std::ifstream("test.kc");
    Lexer::TokenStream ts(file);
    auto contextProvider = Context::ContextProvider();

    auto Context = std::make_shared<LLVMContext>();
    auto Mod = std::make_shared<Module>("my cool jit", *Context);

    // Create a new builder for the module.
    auto Builder = std::make_shared<IRBuilder<>>(*Context);
    auto function = Function::Create(FunctionType::get(Type::getInt32Ty(*Context), false), Function::ExternalLinkage, "main", Mod.get());
    auto entry = BasicBlock::Create(*Context, "entry", function);

    Builder->SetInsertPoint(entry);
    visitor::llvmVisitor lv{Context, Builder, Mod, contextProvider};
    while (!ts.isEmpty())
    {
        std::unique_ptr<Parser::NodeBlock> nodeMain = Parser::parseBlock(ts);
        visitor::PrintVisitor pv;
        nodeMain->accept(pv);
        nodeMain->accept(lv);
        std::cout << std::endl;
    }
    Mod->print(llvm::errs(), nullptr);
    return 0;
}