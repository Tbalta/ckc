#include "lexer.hpp"
#include <iostream>
#include <fstream>
#include "parser.hpp"
#include "visitor/printVisitor.hpp"
int main(void)
{
    auto file = std::ifstream("test.kc");
    Lexer::TokenStream ts(file);
    Parser::Node node = Parser::parse(ts);
    visitor::PrintVisitor pv;
    node.accept(pv);
    return 0;
}