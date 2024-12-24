/**
 * @file unreachableVisitor.hpp
 * @brief Detect and report unreachable code, report node disrupting execution flow
 * @version 0.1
 * @date 2024-12-24
 */

#pragma once

#include <vector>
#include "visitor/baseVisitor.hpp"

namespace visitor
{
    class unreachableVisitor : public baseVisitor
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