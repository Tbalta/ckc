#include "visitor/typeVisitor.hpp"
#include "exception/type_error.hpp"
namespace visitor
{

    std::optional<std::string> resolve_collision(std::string typeA, std::string typeB, std::string default_type)
    {
        if (typeA == typeB)
            return typeA;
        if (typeA == "number" && typeB == "number")
            return default_type;
        if (typeA == "number")
            return typeB;
        if (typeB == "number")
            return typeA;
        return std::nullopt;
    }
    void typeVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        hintType = "bool";
        node.condition->accept(*this);
        hintType = "";
        if (lastType != "bool")
            throw type_error("bool", lastType, node.token.value());
        node.thenStatement->accept(*this);
        if (node.elseStatement.has_value())
            node.elseStatement.value()->accept(*this);
        lastType = "";
    }
    void typeVisitor::visitNodeGoto(Parser::NodeGoto &node)
    {
    }

    void typeVisitor::visitBinOperatorBoolean(Parser::NodeBinOperator &node)
    {
        std::string leftType = node.left.get<Parser::NodeExpression>()->type;
        std::string rightType = node.right.get<Parser::NodeExpression>()->type;
        if (!resolve_collision(leftType, "bool", "bool").has_value() || !resolve_collision(rightType, "bool", "bool").has_value())
            throw different_type_error(node.left, node.right);
        node.type = "bool";
        lastType = "bool";
    }

    void typeVisitor::visitBinOperatorComparison(Parser::NodeBinOperator &node)
    {
        std::string leftType = node.left.get<Parser::NodeExpression>()->type;
        std::string rightType = node.right.get<Parser::NodeExpression>()->type;
        if (!resolve_collision(leftType, rightType, "number").has_value())
            throw different_type_error(node.left, node.right);
        node.type = "bool";
        lastType = "bool";
    }

    void typeVisitor::visitBinOperator(Parser::NodeBinOperator &node)
    {
        std::string hint = hintType;
        hintType = "number";
        node.left->accept(*this);
        node.right->accept(*this);
        if (node.isBooleanOperator())
            return visitBinOperatorBoolean(node);
        if (node.isComparisonOperator())
            return visitBinOperatorComparison(node);

        std::string leftType = node.left.get<Parser::NodeExpression>()->type;
        std::string rightType = node.right.get<Parser::NodeExpression>()->type;

        // Check if left and righ types are compatible
        auto type = resolve_collision(leftType, rightType, "number");
        if (!type.has_value())
            throw different_type_error(node.left, node.right);
        // Check if hint is compatible with left and right types
        auto expressionType = resolve_collision(type.value(), hint, "number");
        if (!expressionType.has_value())
            throw type_error(hint, type.value(), node.left->token.value());

        node.type = expressionType.value();
        lastType = expressionType.value();
    }

    void typeVisitor::visitNode(Parser::Node &node) {}

    void typeVisitor::visitNodeNumber(Parser::NodeNumber &node)
    {
        lastType = resolve_collision("number", hintType, "number").value();
        node.type = lastType;
    }

    void typeVisitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
    {
        if (node.value.has_value())
        {
            hintType = node.type;
            node.value.value()->accept(*this);
            if (node.type != lastType)
                throw type_error(node.type, lastType, node.token.value());
        }
        variables.add(node.name, node.type);
        lastType = "";
    }
    void typeVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
    {
        std::string type = variables.get(node.name).value();
        hintType = type;
        node.value->accept(*this);
        if (type != lastType)
            throw type_error(type, lastType, node.token.value());
    }
    void typeVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node)
    {
    }
    void typeVisitor::visitNodeText(Parser::NodeText &node)
    {
        node.type = variables.get(node.name).value();
        lastType = node.type;
    }
    void typeVisitor::visitNodeReturn(Parser::NodeReturn &node)
    {
        auto function = (*functions.find(currentFunction.value())).second;
        if (function.returnType.has_value())
        {
            hintType = function.returnType.value();
            node.value.value()->accept(*this);
            if (function.returnType.value() != lastType)
                throw type_error(function.returnType.value(), lastType, node.token.value());
        }
        else
        {
            if (node.value.has_value())
                throw type_error("void", lastType, node.token.value());
        }
        lastType = "";
        hintType = "";
    }
    void typeVisitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node)
    {
        if (node.token.value().isBooleanOperator())
        {
            hintType = "bool";
            node.right->accept(*this);
            if (lastType != "bool")
                throw type_error("bool", lastType, node.token.value());
            lastType = "bool";
            return;
        }
        node.right->accept(*this);
        std::string type = lastType;
        std::string hint = hintType;
        auto nodeType = resolve_collision(type, hint, "number");
        if (!nodeType.has_value())
            throw type_error(hint, type, node.token.value());
        lastType = nodeType.value();
        node.type = nodeType.value();
    }
    void typeVisitor::visitNodeFunction(Parser::NodeFunction &node)
    {

        // Register function
        if (functions.find(node.name) == functions.end())
        {
            functions[node.name] = functionType(node.name, node.returnType);
        }

        // Check if function does not differ in return type
        if (functions[node.name].returnType != node.returnType)
            throw type_error(functions[node.name].returnType.value(), node.returnType.value(), node.token.value());

        std::vector<std::string> types;
        variables.enterScope();
        for (auto &arg : node.arguments)
        {
            types.push_back(arg.first);
            variables.add(arg.second, arg.first);
        }
        functions[node.name].add(types);
        currentFunction = node.name;
        if (node.body.has_value())
            node.body.value()->accept(*this);
        variables.exitScope();
        currentFunction = std::nullopt;
    }
    void typeVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        auto function = functions[node.name];
        std::vector<std::string> argumentsTypes;

        // Search for corresponding overloaded function
        for (auto parameter : function.getAllTypes())
        {
            int i = 0;
            bool found = true;
            for (auto &arg : node.arguments)
            {
                
                hintType = parameter[i];
                try
                {
                    arg->accept(*this);
                } catch (type_error e)
                {
                    found = false;
                    break;
                }
            }
            if (found)
            {
                lastType = function.returnType.value_or("void");
                node.type = lastType;
                return;
            }
        }
        throw type_error("function", "no matching function call", node.token.value());
    }
    void typeVisitor::visitNodePragma(Parser::NodePragma &node)
    {
    }

}