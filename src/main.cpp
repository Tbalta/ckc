#include "lexer.hpp"
#include <iostream>
#include <fstream>

#include <llvm/Support/raw_ostream.h>
#include "parser.hpp"
#include "visitor/printVisitor.hpp"
#include "visitor/llvmVisitor.hpp"
#include "visitor/pragmaVisitor.hpp"
#include "visitor/typeVisitor.hpp"
#include "visitor/rangeVisitor.hpp"
#include "visitor/macroVisitor.hpp"
#include "exception/type_error.hpp"
#include "exception/function_error.hpp"

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

#include "colors.hpp"

using namespace llvm;
int main(int argc, char **argv)
{
    const std::string usage = "Usage: " + std::string(argv[0]) + " [options] [file]\n"
                                                                 "Options:\n"
                                                                 "  -s, --silent        Do not print AST\n"
                                                                 "  --print-llvm        Print LLVM IR\n"
                                                                 "  -h, --help          Print this help message\n";
    int silent = 0;
    int print_llvm = 0;
    std::istream *input = nullptr;
    std::string inputFileName = "";
    static struct option long_options[] = {
        {"silent", no_argument, &silent, 's'},
        {"print-llvm", no_argument, &print_llvm, 1},
        {"help", no_argument, nullptr, 'h'},
        {0, 0, 0, 0}};
    int c;
    for (int i = 0; optind + i < argc; i += optind)
    {
        while ((c = getopt_long(argc - i, argv + i, "sh", long_options, nullptr)) != -1)
        {
            switch (c)
            {
            case 0:
                break;
            case 1:
                break;
            case 's':
                silent = 1;
                break;
            case 'h':
                std::cout << usage;
                return 0;
            default:
                std::cout << usage;
                return 1;
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
        delete input;
        errs() << "No input file\n";
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
    auto TargetMachine = std::shared_ptr<llvm::TargetMachine>(Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM));

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
    auto contextProvider = Context::ContextProvider::getInstance();

    auto Context = std::make_shared<LLVMContext>();
    auto Mod = std::make_shared<Module>("my cool jit", *Context);
    Mod->setDataLayout(TargetMachine->createDataLayout());
    Mod->setTargetTriple(TargetTriple);
    // Create a new builder for the module.
    auto Builder = std::make_shared<IRBuilder<>>(*Context);
    // auto function = Function::Create(FunctionType::get(Type::getInt32Ty(*Context), false), Function::ExternalLinkage, "main", Mod.get());
    // auto entry = BasicBlock::Create(*Context, "entry", function);

    // Builder->SetInsertPoint(entry);
    visitor::llvmVisitor lv{Context, Builder, Mod, contextProvider};
    visitor::pragmaVisitor pragmaVisitor;
    visitor::typeVisitor typeVisitor;
    visitor::macroVisitor macroVisitor;
    std::vector<Parser::NodeIdentifier> nodes;
    bool error = false;
    while (!ts.isEmpty())
    {
        Parser::NodeIdentifier nodeMain = Parser::NodeIdentifier();
        try
        {
            nodeMain = Parser::parseBlock(ts);
        }
        catch (const std::exception &e)
        {
            std::cout << e.what() << std::endl;
            break;
        }
        if (nodeMain.get() == nullptr)
            break;
        visitor::PrintVisitor pv;
        if (!silent)
        {
            nodeMain.get()->accept(pv);
            std::cout << std::endl;
        }
        nodeMain.get()->accept(macroVisitor);
        nodeMain.get()->accept(pragmaVisitor);
        try
        {
            nodeMain->accept(typeVisitor);
        }
        catch (type_error &e)
        {
            error = true;
            std::cerr << ERROR_MESSAGE " " << std::string(e.what()) << std::endl;
            visitor::rangeVisitor rv;
            e.node->accept(rv);
            ts.highlightMultiplesTokens(std::vector<std::pair<Lexer::Token, Lexer::Token>>{{rv.firstToken.value(), rv.lastToken.value()}});
        }
        catch (different_type_error &e)
        {
            error = true;
            std::cerr << ERROR_MESSAGE " " << std::string(e.what()) << std::endl;
            std::vector<std::pair<Lexer::Token, Lexer::Token>> tokens;
            visitor::rangeVisitor rv;
            e.nodeA->accept(rv);
            tokens.push_back({rv.firstToken.value(), rv.lastToken.value()});
            rv = visitor::rangeVisitor();
            e.nodeB->accept(rv);
            tokens.push_back({rv.firstToken.value(), rv.lastToken.value()});
            ts.highlightMultiplesTokens(tokens);
        }
        catch (function_definition_error &e)
        {
            error = true;
            std::cerr << ERROR_MESSAGE " " << std::string(e.what()) << std::endl;
            visitor::rangeVisitor rv;
            e.original_declaration->accept(rv);
            std::cerr << "Original declaration is here:" << std::endl;
            ts.printLine(rv.firstToken.value().line);
            rv = visitor::rangeVisitor();
            e.new_declaration->accept(rv);
            std::cerr << "New declaration is here:" << std::endl;
            ts.printLine(rv.firstToken.value().line);

            
        }
        
        nodes.push_back(nodeMain);
    }
    delete input;
    if (Parser::hasError())
    {
        std::cerr << "Error parsing file\n";
        return 1;
    }
    if (error)
        return 1;
    for (auto &node : nodes)
    {
        node.get()->accept(lv);
    }

    // Builder->CreateUnreachable();
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