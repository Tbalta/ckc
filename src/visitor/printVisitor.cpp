#include "visitor/printVisitor.hpp"
#include <iomanip>
namespace visitor
{
    void PrintVisitor::enterNode(Parser::Node &node)
    {
        if (!node.token.has_value())
            return;
        if (node.token.value().line != currentLine)
        {
            currentLine = node.token.value().line;
            out << std::endl;
            out << std::right << std::setfill(' ') << std::setw(4) << currentLine << std::left << std::setw(5) << " |" ;
        }
    }

    void PrintVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        if (node.modifier.has_value())
            node.modifier.value().get()->accept(*this);
        out << "if ";
        node.condition.get()->accept(*this);
        out << " then ";
        node.thenStatement.get()->accept(*this);
        if (node.elseStatement)
        {
            out << " else ";
            node.elseStatement.value().get()->accept(*this);
        }
        out << " fi";
    }
    void PrintVisitor::visitNodeGoto(Parser::NodeGoto &node)
    {
        out << "goto " << node.label;
    }
    void PrintVisitor::visitBinOperator(Parser::NodeBinOperator &node)
    {
        out << "(";
        node.left.get()->accept(*this);
        out << " " << Lexer::tokenTypeToString(node.op) << " ";
        node.right.get()->accept(*this);
        out << ")";
    }

    void PrintVisitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node)
    {
        out << Lexer::tokenTypeToString(node.op) << " ";
        node.right.get()->accept(*this);
    }

    void PrintVisitor::visitNode(Parser::Node &node)
    {
        // out << node.value;
    }
    void PrintVisitor::visitNodeNumber(Parser::NodeNumber &node)
    {
        out << node.value;
    }
    void PrintVisitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
    {
        if (node.modifier.has_value())
            node.modifier.value().get()->accept(*this);
        out << node.type << " " << node.name;
        if (node.value)
        {
            out << " = ";
            node.value.value().get()->accept(*this);
        }
    }

    void PrintVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
    {
        if (node.modifier.has_value())
            node.modifier.value().get()->accept(*this);
        out << node.name << " = ";
        node.value.get()->accept(*this);
    }
    void PrintVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node)
    {
        out << "named"
                  << " " << node.modifier_value << " ";
    }
    void PrintVisitor::visitNodeText(Parser::NodeText &node)
    {
        out << node.name;
    }

    void PrintVisitor::visitNodeReturn(Parser::NodeReturn &node)
    {
        out << "return ";
        if (node.value.has_value())
            node.value.value()->accept(*this);
    }

    void PrintVisitor::visitNodeFunction(Parser::NodeFunction &node)
    {
        out << "function " << node.name << " ";
        out << "(";
        for (ssize_t i = 0; i < ssize_t(node.arguments.size() - 1); i++)
        {
            out << node.arguments[i].first << " " << node.arguments[i].second << ", ";
        }
        if (node.arguments.size() > 0)
            out << (node.arguments.end() - 1)->first << " " << (node.arguments.end() - 1)->second;
        out << ")";
        if (node.returnType.has_value())
            out << " return " << node.returnType.value();
        if (node.body.has_value())
            node.body.value().get()->accept(*this);
    }

    void PrintVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        out << node.name << "(";
        for (ssize_t i = 0; i < ssize_t(node.arguments.size() - 1); i++)
        {
            node.arguments[i].get()->accept(*this);
            out << ", ";
        }
        if (node.arguments.size() > 0)
            (node.arguments.end() - 1)->get()->accept(*this);

        out << ")";
    }

    void PrintVisitor::visitNodePragma(Parser::NodePragma &node)
    {
        out << "pragma " << Lexer::tokenTypeToString(node.pragmaType) << " ";
        out << node.value;
    }

}