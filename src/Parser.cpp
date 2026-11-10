
#include "Parser.h"
#include "DiagnosticEngine.h"

#include <string>
#include <utility>
#include <vector>

Parser::Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens)) {}

Token Parser::peek() const {
    if (cursor >= tokens.size()) {
        return {
            TokenType::END_OF_FILE,
            "",
            tokens.empty() ? 1 : tokens.back().line,
            tokens.empty() ? 1 : tokens.back().column,
            tokens.empty() ? 0 :
                tokens.back().offset + tokens.back().value.size()
        };
    }

    return tokens[cursor];
}

Token Parser::advance() {
    if (cursor >= tokens.size()) {
        return {
            TokenType::END_OF_FILE,
            "",
            tokens.empty() ? 1 : tokens.back().line,
            tokens.empty() ? 1 : tokens.back().column,
            tokens.empty() ? 0 :
                tokens.back().offset + tokens.back().value.size()
        };
    }

    return tokens[cursor++];
}

bool Parser::match(TokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }

    return false;
}

void Parser::addError(
    int line,
    const std::string& message
) {
    errors.push_back(
        "Error: Line " +
        std::to_string(line) +
        ": " +
        message
    );
}

void Parser::addDiagnostic(
    int id,
    int line,
    int column,
    std::size_t offset
) {
    DiagnosticEngine engine;

    diagnostics.push_back(
        engine.analyze(id, line, column, offset)
    );
}

Token Parser::consume(
    TokenType type,
    const std::string& errorMessage
) {
    if (peek().type == type) {
        return advance();
    }

    addError(peek().line, errorMessage);

    return {
        type,
        "",
        peek().line,
        peek().column,
        peek().offset
    };
}

void Parser::consumeClosingParenthesis() {
    if (peek().type == TokenType::RPAREN) {
        advance();
    } else {
        addDiagnostic(
            1,
            peek().line,
            peek().column,
            peek().offset
        );
    }
}

void Parser::consumeOpeningParenthesis() {
    if (peek().type == TokenType::LPAREN) {
        advance();
    } else {
        addDiagnostic(
            4,
            peek().line,
            peek().column,
            peek().offset
        );
    }
}

void Parser::synchronize() {
    while (
        peek().type != TokenType::END_OF_FILE &&
        peek().type != TokenType::PRI &&
        peek().type != TokenType::IF &&
        peek().type != TokenType::ELSE_IF &&
        peek().type != TokenType::WHILE &&
        peek().type != TokenType::IDENTIFIER &&
        peek().type != TokenType::RBRACE
    ) {
        advance();
    }
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (match(TokenType::LPAREN)) {
        auto node = parseOr();

        consumeClosingParenthesis();

        return node;
    }

    if (match(TokenType::NUMBER)) {
        return std::make_unique<NumberASTNode>(
            tokens[cursor - 1].value
        );
    }

    if (match(TokenType::STRING)) {
        return std::make_unique<StringASTNode>(
            tokens[cursor - 1].value
        );
    }

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableASTNode>(
            tokens[cursor - 1].value
        );
    }

    addError(peek().line, "Expected expression");

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseTerm() {
    auto node = parsePrimary();

    while (
        peek().type == TokenType::MULTIPLY ||
        peek().type == TokenType::DIVIDE
    ) {
        Token op = advance();
        auto right = parsePrimary();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            op.value,
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    auto node = parseTerm();

    while (
        peek().type == TokenType::PLUS ||
        peek().type == TokenType::MINUS
    ) {
        Token op = advance();
        auto right = parseTerm();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            op.value,
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseShift() {
    auto node = parseExpression();

    while (
        peek().type == TokenType::SHIFT_LEFT ||
        peek().type == TokenType::SHIFT_RIGHT
    ) {
        Token op = advance();
        auto right = parseExpression();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            op.value,
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseRelational() {
    auto node = parseShift();

    while (
        peek().type == TokenType::GREATER ||
        peek().type == TokenType::LESS ||
        peek().type == TokenType::GREATER_EQUAL ||
        peek().type == TokenType::LESS_EQUAL
    ) {
        Token op = advance();
        auto right = parseShift();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            op.value,
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
    auto node = parseRelational();

    while (
        peek().type == TokenType::EQUAL_EQUAL ||
        peek().type == TokenType::NOT_EQUAL
    ) {
        Token op = advance();
        auto right = parseRelational();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            op.value,
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseAnd() {
    auto node = parseEquality();

    while (match(TokenType::AMPERSAND)) {
        Token op = tokens[cursor - 1];
        auto right = parseEquality();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            op.value,
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseXor() {
    auto node = parseBitwiseAnd();

    while (match(TokenType::CARET)) {
        Token op = tokens[cursor - 1];
        auto right = parseBitwiseAnd();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            op.value,
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseBitwiseOr() {
    auto node = parseBitwiseXor();

    while (match(TokenType::PIPE)) {
        Token op = tokens[cursor - 1];
        auto right = parseBitwiseXor();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            op.value,
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseNot() {
    if (match(TokenType::NOT)) {
        auto node = parseNot();

        if (!node) {
            return nullptr;
        }

        return std::make_unique<UnaryASTNode>(
            "not",
            std::move(node)
        );
    }

    return parseBitwiseOr();
}

std::unique_ptr<ASTNode> Parser::parseAnd() {
    auto node = parseNot();

    while (match(TokenType::AND)) {
        auto right = parseNot();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            "and",
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseOr() {
    auto node = parseAnd();

    while (match(TokenType::OR)) {
        auto right = parseAnd();

        if (!node || !right) {
            node = nullptr;
            continue;
        }

        node = std::make_unique<BinaryASTNode>(
            "or",
            std::move(node),
            std::move(right)
        );
    }

    return node;
}

std::unique_ptr<ASTNode> Parser::parseWhile() {
    consume(TokenType::WHILE, "Expected 'while'");

    consumeOpeningParenthesis();

    auto condition = parseOr();

    consumeClosingParenthesis();

    consume(TokenType::LBRACE, "Expected '{' before while body");

    std::vector<std::unique_ptr<ASTNode>> statements;

    while (
        peek().type != TokenType::RBRACE &&
        peek().type != TokenType::END_OF_FILE
    ) {
        std::size_t previousCursor = cursor;
        auto statement = parseStatement();

        if (statement) {
            statements.push_back(std::move(statement));
        }

        if (cursor == previousCursor) {
            advance();
        }
    }

    consume(TokenType::RBRACE, "Expected '}' after while body");

    if (condition) {
        return std::make_unique<WhileASTNode>(
            std::move(condition),
            std::move(statements)
        );
    }

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseFunction() {
    consume(TokenType::FN, "Expected 'fn'");

    Token nameToken = consume(
        TokenType::IDENTIFIER,
        "Expected function name"
    );

        TokenType::LPAREN,
        consumeOpeningParenthesis();
    

    std::vector<std::unique_ptr<ParameterASTNode>> parameters;

    if (peek().type != TokenType::RPAREN) {
        while (true) {
            Token typeToken = advance();

            Token parameterName = consume(
                TokenType::IDENTIFIER,
                "Expected parameter name"
            );

            parameters.push_back(
                std::make_unique<ParameterASTNode>(
                    parameterName.value,
                    typeToken.value
                )
            );

            if (!match(TokenType::COMMA)) {
                break;
            }
        }
    }

        TokenType::RPAREN,
        consumeClosingParenthesis();

    consume(
        TokenType::ARROW,
        "Expected '->' before return type"
    );

    Token returnTypeToken = advance();

    consume(
        TokenType::LBRACE,
        "Expected '{' before function body"
    );

    std::vector<std::unique_ptr<ASTNode>> statements;

    while (
        peek().type != TokenType::RBRACE &&
        peek().type != TokenType::END_OF_FILE
    ) {
        std::size_t previousCursor = cursor;

        auto statement = parseStatement();

        if (statement) {
            statements.push_back(std::move(statement));
        }

        if (cursor == previousCursor) {
            advance();
        }
    }

    consume(
        TokenType::RBRACE,
        "Expected '}' after function body"
    );

    if (nameToken.type != TokenType::IDENTIFIER) {
        return nullptr;
    }

    return std::make_unique<FunctionASTNode>(
        nameToken.value,
        returnTypeToken.value,
        std::move(parameters),
        std::move(statements)
    );
}

std::unique_ptr<ASTNode> Parser::parseVariableDeclaration() {
    consume(
        TokenType::LET,
        "Expected 'let'"
    );

    Token nameToken = consume(
        TokenType::IDENTIFIER,
        "Expected variable name"
    );

    consume(
        TokenType::COLON,
        "Expected ':' after variable name"
    );

    if (
        peek().type != TokenType::U1 &&
        peek().type != TokenType::U8 &&
        peek().type != TokenType::U16 &&
        peek().type != TokenType::U32 &&
        peek().type != TokenType::U64 &&
        peek().type != TokenType::I1 &&
        peek().type != TokenType::I8 &&
        peek().type != TokenType::I16 &&
        peek().type != TokenType::I32 &&
        peek().type != TokenType::I64 &&
        peek().type != TokenType::F32 &&
        peek().type != TokenType::F64 &&
        peek().type != TokenType::BOOL &&
        peek().type != TokenType::CHAR &&
        peek().type != TokenType::STRING
    ) {
        addError(
            peek().line,
            "Expected a valid data type"
        );

        return nullptr;
    }

    Token typeToken = advance();

    consume(
        TokenType::EQUAL,
        "Expected '=' after variable type"
    );

    auto value = parseOr();

    consume(
        TokenType::DOTDOT,
        "Expected '..' at the end of variable declaration"
    );

    if (
        nameToken.type == TokenType::IDENTIFIER &&
        value
    ) {
        return std::make_unique<VariableDeclarationASTNode>(
            nameToken.value,
            typeToken.value,
            std::move(value)
        );
    }

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseReturn() {
    consume(
        TokenType::RETURN,
        "Expected 'return'"
    );

    auto value = parseOr();

    consume(
        TokenType::DOTDOT,
        "Expected '..' at the end of return statement"
    );

    if (value) {
        return std::make_unique<ReturnASTNode>(
            std::move(value)
        );
    }

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseAssert() {
    consume(
        TokenType::ASSERT,
        "Expected 'assert'"
    );

    consumeOpeningParenthesis();

    auto condition = parseOr();

    consumeClosingParenthesis();

    consume(
        TokenType::DOTDOT,
        "Expected '..' at the end of assert statement"
    );

    if (condition) {
        return std::make_unique<AssertASTNode>(
            std::move(condition)
        );
    }

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseAssume() {
    consume(
        TokenType::ASSUME,
        "Expected 'assume'"
    );

    consumeOpeningParenthesis();

    auto condition = parseOr();

    consumeClosingParenthesis();

    consume(
        TokenType::DOTDOT,
        "Expected '..' at the end of assume statement"
    );

    if (condition) {
        return std::make_unique<AssumeASTNode>(
            std::move(condition)
        );
    }

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseContract() {
    Token contractToken = advance();

    if (
        contractToken.type != TokenType::REQUIRES &&
        contractToken.type != TokenType::ENSURES &&
        contractToken.type != TokenType::INVARIANT
    ) {
        addError(
            contractToken.line,
            "Expected a contract keyword"
        );

        return nullptr;
    }

    consumeOpeningParenthesis();

    auto condition = parseOr();

    consumeClosingParenthesis();

    consume(
        TokenType::DOTDOT,
        "Expected '..' at the end of contract"
    );

    if (condition) {
        return std::make_unique<ContractASTNode>(
            contractToken.value,
            std::move(condition)
        );
    }

    return nullptr;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (peek().type == TokenType::WHILE) {
        return parseWhile();
    }

if (peek().type == TokenType::FN) {
    return parseFunction();
}

if (peek().type == TokenType::LET) {
    return parseVariableDeclaration();
}

if (peek().type == TokenType::RETURN) {
    return parseReturn();
}

if (peek().type == TokenType::ASSERT) {
    return parseAssert();
}

if (peek().type == TokenType::ASSUME) {
    return parseAssume();
}

if (
    peek().type == TokenType::REQUIRES ||
    peek().type == TokenType::ENSURES ||
    peek().type == TokenType::INVARIANT
) {
    return parseContract();
}

    if (
        match(TokenType::IF) ||
        match(TokenType::ELSE_IF)
    ) {
        consumeOpeningParenthesis();

        auto condition = parseOr();

        consumeClosingParenthesis();

        consume(
            TokenType::LBRACE,
            "Expected '{' before if body"
        );

        std::vector<std::unique_ptr<ASTNode>> statements;

        while (
            peek().type != TokenType::RBRACE &&
            peek().type != TokenType::END_OF_FILE
        ) {
            std::size_t previousCursor = cursor;
            auto statement = parseStatement();

            if (statement) {
                statements.push_back(std::move(statement));
            }

            if (cursor == previousCursor) {
                advance();
            }
        }

        consume(
            TokenType::RBRACE,
            "Expected '}' after if body"
        );

        std::vector<std::unique_ptr<ASTNode>> elseStatements;

        if (peek().type == TokenType::ELSE_IF) {
            auto elseIfStatement = parseStatement();

            if (elseIfStatement) {
                elseStatements.push_back(
                    std::move(elseIfStatement)
                );
            }
        } else if (match(TokenType::ELSE)) {
            consume(
                TokenType::LBRACE,
                "Expected '{' before else body"
            );

            while (
                peek().type != TokenType::RBRACE &&
                peek().type != TokenType::END_OF_FILE
            ) {
                std::size_t previousCursor = cursor;
                auto statement = parseStatement();

                if (statement) {
                    elseStatements.push_back(
                        std::move(statement)
                    );
                }

                if (cursor == previousCursor) {
                    advance();
                }
            }

            consume(
                TokenType::RBRACE,
                "Expected '}' after else body"
            );
        }

        if (condition) {
            return std::make_unique<IfASTNode>(
                std::move(condition),
                std::move(statements),
                std::move(elseStatements)
            );
        }

        return nullptr;
    }

    if (match(TokenType::PRI)) {
        std::unique_ptr<ASTNode> expr;

        if (match(TokenType::LPAREN)) {
            expr = parseOr();

            consumeClosingParenthesis();
        } else {
            std::size_t openingOffset = peek().offset;

            expr = parseOr();

            if (peek().type == TokenType::RPAREN) {
                addDiagnostic(
                    4,
                    peek().line,
                    peek().column,
                    openingOffset
                );
            }
        }

        consume(
            TokenType::DOTDOT,
            "Expected '..' at the end of statement"
        );

        if (expr) {
            return std::make_unique<PrintASTNode>(
                std::move(expr)
            );
        }

        return nullptr;
    }

    if (peek().type == TokenType::IDENTIFIER) {
        Token nameToken = advance();

        consume(
            TokenType::EQUAL,
            "Expected '=' after variable name"
        );

        auto value = parseOr();

        consume(
            TokenType::DOTDOT,
            "Expected '..' at the end of statement"
        );

        if (value) {
            return std::make_unique<AssignmentASTNode>(
                nameToken.value,
                std::move(value)
            );
        }

        return nullptr;
    }

    if (peek().type == TokenType::RBRACE) {
        addError(
            peek().line,
            "Unexpected '}'"
        );

        return nullptr;
    }

    Token token = advance();

    addError(
        token.line,
        "Unknown statement type at: " + token.value
    );

    synchronize();

    return nullptr;
}

std::unique_ptr<ProgramASTNode> Parser::parse() {
    auto program = std::make_unique<ProgramASTNode>();

    while (peek().type != TokenType::END_OF_FILE) {
        std::size_t previousCursor = cursor;
        auto statement = parseStatement();

        if (statement) {
            program->statements.push_back(
                std::move(statement)
            );
        }

        if (cursor == previousCursor) {
            advance();
        }
    }

    return program;
}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

const std::vector<Diagnostic>& Parser::getDiagnostics() const {
    return diagnostics;
}

