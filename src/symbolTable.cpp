#include "symbolTable.hpp"
#include "parser.hpp"
namespace CKC
{

    std::string SymbolTable::getUniqueInternalName(std::string baseName)
    {
        int i = 0;
        auto newName = "_" + baseName + "_" + std::to_string(i);

        while (genericContext::has(newName))
        {
            i++;
            newName = baseName + "_" + std::to_string(i);
        }
        return newName;
    }
}