#include <set>
namespace SymbolTable
{
    void addSymbol(std::string name);
    bool hasSymbol(std::string name);
    void removeSymbol(std::string name);
    std::string getUniqueName(std::string name);

}