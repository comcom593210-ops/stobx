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
#include <llvm/IR/Intrinsics.h>


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

llvm::Type* CodeGen::getTypeFromName(const std::string& typeName) {
    if (typeName == "u1" || typeName == "i1" || typeName == "bool") {
        return builder->getInt1Ty();
    }

    if (typeName == "u8" || typeName == "i8" || typeName == "char") {
        return builder->getInt8Ty();
    }

    if (typeName == "u16" || typeName == "i16") {
        return builder->getInt16Ty();
    }

    if (typeName == "u32" || typeName == "i32") {
        return builder->getInt32Ty();
    }

    if (typeName == "u64" || typeName == "i64") {
        return builder->getInt64Ty();
    }

    if (typeName == "f32") {
        return builder->getFloatTy();
    }

    if (typeName == "f64") {
        return builder->getDoubleTy();
    }

    if (typeName == "string") {
        return builder->getPtrTy();
    }

    throw std::runtime_error(
        "Unknown data type: " + typeName
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
        it->second.allocation->getAllocatedType(),
        it->second.allocation,
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

if (binNode->op == "&" ||
    binNode->op == "|" ||
    binNode->op == "^" ||
    binNode->op == "<<" ||
    binNode->op == ">>") {

    if (!leftVal->getType()->isIntegerTy() ||
        !rightVal->getType()->isIntegerTy()) {
        throw std::runtime_error(
            "Bitwise operators require integer operands"
        );
    }

    if (binNode->op == "&" ||
        binNode->op == "|" ||
        binNode->op == "^") {

        if (leftVal->getType() != rightVal->getType()) {
            throw std::runtime_error(
                "Bitwise operands must have the same integer type"
            );
        }

        if (binNode->op == "&") {
            return builder->CreateAnd(
                leftVal, rightVal, "bitandtmp"
            );
        }

        if (binNode->op == "|") {
            return builder->CreateOr(
                leftVal, rightVal, "bitortmp"
            );
        }

        return builder->CreateXor(
            leftVal, rightVal, "bitxortmp"
        );
    }

    llvm::Value* shiftAmount = builder->CreateZExtOrTrunc(
        rightVal,
        leftVal->getType(),
        "shiftamount"
    );

    if (binNode->op == "<<") {
        return builder->CreateShl(
            leftVal, shiftAmount, "shltmp"
        );
    }

    std::function<std::string(ASTNode*)> getSourceTypeName;

    getSourceTypeName = [&](ASTNode* expression) -> std::string {
        if (auto* varNode =
                dynamic_cast<VariableASTNode*>(expression)) {

            auto it = variables.find(varNode->name);

            if (it == variables.end() ||
                it->second.typeName.empty()) {
                throw std::runtime_error(
                    "Cannot determine signedness of variable: " +
                    varNode->name
                );
            }

            return it->second.typeName;
        }

        if (auto* numberNode =
                dynamic_cast<NumberASTNode*>(expression)) {
            return "i32";
        }

        if (auto* binary =
                dynamic_cast<BinaryASTNode*>(expression)) {
            return getSourceTypeName(binary->left.get());
        }

        if (auto* unary =
                dynamic_cast<UnaryASTNode*>(expression)) {
            return getSourceTypeName(unary->operand.get());
        }

        throw std::runtime_error(
            "Cannot determine source type for right shift"
        );
    };

    std::string sourceType =
        getSourceTypeName(binNode->left.get());

    if (sourceType[0] == 'i') {
        return builder->CreateAShr(
            leftVal, shiftAmount, "ashrtmp"
        );
    }

    if (sourceType[0] == 'u') {
        return builder->CreateLShr(
            leftVal, shiftAmount, "lshrtmp"
        );
    }

    throw std::runtime_error(
        "Right shift requires a signed or unsigned integer type"
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
bool hasUserMain = false;

for (const auto& statement : program->statements) {
    if (auto* functionNode =
            dynamic_cast<FunctionASTNode*>(statement.get())) {

        if (functionNode->name == "main") {
            hasUserMain = true;
            break;
        }
    }
}
if (!hasUserMain) {
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
}

llvm::Function* printfFunction = getPrintfFunction();

llvm::Value* intFormat =
    builder->CreateGlobalStringPtr(
        "%d\n", "intFormat", 0, module.get()
    );

llvm::Value* stringFormat =
    builder->CreateGlobalStringPtr(
        "%s\n", "stringFormat", 0, module.get()
    );

llvm::Value* trueString =
    builder->CreateGlobalStringPtr(
        "true", "trueString", 0, module.get()
    );

llvm::Value* falseString =
    builder->CreateGlobalStringPtr(
        "false", "falseString", 0, module.get()
    );

    std::function<void(ASTNode*)> generateStatement;

    generateStatement = [&](ASTNode* node) {
        if (!node) {
            return;
        }

if (auto* varNode = dynamic_cast<VariableDeclarationASTNode*>(node)) {
    llvm::Type* varType = getTypeFromName(varNode->type);

    llvm::Value* value =
        generateExpression(varNode->value.get());

    if (value->getType() != varType) {
        if (value->getType()->isIntegerTy() &&
            varType->isIntegerTy()) {
            bool isSigned = varNode->type[0] == 'i';

            value = builder->CreateIntCast(
                value,
                varType,
                isSigned,
                "casttmp"
            );
        } else {
            throw std::runtime_error(
                "Variable initialization type mismatch: " +
                varNode->name
            );
        }
    }

    if (variables.count(varNode->name)) {
        throw std::runtime_error(
            "Variable already declared: " + varNode->name
        );
    }

    llvm::AllocaInst* alloca =
        builder->CreateAlloca(varType, nullptr, varNode->name);

    builder->CreateStore(value, alloca);

    variables[varNode->name] ={ alloca,
    varNode->type
    };
    return;
}

if (auto* returnNode = dynamic_cast<ReturnASTNode*>(node)) {
    llvm::Value* returnValue =
        generateExpression(returnNode->value.get());

    llvm::Function* currentFunction =
        builder->GetInsertBlock()->getParent();

    if (returnValue->getType() != currentFunction->getReturnType()) {
        throw std::runtime_error(
            "Return type does not match function return type"
        );
    }

    builder->CreateRet(returnValue);

    return;
}

if (auto* functionNode = dynamic_cast<FunctionASTNode*>(node)) {
    if (module->getFunction(functionNode->name)) {
        throw std::runtime_error(
            "Function already declared: " + functionNode->name
        );
    }

    llvm::Type* returnType =
        getTypeFromName(functionNode->returnType);

    std::vector<llvm::Type*> parameterTypes;

    for (const auto& parameter : functionNode->parameters) {
        parameterTypes.push_back(
            getTypeFromName(parameter->type)
        );
    }

    llvm::FunctionType* functionType =
        llvm::FunctionType::get(
            returnType,
            parameterTypes,
            false
        );

    llvm::Function* function =
        llvm::Function::Create(
            functionType,
            llvm::Function::ExternalLinkage,
            functionNode->name,
            module.get()
        );

    auto savedInsertPoint = builder->saveIP();
    auto savedVariables = variables;

    variables.clear();

    llvm::BasicBlock* entry =
        llvm::BasicBlock::Create(
            *context,
            "entry",
            function
        );

    builder->SetInsertPoint(entry);

    std::size_t index = 0;

    for (auto& argument : function->args()) {
        const auto& parameter =
            functionNode->parameters[index];

        argument.setName(parameter->name);

        llvm::AllocaInst* alloca =
            builder->CreateAlloca(
                argument.getType(),
                nullptr,
                parameter->name
            );

        builder->CreateStore(&argument, alloca);

        variables[parameter->name] = { alloca,
parameter->type
        };

        ++index;
    }

    for (const auto& statement : functionNode->statements) {
        generateStatement(statement.get());
    }

    if (!builder->GetInsertBlock()->getTerminator()) {
        throw std::runtime_error(
            "Function is missing a return statement: " +
            functionNode->name
        );
    }

    variables = std::move(savedVariables);

if (functionNode->name != "main") {
    builder->restoreIP(savedInsertPoint);
}

return;
}

if (auto* assertNode = dynamic_cast<AssertASTNode*>(node)) {
    llvm::Value* condition =
        generateExpression(assertNode->condition.get());

    if (!condition->getType()->isIntegerTy(1)) {
        throw std::runtime_error(
            "Assert condition must be a boolean expression"
        );
    }

    llvm::Function* currentFunction =
        builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* successBlock =
        llvm::BasicBlock::Create(
            *context,
            "assert.success",
            currentFunction
        );

    llvm::BasicBlock* failureBlock =
        llvm::BasicBlock::Create(
            *context,
            "assert.failure",
            currentFunction
        );

    builder->CreateCondBr(
        condition,
        successBlock,
        failureBlock
    );

    builder->SetInsertPoint(failureBlock);

    llvm::Function* trapFunction =
        llvm::Intrinsic::getDeclaration(
            module.get(),
            llvm::Intrinsic::trap
        );

    builder->CreateCall(trapFunction);
    builder->CreateUnreachable();

    builder->SetInsertPoint(successBlock);

    return;
}

if (auto* assumeNode = dynamic_cast<AssumeASTNode*>(node)) {
    llvm::Value* condition =
        generateExpression(assumeNode->condition.get());

    if (!condition->getType()->isIntegerTy(1)) {
        throw std::runtime_error(
            "Assume condition must be a boolean expression"
        );
    }

    return;
}

if (auto* contractNode = dynamic_cast<ContractASTNode*>(node)) {
    llvm::Value* condition =
        generateExpression(contractNode->condition.get());

    if (!condition->getType()->isIntegerTy(1)) {
        throw std::runtime_error(
            "Contract condition must be a boolean expression"
        );
    }

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

    variables[assignmentNode->name] = {
        variable,
        ""
    };
} else {
    variable = it->second.allocation;

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
