
#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "Diagnostic.h"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class NumberASTNode : public ASTNode {
public:
    std::string value;

    explicit NumberASTNode(std::string value)
        : value(std::move(value)) {}
};

class StringASTNode : public ASTNode {
public:
    std::string value;

    explicit StringASTNode(std::string value)
        : value(std::move(value)) {}
};

class VariableASTNode : public ASTNode {
public:
    std::string name;

    explicit VariableASTNode(std::string name)
        : name(std::move(name)) {}
};

class UnaryASTNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> operand;

    UnaryASTNode(
        std::string op,
        std::unique_ptr<ASTNode> operand
    )
        : op(std::move(op)),
          operand(std::move(operand)) {}
};

class BinaryASTNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryASTNode(
        std::string op,
        std::unique_ptr<ASTNode> left,
        std::unique_ptr<ASTNode> right
    )
        : op(std::move(op)),
          left(std::move(left)),
          right(std::move(right)) {}
};

class PrintASTNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    explicit PrintASTNode(
        std::unique_ptr<ASTNode> expression
    )
        : expression(std::move(expression)) {}
};

class AssignmentASTNode : public ASTNode {
public:
    std::string name;
    std::unique_ptr<ASTNode> value;

    AssignmentASTNode(
        std::string name,
        std::unique_ptr<ASTNode> value
    )
        : name(std::move(name)),
          value(std::move(value)) {}
};

class ProgramASTNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
};

class IfASTNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> statements;
    std::vector<std::unique_ptr<ASTNode>> elseStatements;

    IfASTNode(
        std::unique_ptr<ASTNode> condition,
        std::vector<std::unique_ptr<ASTNode>> statements,
        std::vector<std::unique_ptr<ASTNode>> elseStatements
    )
        : condition(std::move(condition)),
          statements(std::move(statements)),
          elseStatements(std::move(elseStatements)) {}
};

class WhileASTNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> statements;

    WhileASTNode(
        std::unique_ptr<ASTNode> condition,
        std::vector<std::unique_ptr<ASTNode>> statements
    )
        : condition(std::move(condition)),
          statements(std::move(statements)) {}
};

class ParameterASTNode : public ASTNode {
public:
    std::string name;
    std::string type;

    ParameterASTNode(std::string name, std::string type)
        : name(std::move(name)),
          type(std::move(type)) {}
};

class FunctionASTNode : public ASTNode {
public:
    std::string name;
    std::string returnType;

    std::vector<std::unique_ptr<ParameterASTNode>> parameters;
    std::vector<std::unique_ptr<ASTNode>> statements;

    FunctionASTNode(
        std::string name,
        std::string returnType,
        std::vector<std::unique_ptr<ParameterASTNode>> parameters,
        std::vector<std::unique_ptr<ASTNode>> statements
    )
        : name(std::move(name)),
          returnType(std::move(returnType)),
          parameters(std::move(parameters)),
          statements(std::move(statements)) {}
};

class VariableDeclarationASTNode : public ASTNode {
public:
    std::string name;
    std::string type;
    std::unique_ptr<ASTNode> value;

    VariableDeclarationASTNode(
        std::string name,
        std::string type,
        std::unique_ptr<ASTNode> value
    )
        : name(std::move(name)),
          type(std::move(type)),
          value(std::move(value)) {}
};
class ReturnASTNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> value;

    explicit ReturnASTNode(std::unique_ptr<ASTNode> value)
        : value(std::move(value)) {}
};

class AssertASTNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;

    explicit AssertASTNode(std::unique_ptr<ASTNode> condition)
        : condition(std::move(condition)) {}
};

class AssumeASTNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;

    explicit AssumeASTNode(std::unique_ptr<ASTNode> condition)
        : condition(std::move(condition)) {}
};

class ContractASTNode : public ASTNode {
public:
    std::string contractType;
    std::unique_ptr<ASTNode> condition;

    ContractASTNode(
        std::string contractType,
        std::unique_ptr<ASTNode> condition
    )
        : contractType(std::move(contractType)),
          condition(std::move(condition)) {}
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::unique_ptr<ProgramASTNode> parse();

    const std::vector<std::string>& getErrors() const;
    const std::vector<Diagnostic>& getDiagnostics() const;

private:
    std::vector<Token> tokens;
    std::size_t cursor = 0;

    std::vector<std::string> errors;
    std::vector<Diagnostic> diagnostics;

    Token peek() const;
    Token advance();
    bool match(TokenType type);

    Token consume(
        TokenType type,
        const std::string& errorMessage
    );

    void consumeClosingParenthesis();
    void consumeOpeningParenthesis();

    void addError(
        int line,
        const std::string& message
    );

    void addDiagnostic(
        int id,
        int line,
        int column,
        std::size_t offset
    );

    void synchronize();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseOr();
    std::unique_ptr<ASTNode> parseAnd();
    std::unique_ptr<ASTNode> parseNot();

    std::unique_ptr<ASTNode> parseShift();
    std::unique_ptr<ASTNode> parseRelational();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseBitwiseAnd();
    std::unique_ptr<ASTNode> parseBitwiseXor();
    std::unique_ptr<ASTNode> parseBitwiseOr();

    std::unique_ptr<ASTNode> parseWhile();

    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseTerm();

    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseFunction();
    std::unique_ptr<ASTNode> parseVariableDeclaration();
    std::unique_ptr<ASTNode> parseReturn();
    std::unique_ptr<ASTNode> parseAssert();
    std::unique_ptr<ASTNode> parseAssume();
    std::unique_ptr<ASTNode> parseContract();

};

#endif
