#pragma once
#include <exception>
#include <string>
#include "lexer.hpp"
#include "parser.hpp"

class type_error : public std::exception
{
private:
    std::string expected_type;
    std::string actual_type;
    std::string msg;

public:
    Lexer::Token token;
    type_error(std::string expected_type, std::string actual_type, Lexer::Token token) : expected_type(expected_type), actual_type(actual_type), token(token){
        msg = "Type error: expected " + expected_type + " but got " + actual_type;
    };
    const char *what() const throw()
    {
        return msg.c_str();
    }
};

class different_type_error : public std::exception
{
private:
    std::string msg;
    public:
    Parser::NodeIdentifier nodeA;
    Parser::NodeIdentifier nodeB;
    different_type_error(Parser::NodeIdentifier nodeA, Parser::NodeIdentifier nodeB) : nodeA(nodeA), nodeB(nodeB){
        // Operation between different types
        msg = "Operation between different types: " + nodeA.get<Parser::NodeExpression>()->type + " and " + nodeB.get<Parser::NodeExpression>()->type + " (missing cast?)";
    };
    const char *what() const throw()
    {
        return msg.c_str();
    }
};