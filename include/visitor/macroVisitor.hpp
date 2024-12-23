#pragma once
#include "../parser.hpp"
#include "../genericContext.hpp"
#include "mapVisitor.hpp"
#include "symbolTable.hpp"
namespace visitor
{
    class macroVisitor : public visitor::mapVisitor
    {
    public:
        void visitNodePartial(Parser::NodePartial &node) override;
        void visitNodeFunctionCall(Parser::NodeFunctionCall &node) override;
        void visitNodeMultiBlock(Parser::NodeMultiBlock &node) override;
        void enterNode(Parser::Node &node) override;
        macroVisitor(/* args */) = default;
        ~macroVisitor() = default;
        CKC::SymbolTable symbolTable;
    private:
        genericContext<std::string, Parser::NodeIdentifier> partialFunctionContext;
        Parser::NodeIdentifier createNewBlockFromPartial(Parser::NodeFunctionCall &partialCall);
    }; 
}
