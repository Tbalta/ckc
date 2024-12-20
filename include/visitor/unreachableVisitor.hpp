/*
* unreachableVisitor.hpp
* This visitor reports unreachable code, and set 
*/

#pragma once

#include "../parser.hpp"
#include <vector>

namespace visitor
{
    class unreachableVisitor : public Parser::Visitor
    {
        private:
        public:
            unreachableVisitor() = default;
            std::vector<Parser::NodeIdentifier> unreachableNodes;
            void visitNodeMultiBlock(Parser::NodeMultiBlock &node) override;
            void visitNodeIf(Parser::NodeIf &node) override;
            void visitNodeReturn(Parser::NodeReturn &node) override;
            void visitNodeGoto(Parser::NodeGoto &node) override;
    };
}