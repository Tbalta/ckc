#include "visitor/macroVisitor.hpp"
#include "visitor/copyVisitor.hpp"
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

    class findClosure : public Parser::Visitor
    {
    private:
        bool inClosure = false;

    public:
        std::set<std::string> constantVariables;
        std::set<Parser::NodeIdentifier> closureExpressions;

        void visitNodeBinOperator(Parser::NodeBinOperator &node)
        {
            node.left->accept(*this);
            auto leftClosure = inClosure;
            node.right->accept(*this);
            auto rightClosure = inClosure;

            inClosure = leftClosure && rightClosure;
            if (inClosure)
                return;
            if (leftClosure)
                closureExpressions.insert(node.left);
            if (rightClosure)
                closureExpressions.insert(node.right);
        }

        void visitNodeText(Parser::NodeText &node)
        {
            inClosure = constantVariables.find(node.name) == constantVariables.end();
        }

        void visitNodeNumber(Parser::NodeNumber &node)
        {
            inClosure = false;
        }

        void visitNodeFunctionCall(Parser::NodeFunctionCall &node)
        {
            inClosure = true;
        }
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
        }

        // Create new variable declaration for every argument
        std::vector<Parser::NodeIdentifier> blocks;
        for (auto i = 0; i < partialFunction->arguments.size(); i++)
        {
            auto arg = partialFunction->arguments[i];
            auto newVariable = std::make_shared<Parser::NodeVariableDeclaration>(
                partialCall.firstToken.value(),
                arg.first,
                variableReplacements[arg.second],
                std::move(partialCall.arguments[i]));
            blocks.push_back(Parser::addNode(newVariable));
        }

        // Add the linked function call
        renameVisitor renameVisitor(variableReplacements);
        copyVisitor copyVisitor;
        linkedCall->accept(copyVisitor);
        linkedCall = copyVisitor.newCopy;

        linkedCall->accept(renameVisitor);
        blocks.push_back(linkedCall);

        // Create new block
        auto newBlock = std::make_shared<Parser::NodeMultiBlockExpression>(blocks);
        auto newNode = Parser::addNode(newBlock);
        newNode->firstToken = partialCall.firstToken;
        newNode->lastToken = partialCall.lastToken;
        newNode->accept(*this);
        return newNode;
    }

    void macroVisitor::visitNodePartial(Parser::NodePartial &node)
    {
        if (partialFunctionContext.has(node.name))
            throw std::runtime_error("Partial function " + node.name + " already defined");

        partialFunctionContext.add(node.name, node.thisNode);
        findClosure findClosure;
        node.linkedFunction->accept(findClosure);
        // Create variable for every closure expression
        std::map<Parser::NodeIdentifier, std::string> closureVariables;
        std::vector<Parser::NodeIdentifier> blocks;
        copyVisitor copyVisitor;
        for (auto &closureExpression : findClosure.closureExpressions)
        {
            closureExpression->accept(copyVisitor);
            auto name = SymbolTable::getUniqueName(node.name + "_");
            auto newVariable = std::make_shared<Parser::NodeVariableDeclaration>(
                closureExpression->firstToken.value(),
                closureExpression.get<Parser::NodeExpression>()->type,
                name,
                copyVisitor.newCopy);
            blocks.push_back(Parser::addNode(newVariable));
            closureVariables[closureExpression] = name;
        }

        // Insert blocks in parent
        parent.get<Parser::NodeMultiBlock>()->blocks.insert(
            parent.get<Parser::NodeMultiBlock>()->blocks.begin(),
            blocks.begin(),
            blocks.end());

        // Replace closure expressions by variables
        for (auto variable : closureVariables)
        {
            // Create variable load
            auto newVariable = std::make_shared<Parser::NodeText>(
                variable.second,
                variable.first->firstToken.value());
            newVariable->type = variable.first.get<Parser::NodeExpression>()->type;
            Parser::replaceNode(variable.first, Parser::addNode(newVariable));
        }
    }

    // Find partial function call and replace by linked function call.
    void macroVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        auto partialFunction = partialFunctionContext.get(node.name);
        if (partialFunction.has_value())
        {
            auto newBlock = createNewBlockFromPartial(node);
            Parser::replaceNode(node.thisNode, newBlock);
        }
        for (auto &arg : node.arguments)
            arg->accept(*this);
    }

    void macroVisitor::visitNodeMultiBlock(Parser::NodeMultiBlock &node)
    {
        partialFunctionContext.enterScope();
        auto previousParent = parent;
        parent = node.thisNode;
        Visitor::visitNodeMultiBlock(node);
        parent = previousParent;
        partialFunctionContext.exitScope();
    }

}