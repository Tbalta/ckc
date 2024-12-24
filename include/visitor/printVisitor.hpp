/**
 * @file printVisitor.hpp
 * @brief Print the AST
 * @version 0.1
 * @date 2024-12-24
 */
#pragma once

#include <iostream>
#include <sstream>
#include "visitor/baseVisitor.hpp"

namespace visitor
{
    class PrintVisitor : public baseVisitor
    {
    private:
        int currentLine = 0;
        bool newLine = true;
        int level = 0;
        std::ostream &out;
        void visitNodeIf(Parser::NodeIf &node) override;
        void visitNodeGoto(Parser::NodeGoto &node) override;
        void visitNodeBinOperator(Parser::NodeBinOperator &node) override;
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
        void enterNode(Parser::Node &node) override;
        void printNewLine(int newLineNumber);
        void printNewLine(std::optional<Lexer::Token> token);
        void visitNodeCast(Parser::NodeCast &node) override;
        void visitNodePartial(Parser::NodePartial &node) override;
        void visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node) override;
        void visitNodeMultiBlock(Parser::NodeMultiBlock &node) override;


    public:
        PrintVisitor() : out(std::cout){};
        PrintVisitor(std::ostream &out) : out(out){};
    };
}