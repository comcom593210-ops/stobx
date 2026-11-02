#include "Lexer.h"
#include <cctype>
#include <utility>

Lexer::Lexer(std::string source)
    : source(std::move(source)) {}

char Lexer::peek() const {
    if (cursor >= source.length())
        return '\0';

    return source[cursor];
}

char Lexer::advance() {
    if (cursor >= source.length())
        return '\0';

    return source[cursor++];
}

void Lexer::skipWhitespace() {
    while (std::isspace(
        static_cast<unsigned char>(peek()))) {

        if (peek() == '\n')
            currentLine++;

        advance();
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (cursor < source.length()) {
        skipWhitespace();

        char current = peek();

        if (current == '\0')
            break;

        int tokenLine = currentLine;

        if (current == '.' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '.') {

            advance();
            advance();

            tokens.push_back({
                TokenType::DOTDOT, "..", tokenLine
            });

            continue;
        }

        if (current == '"') {
            advance();

            std::string strVal;

            while (peek() != '"' && peek() != '\0')
                strVal += advance();

            if (peek() == '"')
                advance();

            tokens.push_back({
                TokenType::STRING, strVal, tokenLine
            });

            continue;
        }

        if (std::isdigit(
            static_cast<unsigned char>(current))) {

            std::string numVal;

            while (std::isdigit(
                static_cast<unsigned char>(peek()))) {
                numVal += advance();
            }

            tokens.push_back({
                TokenType::NUMBER, numVal, tokenLine
            });

            continue;
        }

        if (std::isalpha(
            static_cast<unsigned char>(current))) {

            std::string ident;

            while (std::isalnum(
                static_cast<unsigned char>(peek()))) {
                ident += advance();
            }

            if (ident == "pri") {
                tokens.push_back({
                    TokenType::PRI, ident, tokenLine
                });
            } else if (ident == "if") {
                tokens.push_back({
                    TokenType::IF, ident, tokenLine
                });
                } else if (ident == "and") {
                tokens.push_back({
                    TokenType::AND, ident, tokenLine
                });
} else if (ident == "or") {
                tokens.push_back({
                    TokenType::OR, ident, tokenLine
                });
} else if (ident == "not") {
                tokens.push_back({
                    TokenType::NOT, ident, tokenLine
                });

} else if (ident == "while") {
                tokens.push_back({
                    TokenType::WHILE, ident, tokenLine
                });

            } else if (ident == "else") {
                std::size_t savedCursor = cursor;
                int savedLine = currentLine;

                while (std::isspace(
                    static_cast<unsigned char>(peek()))) {

                    if (peek() == '\n')
                        currentLine++;

                    advance();
                }

                bool isIf =
                    peek() == 'i' &&
                    cursor + 1 < source.length() &&
                    source[cursor + 1] == 'f' &&
                    (cursor + 2 >= source.length() ||
                     !std::isalnum(
                         static_cast<unsigned char>(
                             source[cursor + 2]
                         )
                     ));

                if (isIf) {
                    advance();
                    advance();

                    tokens.push_back({
                        TokenType::ELSE_IF,
                        "else if",
                        tokenLine
                    });
                } else {
                    cursor = savedCursor;
                    currentLine = savedLine;

                    tokens.push_back({
                        TokenType::ELSE, ident, tokenLine
                    });
                }
            } else {
                tokens.push_back({
                    TokenType::IDENTIFIER, ident, tokenLine
                });
            }

            continue;
        }
         if (current == '=' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '=') {

            advance();
            advance();

            tokens.push_back({
                TokenType::EQUAL_EQUAL, "==", tokenLine
            });

            continue;
        }

        if (current == '!' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '=') {

            advance();
            advance();

            tokens.push_back({
                TokenType::NOT_EQUAL, "!=", tokenLine
            });

            continue;
        }

        if (current == '>' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '=') {

            advance();
            advance();

            tokens.push_back({
                TokenType::GREATER_EQUAL, ">=", tokenLine
            });

            continue;
        }

        if (current == '<' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '=') {

            advance();
            advance();

            tokens.push_back({
                TokenType::LESS_EQUAL, "<=", tokenLine
            });

            continue;
        }

        char c = advance();

        switch (c) {
            case '=':
                tokens.push_back({
                    TokenType::EQUAL, "=", tokenLine
                });
                break;

            case '>':
                tokens.push_back({
                    TokenType::GREATER, ">", tokenLine
                });
                break;

            case '<':
                tokens.push_back({
                    TokenType::LESS, "<", tokenLine
                });
                break;

            case '(':
                tokens.push_back({
                    TokenType::LPAREN, "(", tokenLine
                });
                break;

            case ')':
                tokens.push_back({
                    TokenType::RPAREN, ")", tokenLine
                });
                break;

            case '{':
                tokens.push_back({
                    TokenType::LBRACE, "{", tokenLine
                });
                break;

            case '}':
                tokens.push_back({
                    TokenType::RBRACE, "}", tokenLine
                });
                break;

            case '+':
                tokens.push_back({
                    TokenType::PLUS, "+", tokenLine
                });
                break;

            case '-':
                tokens.push_back({
                    TokenType::MINUS, "-", tokenLine
                });
                break;

            case '*':
                tokens.push_back({
                    TokenType::MULTIPLY, "*", tokenLine
                });
                break;

            case '/':
                tokens.push_back({
                    TokenType::DIVIDE, "/", tokenLine
                });
                break;

            default:
                tokens.push_back({
                    TokenType::UNKNOWN,
                    std::string(1, c),
                    tokenLine
                });
                break;
        }
    }

    tokens.push_back({
        TokenType::END_OF_FILE,
        "",
        currentLine
    });

    return tokens;
}