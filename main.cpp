#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include "parser.hpp"
#include "visitor/printVisitor.hpp"
int main(void)
{
    auto file = std::ifstream("test.kc");
    Lexer::TokenStream ts(file);
    std::unique_ptr<Parser::NodeStatement> nodeMain = Parser::parseStatement(ts);
    visitor::PrintVisitor pv;
    nodeMain->accept(pv);
    return 0;
}