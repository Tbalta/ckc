#include "visitor/macroVisitor.hpp"
#include "symbolTable.hpp"

namespace visitor
{
    class renameVisitor : public Parser::Visitor
    {
        public:
        std::map<std::string, std::string> variableReplacements;
        void visitNodeText(Parser::NodeText &node)
        {
            if (variableReplacements.find(node.name) != variableReplacements.end())
                node.name = variableReplacements[node.name];
        }
        renameVisitor(std::map<std::string, std::string> variableReplacements) : variableReplacements(variableReplacements) {}
    };

    Parser::NodeIdentifier macroVisitor::createNewBlockFromPartial(Parser::NodeFunctionCall &partialCall)
    {
        auto partialFunctionTest = partialFunctionContext.get(partialCall.name);
        if (!partialFunctionTest.has_value())
            return partialCall.thisNode;

        auto partialFunction = partialFunctionTest.value().get<Parser::NodePartial>();
        auto linkedCall = partialFunction->linkedFunction;


        std::map<std::string, std::string> variableReplacements;
        for (auto &arg : partialFunction->arguments)
        {
            variableReplacements[arg.second] = SymbolTable::getUniqueName(partialFunction->name + "_" + arg.second + "_");
            SymbolTable::addSymbol(variableReplacements[arg.second]);
        }

        // Create new variable declaration for every argument
        std::vector<Parser::NodeIdentifier> blocks;
        for (auto i = 0; i < partialFunction->arguments.size(); i++)
        {
            auto arg = partialFunction->arguments[i];
            auto newVariable = std::make_shared<Parser::NodeVariableDeclaration>(
                partialCall.token.value(),
                arg.first,
                variableReplacements[arg.second],
                std::move(partialCall.arguments[i])
            );
            blocks.push_back(Parser::addNode(newVariable));
        }

        // Add the linked function call
        renameVisitor renameVisitor(variableReplacements);
        linkedCall->accept(renameVisitor);
        blocks.push_back(linkedCall);

        // Create new block
        auto newBlock = std::make_shared<Parser::NodeMultiBlockExpression>( blocks);
        auto newNode = Parser::addNode(newBlock);
        newNode->accept(*this);
        return newNode;
    }

    void macroVisitor::visitNodePartial(Parser::NodePartial &node)
    {
        if (partialFunctionContext.has(node.name))
            throw std::runtime_error("Partial function " + node.name + " already defined");
 
        partialFunctionContext.add(node.name, node.thisNode);
    }

    // Find partial function call and replace by linked function call.
    void macroVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        auto partialFunction = partialFunctionContext.get(node.name);
        if (!partialFunction.has_value())
            return;
        auto newBlock = createNewBlockFromPartial(node);
        Parser::replaceNode(node.thisNode, newBlock);
    }



}