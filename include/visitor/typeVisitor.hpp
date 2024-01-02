#pragma once

#include "parser.hpp"
#include <iterator>
#include <vector>

#include "../genericContext.hpp"
#include "../contextProvider.hpp"
namespace visitor
{

class typeVisitor : public Parser::Visitor
{
    private:
        Context::ContextProvider &contextProvider = Context::ContextProvider::getInstance();
        genericContext<std::string, std::string> variables;
        std::optional<std::string> currentFunction;
        std::string lastType;
        std::string hintType;
    public:
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
        void visitNodeUnaryOperator (Parser::NodeUnaryOperator &node) override;
        void visitNodeFunction(Parser::NodeFunction &node) override;
        void visitNodeFunctionCall(Parser::NodeFunctionCall &node) override;
        void visitNodePragma(Parser::NodePragma &node) override;
        void visitBinOperatorBoolean(Parser::NodeBinOperator &node);
        void visitBinOperatorComparison(Parser::NodeBinOperator &node);
        void visitNodeCast(Parser::NodeCast &node) override;
        void visitNodePartial(Parser::NodePartial &node) override;
        void visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node) override;



        typeVisitor()
        {
            // contextProvider = Context::ContextProvider::getInstance();
        };
};
}