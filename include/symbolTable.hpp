#pragma once
#include <set>
#include <string>
#include "genericContext.hpp"
#include "parser.hpp"

namespace CKC
{
    class SymbolTable : public genericContext<std::string, Parser::NodeIdentifier>
    {
        public:
        void removeSymbol(std::string name);
        std::string getUniqueInternalName(std::string name);
    };

}