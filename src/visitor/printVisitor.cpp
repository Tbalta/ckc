#include "visitor/printVisitor.hpp"
#include <iomanip>
namespace visitor
{
    void PrintVisitor::enterNode(Parser::Node &node)
    {
        printNewLine(node.firstToken);
    }

    void PrintVisitor::printNewLine(std::optional<Lexer::Token> token)
    {
        const int newLineNumber = token.has_value() ? token.value().line : currentLine;
        
        if (newLine || newLineNumber != currentLine)
        {
            printNewLine(newLineNumber);
        }

        newLine = false;
    }

    void PrintVisitor::printNewLine(int newLineNumber)
    {
        out << std::endl;
        if (newLineNumber != currentLine)
        {
            out << std::right << std::setfill(' ') << std::setw(4) << newLineNumber << std::left << std::setw(2 + (5 * level)) << " |";
        } else
        {
            out << std::right << std::setfill(' ') << std::setw(4) << " " << std::left << std::setw(2 + (5 * level)) << " |";
        }


        currentLine = newLineNumber;
    }
    



    void PrintVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        out << "if ";
        node.condition.get()->accept(*this);
        out << " then ";
        node.thenStatement.get()->accept(*this);
        if (node.elseStatement)
        {
            out << " else ";
            node.elseStatement.value().get()->accept(*this);
        }
        printNewLine(node.fiToken.line);
        out << "fi";
    }
    void PrintVisitor::visitNodeGoto(Parser::NodeGoto &node)
    {
        out << "goto " << node.label;
        newLine = true;
    }
    void PrintVisitor::visitNodeBinOperator(Parser::NodeBinOperator &node)
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
        newLine = true;
    }

    void PrintVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
    {
        if (node.modifier.has_value())
            node.modifier.value().get()->accept(*this);
        out << node.name << " = ";
        node.value.get()->accept(*this);
        newLine = true;
    }
    void PrintVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node)
    {
        out << "#named" << " " << node.modifier_value << " ";
        out << std::endl;
        out << std::right << std::setfill(' ') << std::setw(4) << " " << std::left << std::setw(5) << " |";
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
        
        printNewLine(currentLine);
        out << "endFunction";
        printNewLine(currentLine);
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

    void PrintVisitor::visitNodeCast(Parser::NodeCast &node)
    {
        out << node.type << "(";
        node.value->accept(*this);
        out << ")";
    }
    void PrintVisitor::visitNodePartial(Parser::NodePartial &node) {
        out << "partial " << node.name << " ";
        out << "(";
        for (ssize_t i = 0; i < ssize_t(node.arguments.size() - 1); i++)
        {
            out << node.arguments[i].first << " " << node.arguments[i].second << ", ";
        }
        if (node.arguments.size() > 0)
            out << (node.arguments.end() - 1)->first << " " << (node.arguments.end() - 1)->second;
        out << ")";
    };

    void PrintVisitor::visitNodeMultiBlockExpression(Parser::NodeMultiBlockExpression &node)
    {
        level++;
        for (auto &block : node.blocks)
        {
            newLine = true;
            block.get()->accept(*this);
        }
        level--;
    }

    void PrintVisitor::visitNodeMultiBlock(Parser::NodeMultiBlock &node)
    {
        level++;
        for (auto &block : node.blocks)
        {
            block.get()->accept(*this);
        }
        level--;
    }



}