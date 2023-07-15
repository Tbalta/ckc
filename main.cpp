#include "lexer.hpp"
#include <iostream>
#include <fstream>

#include <llvm/Support/raw_ostream.h>
#include "parser.hpp"
#include "visitor/printVisitor.hpp"
#include "visitor/llvmVisitor.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <getopt.h>

using namespace llvm;
int main(int argc, char **argv)
{
    int silent = 0;
    int print_llvm = 0;
    std::istream *input = nullptr;
    std::string inputFileName = "";
    static struct option long_options[] = {
        {"silent", no_argument, &silent, 's'},
        {"print-llvm", no_argument, &print_llvm, 0},
        {0, 0, 0, 0}};
    int c;
    for (int i = 0; optind + i < argc; i += optind)
    {
        while ((c = getopt_long(argc - i, argv + i, "s", long_options, nullptr)) != -1)
        {
            switch (c)
            {
            case 0:
                break;
            default:
                input = new std::ifstream(optarg);
                inputFileName = optarg;
                break;
            }
        }
        if (optind + i < argc)
        {
            delete input;
            input = new std::ifstream(argv[optind + i]);
            inputFileName = argv[optind + i];
            if (input->fail())
            {
                std::cout << "File not found: " << argv[optind + i] << std::endl;
                return 1;
            }
        }
    }

    if (input == nullptr)
    {
        errs() << "No input file";
        return 1;
    }
    auto TargetTriple = sys::getDefaultTargetTriple();

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    if (!Target)
    {
        errs() << Error;
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Optional<llvm::Reloc::Model>();
    auto TargetMachine = std::unique_ptr<llvm::TargetMachine>(Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM));

    // if (!file.is_open())
    // {
    //     std::cout << "Could not open file: " << input << std::endl;
    //     return 1;
    // }
    if (input == nullptr)
    {
        std::cout << "No input file" << std::endl;
        return 1;
    }
    Lexer::TokenStream ts(*input, inputFileName);
    auto contextProvider = Context::ContextProvider();

    auto Context = std::make_shared<LLVMContext>();
    auto Mod = std::make_shared<Module>("my cool jit", *Context);
    Mod->setDataLayout(TargetMachine->createDataLayout());
    Mod->setTargetTriple(TargetTriple);
    // Create a new builder for the module.
    auto Builder = std::make_shared<IRBuilder<>>(*Context);
    auto function = Function::Create(FunctionType::get(Type::getInt32Ty(*Context), false), Function::ExternalLinkage, "main", Mod.get());
    auto entry = BasicBlock::Create(*Context, "entry", function);

    Builder->SetInsertPoint(entry);
    visitor::llvmVisitor lv{Context, Builder, Mod, contextProvider};
    while (!ts.isEmpty())
    {
        std::unique_ptr<Parser::NodeBlock> nodeMain = Parser::parseBlock(ts);
        if (nodeMain == nullptr)
            break;
        visitor::PrintVisitor pv;
        if (!silent)
        {
            nodeMain->accept(pv);
            std::cout << std::endl;
        }
        nodeMain->accept(lv);
    }
    delete input;
    if (Parser::hasError())
    {
        return 1;
    }

    Builder->CreateUnreachable();
    if (print_llvm)
    Mod->print(llvm::outs(), nullptr);

    auto Filename = "output.o";
    std::error_code EC;
    raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

    if (EC)
    {
        errs() << "Could not open file: " << EC.message();
        return 1;
    }

    legacy::PassManager pass;
    auto FileType = CGFT_ObjectFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType))
    {
        errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }

    pass.run(*Mod);
    dest.flush();

    return 0;
}