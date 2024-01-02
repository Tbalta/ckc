#include "visitor/pragmaVisitor.hpp"
#include <iostream>

namespace visitor
{

    void pragmaVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        pragmaContext.enterScope();
        node.thenStatement.get()->accept(*this);
        pragmaContext.exitScope();
        pragmaContext.enterScope();
        if (node.elseStatement.has_value())
            node.elseStatement.value().get()->accept(*this);
        pragmaContext.exitScope();
    }

    void pragmaVisitor::visitNodeGoto(Parser::NodeGoto &node) {}

    void pragmaVisitor::visitBinOperator(Parser::NodeBinOperator &node) {}
    void pragmaVisitor::visitNode(Parser::Node &node) {}
    void pragmaVisitor::visitNodeNumber(Parser::NodeNumber &node) {}

    void pragmaVisitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
    {
        pragmaContext.add(node.name, &node);
    }
    void pragmaVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node) {}
    void pragmaVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node) {}
    void pragmaVisitor::visitNodeText(Parser::NodeText &node) {}

    void pragmaVisitor::visitNodeReturn(Parser::NodeReturn &node) {}
    void pragmaVisitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node) {}
    void pragmaVisitor::visitNodeFunction(Parser::NodeFunction &node)
    {
        pragmaContext.add(node.name, &node);
        pragmaContext.enterScope();
        if (node.body.has_value())
            node.body.value().get()->accept(*this);
        pragmaContext.exitScope();
    }

    void pragmaVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node){};

    void pragmaVisitor::visitNodePragma(Parser::NodePragma &node)
    {
        // Check if target object exists
        auto targetObjectNode = pragmaContext.get(node.targetObject.value);
        if (!targetObjectNode.has_value())
        {
            std::cerr << "Error: pragma target object " << node.targetObject.value << " does not exist" << std::endl;
            exit(1);
        }

        switch (node.pragmaType)
        {
        case Lexer::TokenType::SYMBOL_NAME:
            targetObjectNode.value()->setSymbolName(node.value);
            break;
        default:
            std::cerr << "Error: pragma type " << Lexer::tokenTypeToString(node.pragmaType) << " not implemented" << std::endl;
            exit(1);
        }
    }
    void pragmaVisitor::visitNodeCast(Parser::NodeCast &node) {};
    void pragmaVisitor::visitNodePartial(Parser::NodePartial &node) {};

}