#pragma once

#include "parser.hpp"

namespace mappingFunction
{
    template <typename T>
    bool isTypeOf(Parser::Node &node)
    {
        return dynamic_cast<T *>(&node) != nullptr;
    }
    
    Parser::NodeIdentifier desugarFor(Parser::Node &node);
    Parser::NodeIdentifier desugarFor(Parser::NodeIdentifier node);

    Parser::NodeIdentifier cloneNode(Parser::Node &node);
    Parser::NodeIdentifier cloneNode(Parser::NodeIdentifier node);
}