
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
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseOr();
    std::unique_ptr<ASTNode> parseAnd();
    std::unique_ptr<ASTNode> parseNot();

    std::unique_ptr<ASTNode> parseWhile();

    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseTerm();

    std::unique_ptr<ASTNode> parseStatement();
};

#endif

