
#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <cstddef>

enum class TokenType {
    PRI,
    IF,
    ELSE,
    ELSE_IF,
    AND,
    OR,
    NOT,
    WHILE,
    IDENTIFIER,
    STRING,
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    EQUAL,
    GREATER,
    LESS,
    GREATER_EQUAL,
    LESS_EQUAL,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    DOTDOT,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    std::size_t offset;
};

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t cursor = 0;
    int currentLine = 1;
    int currentColumn = 1;

    char peek() const;
    char advance();
    void skipWhitespace();
};

#endif

