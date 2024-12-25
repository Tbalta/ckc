/**
 * @file macroVisitor.hpp
 * @brief Replace partial function definition and partial function call.
 * @version 0.2
 * @date 2024-12-24
 */
#pragma once
#include "../genericContext.hpp"
#include "mapVisitor.hpp"
#include "symbolTable.hpp"
#include "visitor/baseVisitor.hpp"
namespace visitor
{
    class macroVisitor : public mapVisitor
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
        Parser::NodeIdentifier createBlockFromPartialCall(Parser::NodeFunctionCall &partialCall);
    }; 
}
