
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
    FN,
    LET,
    RETURN,
    U1,
    U8,
    U16,
    U32,
    U64,
    I1,
    I8,
    I16,
    I32,
    I64,
    F32,
    F64,
    BOOL,
    CHAR,
    STRING,
    REQUIRES,
    ENSURES,
    INVARIANT,
    ASSERT,
    ASSUME,
    RESULT,
    IDENTIFIER,
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
    ARROW,
    COLON,
    COMMA,
    AMPERSAND,
    PIPE,
    CARET,
    SHIFT_LEFT,
    SHIFT_RIGHT,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    VERIFY_START,
    VERIFY_END,
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

