#include "CodeGen.h"
#include "Parser.h"

#include <stdexcept>
#include <string>
#include <system_error>
#include <functional>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>

CodeGen::CodeGen() {
    context = std::make_unique<llvm::LLVMContext>();

    module = std::make_unique<llvm::Module>(
        "StobxModule",
        *context
    );

    builder = std::make_unique<llvm::IRBuilder<>>(
        *context
    );
}

llvm::Function* CodeGen::getPrintfFunction() {
    llvm::Function* printfFunc =
        module->getFunction("printf");

    if (!printfFunc) {
        llvm::FunctionType* printfType =
            llvm::FunctionType::get(
                builder->getInt32Ty(),
                builder->getPtrTy(),
                true
            );

        printfFunc = llvm::Function::Create(
            printfType,
            llvm::Function::ExternalLinkage,
            "printf",
            module.get()
        );
    }

    return printfFunc;
}

llvm::Value* CodeGen::generateExpression(ASTNode* node) {
    if (!node) {
        throw std::runtime_error("Invalid expression");
    }

    if (auto* numNode = dynamic_cast<NumberASTNode*>(node)) {
        return builder->getInt32(
            std::stoi(numNode->value)
        );
    }

    if (auto* strNode = dynamic_cast<StringASTNode*>(node)) {
        return builder->CreateGlobalStringPtr(
            strNode->value
        );
    }

    if (auto* varNode = dynamic_cast<VariableASTNode*>(node)) {
        auto it = variables.find(varNode->name);

        if (it == variables.end()) {
            throw std::runtime_error(
                "Unknown variable: " + varNode->name
            );
        }

        return builder->CreateLoad(
            it->second->getAllocatedType(),
            it->second,
            varNode->name
        );
    }

    if (auto* unaryNode = dynamic_cast<UnaryASTNode*>(node)) {
        llvm::Value* operand =
            generateExpression(unaryNode->operand.get());

        if (unaryNode->op == "not") {
            if (!operand->getType()->isIntegerTy(1)) {
                throw std::runtime_error(
                    "'not' requires a comparison or logical expression"
                );
            }

            return builder->CreateNot(operand, "nottmp");
        }

        throw std::runtime_error(
            "Unknown unary operator: " + unaryNode->op
        );
    }

    if (auto* binNode = dynamic_cast<BinaryASTNode*>(node)) {
        llvm::Value* leftVal =
            generateExpression(binNode->left.get());

        llvm::Value* rightVal =
            generateExpression(binNode->right.get());

        if (binNode->op == "+") {
            return builder->CreateAdd(
                leftVal, rightVal, "addtmp"
            );
        }

        if (binNode->op == "-") {
            return builder->CreateSub(
                leftVal, rightVal, "subtmp"
            );
        }

        if (binNode->op == "*") {
            return builder->CreateMul(
                leftVal, rightVal, "multmp"
            );
        }

        if (binNode->op == "/") {
            return builder->CreateSDiv(
                leftVal, rightVal, "divtmp"
            );
        }

        if (binNode->op == ">") {
            return builder->CreateICmpSGT(
                leftVal, rightVal, "cmptmp"
            );
        }

        if (binNode->op == "<") {
            return builder->CreateICmpSLT(
                leftVal, rightVal, "cmptmp"
            );
        }

        if (binNode->op == ">=") {
            return builder->CreateICmpSGE(
                leftVal, rightVal, "cmptmp"
            );
        }

        if (binNode->op == "<=") {
            return builder->CreateICmpSLE(
                leftVal, rightVal, "cmptmp"
            );
        }

        if (binNode->op == "==") {
            return builder->CreateICmpEQ(
                leftVal, rightVal, "cmptmp"
            );
        }

        if (binNode->op == "!=") {
            return builder->CreateICmpNE(
                leftVal, rightVal, "cmptmp"
            );
        }

        if (binNode->op == "and") {
            if (!leftVal->getType()->isIntegerTy(1) ||
                !rightVal->getType()->isIntegerTy(1)) {
                throw std::runtime_error(
                    "'and' requires comparison or logical expressions"
                );
            }

            return builder->CreateAnd(
                leftVal, rightVal, "andtmp"
            );
        }

        if (binNode->op == "or") {
            if (!leftVal->getType()->isIntegerTy(1) ||
                !rightVal->getType()->isIntegerTy(1)) {
                throw std::runtime_error(
                    "'or' requires comparison or logical expressions"
                );
            }

            return builder->CreateOr(
                leftVal, rightVal, "ortmp"
            );
        }

        throw std::runtime_error(
            "Unknown binary operator: " + binNode->op
        );
    }

    throw std::runtime_error("Unknown expression node");
}

void CodeGen::generate(ProgramASTNode* program) {
    if (!program) {
        throw std::runtime_error("Program is empty");
    }

    if (module->getFunction("main")) {
        throw std::runtime_error(
            "Code generation has already been performed"
        );
    }

    variables.clear();

    llvm::FunctionType* mainType =
        llvm::FunctionType::get(
            builder->getInt32Ty(),
            false
        );

    llvm::Function* mainFunction =
        llvm::Function::Create(
            mainType,
            llvm::Function::ExternalLinkage,
            "main",
            module.get()
        );

    llvm::BasicBlock* entryBlock =
        llvm::BasicBlock::Create(
            *context,
            "entry",
            mainFunction
        );

    builder->SetInsertPoint(entryBlock);

    llvm::Function* printfFunction = getPrintfFunction();

    llvm::Value* intFormat =
        builder->CreateGlobalStringPtr("%d\n", "intFormat");

    llvm::Value* stringFormat =
        builder->CreateGlobalStringPtr("%s\n", "stringFormat");

    llvm::Value* trueString =
        builder->CreateGlobalStringPtr("true", "trueString");

    llvm::Value* falseString =
        builder->CreateGlobalStringPtr("false", "falseString");

    std::function<void(ASTNode*)> generateStatement;

    generateStatement = [&](ASTNode* node) {
        if (!node) {
            return;
        }

        if (auto* assignmentNode =
                dynamic_cast<AssignmentASTNode*>(node)) {

            llvm::Value* value =
                generateExpression(assignmentNode->value.get());

            auto it = variables.find(assignmentNode->name);

            llvm::AllocaInst* variable = nullptr;

            if (it == variables.end()) {
                variable = builder->CreateAlloca(
                    value->getType(),
                    nullptr,
                    assignmentNode->name
                );

                variables[assignmentNode->name] = variable;
            } else {
                variable = it->second;

                if (variable->getAllocatedType() != value->getType()) {
                    throw std::runtime_error(
                        "Cannot assign different type to variable: " +
                        assignmentNode->name
                    );
                }
            }

            builder->CreateStore(value, variable);
            return;
        }

        if (auto* printNode =
                dynamic_cast<PrintASTNode*>(node)) {

            ASTNode* expression = printNode->expression.get();

            llvm::Value* value =
                generateExpression(expression);

            if (dynamic_cast<StringASTNode*>(expression) ||
                value->getType()->isPointerTy()) {

                builder->CreateCall(
                    printfFunction,
                    {stringFormat, value}
                );
            } else if (value->getType()->isIntegerTy(1)) {
                llvm::Value* selectedString =
                    builder->CreateSelect(
                        value,
                        trueString,
                        falseString,
                        "boolString"
                    );

                builder->CreateCall(
                    printfFunction,
                    {stringFormat, selectedString}
                );
            } else {
                builder->CreateCall(
                    printfFunction,
                    {intFormat, value}
                );
            }

            return;
        }

        if (auto* whileNode =
                dynamic_cast<WhileASTNode*>(node)) {

            llvm::Function* function =
                builder->GetInsertBlock()->getParent();

            llvm::BasicBlock* conditionBlock =
                llvm::BasicBlock::Create(
                    *context,
                    "while.condition",
                    function
                );

            llvm::BasicBlock* bodyBlock =
                llvm::BasicBlock::Create(
                    *context,
                    "while.body",
                    function
                );

            llvm::BasicBlock* endBlock =
                llvm::BasicBlock::Create(
                    *context,
                    "while.end",
                    function
                );

            builder->CreateBr(conditionBlock);

            builder->SetInsertPoint(conditionBlock);

            llvm::Value* condition =
                generateExpression(whileNode->condition.get());

            if (!condition->getType()->isIntegerTy(1)) {
                throw std::runtime_error(
                    "The while condition must be a comparison or logical expression"
                );
            }

            builder->CreateCondBr(
                condition,
                bodyBlock,
                endBlock
            );

            builder->SetInsertPoint(bodyBlock);

            for (const auto& statement : whileNode->statements) {
                generateStatement(statement.get());
            }

            if (!builder->GetInsertBlock()->getTerminator()) {
                builder->CreateBr(conditionBlock);
            }

            builder->SetInsertPoint(endBlock);
            return;
        }

        if (auto* ifNode = dynamic_cast<IfASTNode*>(node)) {
            llvm::Value* condition =
                generateExpression(ifNode->condition.get());

            if (!condition->getType()->isIntegerTy(1)) {
                throw std::runtime_error(
                    "The if condition must be a comparison or logical expression"
                );
            }

            llvm::Function* function =
                builder->GetInsertBlock()->getParent();

            llvm::BasicBlock* thenBlock =
                llvm::BasicBlock::Create(
                    *context,
                    "if.then",
                    function
                );

            llvm::BasicBlock* elseBlock =
                llvm::BasicBlock::Create(
                    *context,
                    "if.else",
                    function
                );

            llvm::BasicBlock* mergeBlock =
                llvm::BasicBlock::Create(
                    *context,
                    "if.end",
                    function
                );

            builder->CreateCondBr(
                condition,
                thenBlock,
                elseBlock
            );

            builder->SetInsertPoint(thenBlock);

            for (const auto& statement : ifNode->statements) {
                generateStatement(statement.get());
            }

            if (!builder->GetInsertBlock()->getTerminator()) {
                builder->CreateBr(mergeBlock);
            }

            builder->SetInsertPoint(elseBlock);

            for (const auto& statement : ifNode->elseStatements) {
                generateStatement(statement.get());
            }

            if (!builder->GetInsertBlock()->getTerminator()) {
                builder->CreateBr(mergeBlock);
            }

            builder->SetInsertPoint(mergeBlock);
            return;
        }

        throw std::runtime_error("Unknown statement node");
    };

    for (const auto& statement : program->statements) {
        generateStatement(statement.get());
    }

    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateRet(
            builder->getInt32(0)
        );
    }
}

void CodeGen::printIR() {
    std::error_code error;

    llvm::raw_fd_ostream file(
        "output.ll",
        error
    );

    if (error) {
        throw std::runtime_error(
            "Could not open output.ll: " + error.message()
        );
    }

    module->print(file, nullptr);
}