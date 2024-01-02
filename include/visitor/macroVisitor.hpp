#pragma once
#include "../parser.hpp"
#include "../genericContext.hpp"

namespace visitor
{
    class macroVisitor : public Parser::Visitor
    {
    public:
        void visitNodePartial(Parser::NodePartial &node) override;
        void visitNodeFunctionCall(Parser::NodeFunctionCall &node);
        macroVisitor(/* args */) = default;
        ~macroVisitor() = default;
    private:
        genericContext<std::string, Parser::NodeIdentifier> partialFunctionContext;
        Parser::NodeIdentifier createNewBlockFromPartial(Parser::NodeFunctionCall &partialCall);
    }; 
}
