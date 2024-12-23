#include "symbolTable.hpp"
#include "parser.hpp"
namespace CKC
{

    std::string SymbolTable::getUniqueName(std::string name)
    {
        int i = 0;
        while (genericContext::has(name + std::to_string(i)))
        {
            i++;
        }
        auto newName = name + std::to_string(i);
        return newName;
    }
}