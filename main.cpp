#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include "parser.hpp"
#include "visitor/printVisitor.hpp"
int main(void)
{
    auto file = std::ifstream("test.kc");
    Lexer::TokenStream ts(file);
    while (!ts.isEmpty())
    {
        std::unique_ptr<Parser::NodeBlock> nodeMain = Parser::parseBlock(ts);
        visitor::PrintVisitor pv;
        nodeMain->accept(pv);
        std::cout << std::endl;
    }
    return 0;
}