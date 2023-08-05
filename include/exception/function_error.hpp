#pragma once

#include "lexer.hpp"
#include "parser.hpp"


class function_definition_error : public std::exception
{
    public:
    Parser::NodeIdentifier original_declaration;
    Parser::NodeIdentifier new_declaration;
    std::string msg;
    function_definition_error(Parser::NodeIdentifier original_declaration, Parser::NodeIdentifier new_declaration) : original_declaration(original_declaration), new_declaration(new_declaration)
    {
        msg = "Function already exist";
    }
    const char *what() const throw()
    {
        return msg.c_str();
    }
};
