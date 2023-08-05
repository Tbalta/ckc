#include "visitor/rangeVisitor.hpp"

namespace visitor
{
    void rangeVisitor::setMin(Lexer::Token token)
    {
        if (!firstToken.has_value())
        {
            firstToken = token;
            return;
        }
        if (firstToken.value().line == token.line)
        {
            if (firstToken.value().column > token.column)
                firstToken = token;
        }
        else if (firstToken.value().line > token.line)
            firstToken = token;
    }
    void rangeVisitor::setMax(Lexer::Token token)
    {
        if (!lastToken.has_value())
        {
            lastToken = token;
            return;
        }
        if (lastToken.value().line == token.line)
        {
            if (lastToken.value().column < token.column)
                lastToken = token;
        }
        else if (lastToken.value().line < token.line)
            lastToken = token;
    }

    void rangeVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        setMin(node.token.value());
        // setMin(node.fiToken.value());
    }
    void rangeVisitor::visitNodeGoto(Parser::NodeGoto &node)
    {
    }
    void rangeVisitor::visitBinOperator(Parser::NodeBinOperator &node)
    {
        node.left->accept(*this);
        node.right->accept(*this);
    }
    void rangeVisitor::visitNode(Parser::Node &node){}
    void rangeVisitor::visitNodeNumber(Parser::NodeNumber &node)
    {
        setMin(node.token.value());
        setMax(node.token.value());
    }
    void rangeVisitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node){}
    void rangeVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node){}
    void rangeVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node){}
    void rangeVisitor::visitNodeText(Parser::NodeText &node)
    {
        setMin(node.token.value());
        setMax(node.token.value());
    }
    void rangeVisitor::visitNodeReturn(Parser::NodeReturn &node){}
    void rangeVisitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node){}
    void rangeVisitor::visitNodeFunction(Parser::NodeFunction &node){
        setMin(node.token.value());
        setMax(node.endfunctionToken);
    }
    void rangeVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node){
        setMin(node.token.value());
        setMax(node.closeParen);
    }
    void rangeVisitor::visitNodePragma(Parser::NodePragma &node){}
    void rangeVisitor::visitNodeCast(Parser::NodeCast &node){
        setMin(node.token.value());
        setMax(node.closeParen);
    }
}