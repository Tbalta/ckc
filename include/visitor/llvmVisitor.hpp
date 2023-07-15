#pragma once

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include <llvm/IR/LLVMContext.h>

#include "../parser.hpp"
#include "../contextProvider.hpp"

using namespace llvm;
namespace visitor
{
    void LogError(const char *Str)
    {
        fprintf(stderr, "Error: %s\n", Str);
    }

    class llvmVisitor : public Parser::Visitor
    {
    private:
        std::shared_ptr<LLVMContext> context;
        std::shared_ptr<IRBuilder<>> Builder;
        std::shared_ptr<Module> TheModule;
        std::string currentType;
        Context::ContextProvider &contextProvider;

    public:
        ~llvmVisitor() = default;
        Value *lastValue;
        llvmVisitor(std::shared_ptr<LLVMContext> context, std::shared_ptr<IRBuilder<>> Builder, std::shared_ptr<Module> module, Context::ContextProvider &contextProvider) : context(context), Builder(Builder), TheModule(module), contextProvider(contextProvider){};
        virtual void visitNodeIf(Parser::NodeIf &node)
        {
            if (node.modifier.has_value())
            {
                node.modifier.value()->accept(*this);
            }
            node.condition->accept(*this);
            Value *condV = lastValue;
            BasicBlock *thenBB = BasicBlock::Create(*context, "then", Builder->GetInsertBlock()->getParent());
            BasicBlock *elseBB = BasicBlock::Create(*context, "else");
            BasicBlock *mergeBB = BasicBlock::Create(*context, "ifcont");
            Builder->CreateCondBr(condV, thenBB, elseBB);
            Builder->SetInsertPoint(thenBB);
            node.thenStatement->accept(*this);
            Builder->CreateBr(mergeBB);
            Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(elseBB);
            Builder->SetInsertPoint(elseBB);
            if (node.elseStatement.has_value())
            {
                node.elseStatement.value()->accept(*this);
            }

            Builder->CreateBr(mergeBB);
            Builder->GetInsertBlock()->getParent()->getBasicBlockList().push_back(mergeBB);
            Builder->SetInsertPoint(mergeBB);
        }
        virtual void visitNodeGoto(Parser::NodeGoto &node)
        {
            auto block = contextProvider.getBasicBlock(node.label);
            if (block == nullptr)
            {
                block = BasicBlock::Create(*context, node.label);
                contextProvider.addBasicBlock(node.label, block);
            }
            Builder->CreateBr(block);
        }
        virtual void visitBinOperator(Parser::NodeBinOperator &node)
        {
            if (node.isLazyOperator())
                return visitLazyBinOperator(node);
            node.left->accept(*this);
            Value *left = lastValue;
            node.right->accept(*this);
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

        virtual void visitLazyBinOperator(Parser::NodeBinOperator &node)
        {
            switch (node.op)
            {
            case Lexer::TokenType::LOGICAL_AND:
            {
                // Get left value
                node.left->accept(*this);
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
                node.right->accept(*this);
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
                node.left->accept(*this);
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
                node.right->accept(*this);
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


        virtual void visitNode(Parser::Node &node){};
        virtual void visitNodeNumber(Parser::NodeNumber &node)
        {
            std::map<std::string, std::function<Value *()>> typeMap{
                {"uint8", [&]()
                 { return ConstantInt::get(*context, APInt(8, node.value, false)); }},
                {"uint16", [&]()
                 { return ConstantInt::get(*context, APInt(16, node.value, false)); }},
                {"uint32", [&]()
                 { return ConstantInt::get(*context, APInt(32, node.value, false)); }},
                {"uint64", [&]()
                 { return ConstantInt::get(*context, APInt(64, node.value, false)); }},
                {"int8", [&]()
                 { return ConstantInt::get(*context, APInt(8, node.value, true)); }},
                {"int16", [&]()
                 { return ConstantInt::get(*context, APInt(16, node.value, true)); }},
                {"int32", [&]()
                 { return ConstantInt::get(*context, APInt(32, node.value, true)); }},
                {"int64", [&]()
                 { return ConstantInt::get(*context, APInt(64, node.value, true)); }},
                {"", [&]()
                 { return ConstantInt::get(*context, APInt(32, node.value, false)); }}};
            lastValue = typeMap[currentType]();
        };
        virtual void visitNodeVariableDeclaration(Parser::NodeVariableDeclaration &node)
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
                {"int64", Type::getInt64Ty(*context)}};
            //
            // auto *block = Builder->GetInsertBlock();
            auto *alloca = Builder->CreateAlloca(typeMap[node.type], 0, node.name);
            contextProvider.addVariable(node.name, alloca, node.type);
            if (!node.value.has_value())
                return;
            node.value.value()->accept(*this);
            Builder->CreateStore(lastValue, alloca);
        };
        virtual void visitNodeVariableAssignment(Parser::NodeVariableAssignment &node)
        {
            auto variable = contextProvider.getVariable(node.name);
            if (variable.value == nullptr)
            {
                LogError("Unknown variable name");
                return;
            }
            currentType = variable.type;
            node.value->accept(*this);
            Builder->CreateStore(lastValue, variable.value);
        }
        virtual void visitNodeBlockModifier(Parser::NodeBlockModifier &node)
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
        virtual void visitNodeIdentifier(Parser::NodeIdentifier &node)
        {

            auto alloca = contextProvider.getVariable(node.name).value;
            if (alloca == nullptr)
            {
                LogError("Unknown variable name");
                return;
            }
            lastValue = Builder->CreateLoad(alloca->getAllocatedType(), alloca, node.name.c_str());
        }

        virtual void visitNodeReturn(Parser::NodeReturn &node)
        {
            node.value->accept(*this);
            Builder->CreateRet(lastValue);
        }

        virtual void visitNodeUnaryOperator(Parser::NodeUnaryOperator &node)
        {
            node.right->accept(*this);
            switch (node.op)
            {
            case Lexer::TokenType::OPERATOR_NOT:
                lastValue = Builder->CreateNot(lastValue, "nottmp");
                break;
            case Lexer::TokenType::LOGICAL_NOT:
            {
                auto constantZero = ConstantInt::get(lastValue->getType(), 0);
                lastValue = Builder->CreateICmpEQ(lastValue, constantZero , "nottmp");
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
    };

}