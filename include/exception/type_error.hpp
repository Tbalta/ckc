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
    Parser::NodeIdentifier node;
    type_error(std::string expected_type, std::string actual_type, Parser::NodeIdentifier node) : expected_type(expected_type), actual_type(actual_type), node(node)
    {
        msg = "Type error: expected " + expected_type + " but got " + actual_type;
    };
    const char *what() const throw()
    {
        return msg.c_str();
    }
};

class generic_type_error : public std::exception
{
protected:
    std::string msg;
public:
    Parser::NodeIdentifier node;
    generic_type_error(Parser::NodeIdentifier node, std::string msg) : msg(msg), node(node) {};
    generic_type_error() : msg(""), node(-1) {};
};

class different_type_error : public std::exception
{
private:
    std::string msg;

public:
    Parser::NodeIdentifier nodeA;
    Parser::NodeIdentifier nodeB;
    different_type_error(Parser::NodeIdentifier nodeA, Parser::NodeIdentifier nodeB) : nodeA(nodeA), nodeB(nodeB)
    {
        // Operation between different types
        msg = "Operation between different types: " + nodeA.get<Parser::NodeExpression>()->type + " and " + nodeB.get<Parser::NodeExpression>()->type + " (missing cast?)";
    };
    const char *what() const throw()
    {
        return msg.c_str();
    }
};

class no_matching_function_call : public generic_type_error
{
public:
    no_matching_function_call(Parser::NodeIdentifier functionNode)
    {
        node = functionNode;
        auto function = functionNode.get<Parser::NodeFunctionCall>();
        msg = "No call match " + function->name + "(";
        for (auto i = 0; i < function->arguments.size(); i++)
        {
            auto arg = function->arguments[i];
            msg += arg.get<Parser::NodeExpression>()->type;
            if (i != function->arguments.size() - 1)
                msg += ", ";
        }
        msg += ")";
    };
    const char *what() const throw()
    {
        return msg.c_str();
    }
};
