/**
 * @file closureVisitor.hpp
 * @brief Find expressions inside node closures
 * @version 0.1
 * @date 2024-12-25
 */

#pragma once
#include "visitor/baseVisitor.hpp"
#include <set>

namespace visitor
{

    class closureVisitor : public baseVisitor
    {
    public:
        std::set<std::string> constantVariables;
        std::set<Parser::NodeIdentifier> closureExpressions;
        bool inClosure = false;

        closureVisitor(std::set<std::string> constantVariables) : constantVariables(constantVariables) {}
        closureVisitor() = default;

        std::set<Parser::NodeIdentifier> getExpressionsInClosure();
        void visit(Parser::NodeIdentifier &node);

    private:
        bool isInClosure(Parser::NodeIdentifier &node);
        void visitNodeBinOperator(Parser::NodeBinOperator &node);
        void visitNodeText(Parser::NodeText &node);
        void visitNodeNumber(Parser::NodeNumber &node);
        void visitNodeFunctionCall(Parser::NodeFunctionCall &node);
    };

}