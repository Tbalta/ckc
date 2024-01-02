#include "visitor/llvmVisitor.hpp"
#include <set>
using namespace llvm;
namespace visitor
{

    void LogError(const char *Str)
    {
        fprintf(stderr, "Error: %s\n", Str);
    }

    void llvmVisitor::visitNodeIf(Parser::NodeIf &node)
    {
        node.condition.get()->accept(*this);
        Value *condV = lastValue;
        BasicBlock *thenBB = BasicBlock::Create(*context, "then", Builder->GetInsertBlock()->getParent());
        BasicBlock *elseBB = BasicBlock::Create(*context, "else");
        BasicBlock *mergeBB = BasicBlock::Create(*context, "ifcont");
        Builder->CreateCondBr(condV, thenBB, elseBB);
        Builder->SetInsertPoint(thenBB);
        enterBlock();
        node.thenStatement.get()->accept(*this);
        exitBlock();
        Builder->CreateBr(mergeBB);
        Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(elseBB);
        Builder->SetInsertPoint(elseBB);
        if (node.elseStatement.has_value())
        {
            enterBlock();
            node.elseStatement.value().get()->accept(*this);
            exitBlock();
        }

        Builder->CreateBr(mergeBB);
        Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(mergeBB);
        Builder->SetInsertPoint(mergeBB);
    }

    void llvmVisitor::visitNodeGoto(Parser::NodeGoto &node)
    {
        auto block = contextProvider.getBasicBlock(node.label);
        if (block == nullptr)
        {
            block = BasicBlock::Create(*context, node.label);
            contextProvider.addBasicBlock(node.label, block);
        }
        Builder->CreateBr(block);
    }
    void llvmVisitor::visitBinOperator(Parser::NodeBinOperator &node)
    {
        if (node.isLazyOperator())
            return visitLazyBinOperator(node);
        node.left.get()->accept(*this);
        Value *left = lastValue;
        node.right.get()->accept(*this);
        Value *right = lastValue;
        lastValue = nullptr;
        switch (node.op)
        {
        case Lexer::TokenType::OPERATOR_ADD:
            lastValue = Builder->CreateAdd(left, right, "addtmp");
            break;
        case Lexer::TokenType::OPERATOR_SUB:
            lastValue = Builder->CreateSub(left, right, "subtmp");
            break;
        case Lexer::TokenType::OPERATOR_MUL:
            lastValue = Builder->CreateMul(left, right, "multmp");
            break;
        case Lexer::TokenType::OPERATOR_DIV:
            lastValue = Builder->CreateSDiv(left, right, "divtmp");
            break;
        case Lexer::TokenType::OPERATOR_MOD:
            lastValue = Builder->CreateSRem(left, right, "modtmp");
            break;
        case Lexer::TokenType::OPERATOR_EQ:
            lastValue = Builder->CreateICmpEQ(left, right, "eqtmp");
            break;
        case Lexer::TokenType::OPERATOR_NE:
            lastValue = Builder->CreateICmpNE(left, right, "netmp");
            break;
        case Lexer::TokenType::OPERATOR_LT:
            lastValue = Builder->CreateICmpSLT(left, right, "lttmp");
            break;
        case Lexer::TokenType::OPERATOR_GT:
            lastValue = Builder->CreateICmpSGT(left, right, "gttmp");
            break;
        case Lexer::TokenType::OPERATOR_LE:
            lastValue = Builder->CreateICmpSLE(left, right, "letmp");
            break;
        case Lexer::TokenType::OPERATOR_GE:
            lastValue = Builder->CreateICmpSGE(left, right, "getmp");
            break;
        case Lexer::TokenType::BINARY_AND:
            lastValue = Builder->CreateAnd(left, right, "andtmp");
            break;
        case Lexer::TokenType::BINARY_OR:
            lastValue = Builder->CreateOr(left, right, "ortmp");
            break;
        case Lexer::TokenType::BINARY_RSHIFT:
            lastValue = Builder->CreateAShr(left, right, "rshifttmp");
            break;
        case Lexer::TokenType::OPERATOR_LSHIFT:
            lastValue = Builder->CreateShl(left, right, "lshifttmp");
            break;

        default:
            LogError("Unknown binary operator");
            break;
        }
    }

    void llvmVisitor::visitLazyBinOperator(Parser::NodeBinOperator &node)
    {
        switch (node.op)
        {
        case Lexer::TokenType::LOGICAL_AND:
        {
            // Get left value
            node.left.get()->accept(*this);
            Value *left = Builder->CreateICmpNE(lastValue, ConstantInt::get(*context, APInt(1, 0, false)), "lefttmp");
            // Create block for right value
            BasicBlock *leftBlock = BasicBlock::Create(*context, "left", Builder->GetInsertBlock()->getParent());
            BasicBlock *rightBlock = BasicBlock::Create(*context, "right");
            BasicBlock *mergeBlock = BasicBlock::Create(*context, "merge");

            Builder->CreateCondBr(left, rightBlock, leftBlock);
            Builder->SetInsertPoint(leftBlock);
            Builder->CreateBr(mergeBlock);

            // Create block for right value
            Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(rightBlock);
            Builder->SetInsertPoint(rightBlock);
            node.right.get()->accept(*this);
            Value *right = Builder->CreateICmpNE(lastValue, ConstantInt::get(*context, APInt(1, 0, false)), "righttmp");
            Builder->CreateBr(mergeBlock);
            Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(mergeBlock);
            Builder->SetInsertPoint(mergeBlock);

            PHINode *PN = Builder->CreatePHI(Type::getInt1Ty(*context), 2, "iftmp");
            PN->addIncoming(left, leftBlock);
            PN->addIncoming(right, rightBlock);
            lastValue = PN;
        }
        break;
        case Lexer::TokenType::LOGICAL_OR:
        {
            // Get left value
            node.left.get()->accept(*this);
            Value *left = Builder->CreateICmpNE(lastValue, ConstantInt::get(*context, APInt(1, 0, false)), "lefttmp");
            // Create block for right value
            BasicBlock *leftBlock = BasicBlock::Create(*context, "left", Builder->GetInsertBlock()->getParent());
            BasicBlock *rightBlock = BasicBlock::Create(*context, "right");
            BasicBlock *mergeBlock = BasicBlock::Create(*context, "merge");

            Builder->CreateCondBr(left, leftBlock, rightBlock);
            Builder->SetInsertPoint(leftBlock);
            Builder->CreateBr(mergeBlock);

            // Create block for right value
            Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(rightBlock);
            Builder->SetInsertPoint(rightBlock);
            node.right.get()->accept(*this);
            Value *right = Builder->CreateICmpNE(lastValue, ConstantInt::get(*context, APInt(1, 0, false)), "righttmp");
            Builder->CreateBr(mergeBlock);
            Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(mergeBlock);
            Builder->SetInsertPoint(mergeBlock);

            PHINode *PN = Builder->CreatePHI(Type::getInt1Ty(*context), 2, "iftmp");
            PN->addIncoming(left, leftBlock);
            PN->addIncoming(right, rightBlock);
            lastValue = PN;
        }
        break;
        default:
            LogError("Unknown binary operator");
            break;
        }
    }

    void llvmVisitor::visitNode(Parser::Node &node){};
    void llvmVisitor::visitNodeNumber(Parser::NodeNumber &node)
    {
        lastValue = typeWithInitialValueContext.get(currentType).value()(node.value);
    };
    void llvmVisitor::visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
    {
        currentType = node.type;
        std::map<std::string, llvm::Type *> typeMap{
            {"uint8", Type::getInt8Ty(*context)},
            {"uint16", Type::getInt16Ty(*context)},
            {"uint32", Type::getInt32Ty(*context)},
            {"uint64", Type::getInt64Ty(*context)},
            {"int8", Type::getInt8Ty(*context)},
            {"int16", Type::getInt16Ty(*context)},
            {"int32", Type::getInt32Ty(*context)},
            {"int64", Type::getInt64Ty(*context)},
            {"bool", Type::getInt1Ty(*context)}};
        //
        // auto *block = Builder->GetInsertBlock();
        auto *alloca = Builder->CreateAlloca(typeMap[node.type], 0, node.name);
        contextProvider.addVariable(node.name, alloca, node.type);
        if (!node.value.has_value())
            return;
        node.value.value().get()->accept(*this);
        Builder->CreateStore(lastValue, alloca);
    };
    void llvmVisitor::visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
    {
        auto variable = contextProvider.getVariable(node.name);
        if (variable.value == nullptr)
        {
            LogError("Unknown variable name");
            return;
        }
        currentType = variable.type;
        node.value.get()->accept(*this);
        Builder->CreateStore(lastValue, variable.value);
    }
    void llvmVisitor::visitNodeBlockModifier(Parser::NodeBlockModifier &node)
    {
        switch (node.modifier_type)
        {
        case Lexer::ModifierType::Named:
        {
            BasicBlock *block = contextProvider.getBasicBlock(node.modifier_value);
            if (block == nullptr)
            {
                block = BasicBlock::Create(*context, node.modifier_value);
                contextProvider.addBasicBlock(node.modifier_value, block);
            }
            Builder->CreateBr(block);
            Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(block);
            Builder->SetInsertPoint(block);
        }
        break;
        }
    }
    void llvmVisitor::visitNodeText(Parser::NodeText &node)
    {

        auto alloca = contextProvider.getVariable(node.name).value;
        if (alloca == nullptr)
        {
            LogError("Unknown variable name");
            return;
        }
        lastValue = Builder->CreateLoad(alloca->getAllocatedType(), alloca, node.name.c_str());
    }

    void llvmVisitor::visitNodeReturn(Parser::NodeReturn &node)
    {
        if (!node.value.has_value())
        {
            Builder->CreateRetVoid();
            return;
        }
        currentType = node.value.value().get<Parser::NodeExpression>()->type;
        node.value.value()->accept(*this);
        Builder->CreateRet(lastValue);
    }

    void llvmVisitor::visitNodeUnaryOperator(Parser::NodeUnaryOperator &node)
    {
        node.right.get()->accept(*this);
        switch (node.op)
        {
        case Lexer::TokenType::OPERATOR_NOT:
            lastValue = Builder->CreateNot(lastValue, "nottmp");
            break;
        case Lexer::TokenType::LOGICAL_NOT:
        {
            auto constantZero = ConstantInt::get(lastValue->getType(), 0);
            lastValue = Builder->CreateICmpEQ(lastValue, constantZero, "nottmp");
        }
        break;
        case Lexer::TokenType::OPERATOR_SUB:
            lastValue = Builder->CreateNeg(lastValue, "negtmp");
            break;
        default:
            LogError("Unknown unary operator");
            break;
        }
    }

    void llvmVisitor::visitNodeFunction(Parser::NodeFunction &node)
    {
        lastValue = nullptr;
        std::map<std::string, std::function<llvm::Type *()>> typeMap{
            {"uint8", [&]()
             { return llvm::Type::getInt8Ty(*context); }},
            {"uint16", [&]()
             { return llvm::Type::getInt16Ty(*context); }},
            {"uint32", [&]()
             { return llvm::Type::getInt32Ty(*context); }},
            {"uint64", [&]()
             { return llvm::Type::getInt64Ty(*context); }},
            {"int8", [&]()
             { return llvm::Type::getInt8Ty(*context); }},
            {"int16", [&]()
             { return llvm::Type::getInt16Ty(*context); }},
            {"int32", [&]()
             { return llvm::Type::getInt32Ty(*context); }},
            {"int64", [&]()
             { return llvm::Type::getInt64Ty(*context); }},
            {"void", [&]()
             { return llvm::Type::getVoidTy(*context); }},
            {"bool", [&]()
             { return llvm::Type::getInt1Ty(*context); }}};
        std::vector<Type *> args;
        for (auto &arg : node.arguments)
        {
            args.push_back(typeMap[arg.first]());
        }
        auto funcType = FunctionType::get(typeMap[node.returnType.value_or("void")](), args, false);
        auto Function = Function::Create(funcType, Function::ExternalLinkage, node.symbol_name.value(), TheModule.get());
        // contextProvider.addNameTranslation(node.name, node.symbol_name);
        // Set names for all arguments.
        int i = 0;
        for (auto &arg : Function->args())
        {
            arg.setName(node.arguments[i].second);
            i++;
        }
        if (!node.body.has_value())
            return;
        // Create a new basic block to start insertion into.
        BasicBlock *BB = BasicBlock::Create(*context, "entry", Function);
        Builder->SetInsertPoint(BB);

        // Create allocas for all arguments
        contextProvider.enterScope();
        i = 0;
        for (auto &arg : Function->args())
        {
            auto *alloca = Builder->CreateAlloca(arg.getType(), 0, arg.getName());
            Builder->CreateStore(&arg, alloca);
            contextProvider.addVariable(arg.getName(), alloca, node.arguments[i].first);
            i++;
        }
        node.body.value().get()->accept(*this);
        contextProvider.exitScope();
        if (!node.returnType.has_value())
        {
            Builder->CreateRetVoid();
        }
        // No code should reach this point.
        Builder->CreateUnreachable();
    }
    void llvmVisitor::visitNodeFunctionCall(Parser::NodeFunctionCall &node)
    {
        auto callee = TheModule->getFunction(node.symbol_name.value());
        if (callee == nullptr)
        {
            LogError("Unknown function referenced");
            return;
        }
        std::vector<Value *> args;
        for (auto &arg : node.arguments)
        {
            arg.get()->accept(*this);
            args.push_back(lastValue);
        }
        lastValue = Builder->CreateCall(callee, args, "calltmp");
    }

    void llvmVisitor::visitNodePragma(Parser::NodePragma &node)
    {
    }

    void llvmVisitor::visitNodeCast(Parser::NodeCast &node)
    {
        currentType = node.type;
        node.value->accept(*this);
        llvm::Type *type = typeNameContext.get(node.type).value()();

        std::set<std::string> signedType{"int8", "int16", "int32", "int64"};
        if (signedType.find(node.type) != signedType.end())
            lastValue = Builder->CreateCast(Instruction::CastOps::SExt, lastValue, type);
        else
            lastValue = Builder->CreateCast(Instruction::CastOps::Trunc, lastValue, type);

    }
    void llvmVisitor::visitNodePartial(Parser::NodePartial &node) {};


}