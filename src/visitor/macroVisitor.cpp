#include "visitor/macroVisitor.hpp"
#include "visitor/copyVisitor.hpp"
#include "symbolTable.hpp"
#include <cassert>
namespace visitor
{
    class renameVisitor : public baseVisitor
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

    class findClosure : public baseVisitor
    {
    private:

    public:
        bool inClosure = false;
        std::set<std::string> constantVariables;
        std::set<Parser::NodeIdentifier> closureExpressions;
        findClosure(std::set<std::string> constantVariables) : constantVariables(constantVariables) {}
        findClosure() = default;
        void visitNodeBinOperator(Parser::NodeBinOperator &node)
        {
            inClosure = false;
            node.left->accept(*this);
            auto leftClosure = inClosure;
            inClosure = false;
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

    class findSymbolVisitor : public baseVisitor
    {
        public:
        CKC::SymbolTable& symbolTable;
        findSymbolVisitor (CKC::SymbolTable &symbolTable) : symbolTable(symbolTable){};

        void visitNodeFunction(Parser::NodeFunction &node)
        {
            symbolTable.add(node.name, node.thisNode);
        }

        void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
        {
            symbolTable.add(node.name, node.thisNode);
        }

        void visitNodePartial(Parser::NodePartial &node)
        {
            symbolTable.add(node.name, node.thisNode);
        }
    };


    Parser::NodeIdentifier macroVisitor::createNewBlockFromPartial(Parser::NodeFunctionCall &partialCall)
    {
        auto optionalPartialFunction = partialFunctionContext.get(partialCall.name);
        assert(optionalPartialFunction.has_value());
        if (!optionalPartialFunction.has_value())
            return partialCall.thisNode;
    
        auto partialFunction = optionalPartialFunction.value().get<Parser::NodePartial>();
        assert(partialFunction != nullptr);
        
        for (auto &arg : partialFunction->arguments)
        {
        }

        // Create new variable declaration for every argument
        std::map<std::string, std::string> variableReplacements;
        std::vector<Parser::NodeIdentifier> blocks;
        for (auto i = 0; i < partialFunction->arguments.size(); i++)
        {
            auto arg = partialFunction->arguments[i];
            auto name = symbolTable.getUniqueName(partialFunction->name + "_" + arg.second + "_");
            variableReplacements[arg.second] = name;

            auto newVariable = std::make_shared<Parser::NodeVariableDeclaration>(
                partialCall.firstToken.value(),
                arg.first,
                name,
                std::move(partialCall.arguments[i]));
            auto argumentNodeId = Parser::addNode(newVariable);
            symbolTable.add(name, argumentNodeId);
            blocks.push_back(argumentNodeId);
        }

        // Add the linked function call
        renameVisitor renameVisitor(variableReplacements);
        copyVisitor copyVisitor;
        auto linkedCall = partialFunction->linkedFunction;
        linkedCall->accept(copyVisitor);
        linkedCall = copyVisitor.newCopy;

        linkedCall->accept(renameVisitor);
        blocks.push_back(linkedCall);

        // Create new block
        auto newBlock = std::make_shared<Parser::NodeMultiBlockExpression>(blocks);
        auto result = Parser::addNode(newBlock);
        result->firstToken = partialCall.firstToken;
        result->lastToken = partialCall.lastToken;
        result->accept(*this);
        return result;
    }

    void macroVisitor::visitNodePartial(Parser::NodePartial &node)
    {
        if (partialFunctionContext.has(node.name))
            throw std::runtime_error("Partial function " + node.name + " already defined");


        partialFunctionContext.add(node.name, node.thisNode);
        
        std::set<std::string> constantVariables;
        for (auto &arg : node.arguments)
        {
            constantVariables.insert(arg.second);
        }

        auto linkedFunction = node.linkedFunction.get<Parser::NodeFunctionCall>();
        
        findClosure findClosure(constantVariables);
        for (auto &arg : linkedFunction->arguments)
        {
            findClosure.inClosure = false;
            arg->accept(findClosure);
            if (findClosure.inClosure)
                findClosure.closureExpressions.insert(arg);
        }
        

        // Create variable for every closure expression
        std::map<Parser::NodeIdentifier, std::string> closureVariables;
        std::vector<Parser::NodeIdentifier> blocks;
        for (auto &closureExpression : findClosure.closureExpressions)
        {
            copyVisitor copyVisitor;
            closureExpression->accept(copyVisitor);
            assert(copyVisitor.newCopy.id != -1);
            auto name = symbolTable.getUniqueName(node.name + "_");
            auto newVariable = std::make_shared<Parser::NodeVariableDeclaration>(
                node.firstToken.value(),
                closureExpression.get<Parser::NodeExpression>()->type,
                name,
                copyVisitor.newCopy);
            
            auto newVariableID = Parser::addNode(newVariable);
            symbolTable.add(name, newVariableID);
            blocks.push_back(newVariableID);
            closureVariables[closureExpression] = name;
        }

        // Replace closure expressions by variables
        for (auto variable : closureVariables)
        {
            // Create variable load
            auto newVariable = std::make_shared<Parser::NodeText>(
                variable.second,
                node.firstToken.value());
            newVariable->type = variable.first.get<Parser::NodeExpression>()->type;
            Parser::replaceNode(variable.first, Parser::addNode(newVariable));
        }

        newNode = Parser::addNode(std::make_shared<Parser::NodeMultiBlock>(blocks));
    }

    // Find partial function call and replace by linked function call.
    void macroVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        auto partialFunction = partialFunctionContext.get(node.name);
        auto newID = node.thisNode;
        if (partialFunction.has_value())
        {
            auto newBlock = createNewBlockFromPartial(node);
            newID = newBlock;
        }
        for (auto &arg : node.arguments)
        {
            arg->accept(*this);
            arg = newNode;
        }

        newNode = newID;
    }

    void macroVisitor::visitNodeMultiBlock(Parser::NodeMultiBlock &node)
    {
        partialFunctionContext.enterScope();
        symbolTable.enterScope();
        findSymbolVisitor findSymbolVisitor(symbolTable);
        node.accept(findSymbolVisitor);
        for (auto &block : node.blocks)
        {
            block->accept(*this);
            block = newNode;
        }
        symbolTable.exitScope();
        partialFunctionContext.exitScope();
        newNode = node.thisNode;
    }

    void macroVisitor::enterNode(Parser::Node &node)
    {
    }
}