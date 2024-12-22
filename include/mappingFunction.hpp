#pragma once

#include "parser.hpp"

namespace mappingFunction
{
    template <typename T>
    bool isTypeOf(Parser::Node &node);
    
    Parser::NodeIdentifier desugarFor(Parser::Node &node);
}