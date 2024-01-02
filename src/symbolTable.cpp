#include "symbolTable.hpp"
namespace SymbolTable
{
    static std::set<std::string> symbolTable;
    void addSymbol(std::string name)
    {
        symbolTable.insert(name);
    }

    bool hasSymbol(std::string name)
    {
        return symbolTable.find(name) != symbolTable.end();
    }

    void removeSymbol(std::string name)
    {
        symbolTable.erase(name);
    }

    std::string getUniqueName(std::string name)
    {
        int i = 0;
        while (hasSymbol(name + std::to_string(i)))
        {
            i++;
        }
        return name + std::to_string(i);
    }
}