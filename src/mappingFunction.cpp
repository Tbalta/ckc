
#include "mappingFunction.hpp"
#include "lexer.hpp"
#include <string>
#include <cassert>

namespace mappingFunction
{
    Parser::NodeIdentifier desugarFor(Parser::NodeIdentifier node)
    {
        return desugarFor(*node.get());
    }

    Parser::NodeIdentifier desugarFor(Parser::Node &node)
    {
        if (!isTypeOf<Parser::NodeFor>(node))
            return node.thisNode;
        
        Parser::NodeFor nodeFor = dynamic_cast<Parser::NodeFor &>(node);

        auto body = nodeFor.body.get<Parser::NodeMultiBlock>();
        assert(body != nullptr);
        std::string label = "for_" + std::to_string(node.thisNode.id);

        auto namedBlock = std::make_shared<Parser::NodeBlockModifier>(Lexer::ModifierType::Named, label);

        std::vector<Parser::NodeIdentifier> result;
        if (nodeFor.initialiser.has_value())
        {
            result.push_back(nodeFor.initialiser.value());
        }

        // Craft new body, append increment and goto
        if (nodeFor.increment.has_value())
            body->blocks.push_back(nodeFor.increment.value());
        
        assert(nodeFor.firstToken.has_value());
        auto gotoNode = std::make_shared<Parser::NodeGoto>(label);
        body->blocks.push_back(Parser::addNode(gotoNode));

        // Wrap body in if block if condition exists
        if (nodeFor.condition.has_value())
        {
            auto ifNode = std::make_shared<Parser::NodeIf>(nodeFor.firstToken.value(), nodeFor.lastToken.value(), nodeFor.condition.value(), nodeFor.body, std::nullopt);
            ifNode->setSymbolName(label);
            nodeFor.body->breakFlowControl = true;
            ifNode->modifier = Parser::addNode(namedBlock);
            result.push_back(Parser::addNode(ifNode));
        } else {
            nodeFor.body->setSymbolName(label);
            nodeFor.body.get<Parser::NodeMultiBlock>()->modifier = Parser::addNode(namedBlock);
            result.push_back(nodeFor.body);
        }


        return addNode(std::make_shared<Parser::NodeMultiBlock>(result));
        
    }


    


}