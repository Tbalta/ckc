#pragma once

#include "parser.hpp"
#include <iterator>
#include <vector>

#include "../genericContext.hpp"
namespace visitor
{

    class functionType : public genericContext<int, std::vector<std::string>>
    {
        public:
            std::string functionName;
            std::optional<std::string> returnType;
            functionType(std::string functionName, std::optional<std::string> returnType) : functionName(functionName), returnType(returnType){};
            functionType() : functionName(""), returnType(std::nullopt) {}

            void add(std::vector<std::string> types)
            {
                genericContext::add(types.size(), types);
            }
            bool compare(std::vector<std::string> a, std::vector<std::string> b)
            {
                return a.size() == b.size() && std::equal(std::begin(a), std::end(a), std::begin(b));
            }

            bool correspondToFunction(std::vector<std::string> types)
            {
                for (auto it = contextStack.rbegin(); it != contextStack.rend(); it++)
                {
                    for (auto it2 = it->begin(); it2 != it->end(); it2++)
                    {
                        if (compare(it2->second, types))
                        {
                            return true;
                        }
                    }
                }
                return false;
            }

            std::vector<std::vector<std::string>> getAllTypes()
            {
                std::vector<std::vector<std::string>> types;
                for (auto it = contextStack.rbegin(); it != contextStack.rend(); it++)
                {
                    for (auto it2 = it->begin(); it2 != it->end(); it2++)
                    {
                        types.push_back(it2->second);
                    }
                }
                return types;
            }
    };

class typeVisitor : public Parser::Visitor
{
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

        typeVisitor() = default;
    private:
        std::map<std::string, functionType> functions;
        genericContext<std::string, std::string> variables;
        std::optional<std::string> currentFunction;
        std::string lastType;
        std::string hintType;
};
}