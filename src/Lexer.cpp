
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

    char c = source[cursor++];

    if (c == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }

    return c;
}

void Lexer::skipWhitespace() {
    while (std::isspace(
        static_cast<unsigned char>(peek()))) {
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
        int tokenColumn = currentColumn;
        std::size_t tokenOffset = cursor;

        if (current == '.' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '.') {

            advance();
            advance();

            tokens.push_back({
                TokenType::DOTDOT, "..",
                tokenLine, tokenColumn, tokenOffset
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
                TokenType::STRING, strVal,
                tokenLine, tokenColumn, tokenOffset
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
                TokenType::NUMBER, numVal,
                tokenLine, tokenColumn, tokenOffset
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
                    TokenType::PRI, ident,
                    tokenLine, tokenColumn, tokenOffset
                });

            } else if (ident == "if") {
                tokens.push_back({
                    TokenType::IF, ident,
                    tokenLine, tokenColumn, tokenOffset
                });

            } else if (ident == "and") {
                tokens.push_back({
                    TokenType::AND, ident,
                    tokenLine, tokenColumn, tokenOffset
                });

            } else if (ident == "or") {
                tokens.push_back({
                    TokenType::OR, ident,
                    tokenLine, tokenColumn, tokenOffset
                });

            } else if (ident == "not") {
                tokens.push_back({
                    TokenType::NOT, ident,
                    tokenLine, tokenColumn, tokenOffset
                });

            } else if (ident == "while") {
                tokens.push_back({
                    TokenType::WHILE, ident,
                    tokenLine, tokenColumn, tokenOffset
                });

                 } else if (ident == "fn") {
                tokens.push_back({
                    TokenType::FN, ident,
                    tokenLine, tokenColumn, tokenOffset
                });

            } else if (ident == "let") {
                tokens.push_back({
                    TokenType::LET, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "return") {
                tokens.push_back({
                    TokenType::RETURN, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "u1") {
                tokens.push_back({
                    TokenType::U1, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "u8") {
                tokens.push_back({
                    TokenType::U8, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "u16") {
                tokens.push_back({
                    TokenType::U16, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "u32") {
                tokens.push_back({
                    TokenType::U32, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "u64") {
                tokens.push_back({
                    TokenType::U64, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "i1") {
                tokens.push_back({
                    TokenType::I1, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "i8") {
                tokens.push_back({
                    TokenType::I8, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "i16") {
                tokens.push_back({
                    TokenType::I16, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "i32") {
                tokens.push_back({
                    TokenType::I32, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "i64") {
                tokens.push_back({
                    TokenType::I64, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "f32") {
                tokens.push_back({
                    TokenType::F32, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "f64") {
                tokens.push_back({
                    TokenType::F64, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "bool") {
                tokens.push_back({
                    TokenType::BOOL, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "char") {
                tokens.push_back({
                    TokenType::CHAR, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "string") {
                tokens.push_back({
                    TokenType::STRING, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

  } else if (ident == "requires") {
                tokens.push_back({
                    TokenType::REQUIRES, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     
} else if (ident == "ensures") {
                tokens.push_back({
                    TokenType::ENSURES, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     
} else if (ident == "invariant") {
                tokens.push_back({
                    TokenType::INVARIANT, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     
} else if (ident == "assert") {
                tokens.push_back({
                    TokenType::ASSERT, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     
} else if (ident == "assume") {
                tokens.push_back({
                    TokenType::ASSUME, ident,
                    tokenLine, tokenColumn, tokenOffset
                });    

} else if (ident == "result") {
                tokens.push_back({
                    TokenType::RESULT, ident,
                    tokenLine, tokenColumn, tokenOffset
                });     

            } else if (ident == "else") {
                std::size_t savedCursor = cursor;
                int savedLine = currentLine;
                int savedColumn = currentColumn;

                while (std::isspace(
                    static_cast<unsigned char>(peek()))) {
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
                        tokenLine, tokenColumn, tokenOffset
                    });

                } else {
                    cursor = savedCursor;
                    currentLine = savedLine;
                    currentColumn = savedColumn;

                    tokens.push_back({
                        TokenType::ELSE, ident,
                        tokenLine, tokenColumn, tokenOffset
                    });
                }

            } else {
                tokens.push_back({
                    TokenType::IDENTIFIER, ident,
                    tokenLine, tokenColumn, tokenOffset
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
                TokenType::EQUAL_EQUAL, "==",
                tokenLine, tokenColumn, tokenOffset
            });

            continue;
        }

        if (current == '!' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '=') {

            advance();
            advance();

            tokens.push_back({
                TokenType::NOT_EQUAL, "!=",
                tokenLine, tokenColumn, tokenOffset
            });

            continue;
        }

        if (current == '>' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '=') {

            advance();
            advance();

            tokens.push_back({
                TokenType::GREATER_EQUAL, ">=",
                tokenLine, tokenColumn, tokenOffset
            });

            continue;
        }

        if (current == '<' &&
            cursor + 1 < source.length() &&
            source[cursor + 1] == '=') {

            advance();
            advance();

            tokens.push_back({
                TokenType::LESS_EQUAL, "<=",
                tokenLine, tokenColumn, tokenOffset
            });

            continue;
        }

        char c = advance();

        switch (c) {
            case '=':
                tokens.push_back({
                    TokenType::EQUAL, "=",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;

            case '>':
                if (peek() == '>') {
                    advance();
                    tokens.push_back({
                        TokenType::SHIFT_RIGHT, ">>",
                        tokenLine, tokenColumn, tokenOffset
                    });
                } else {
                    tokens.push_back({
                        TokenType::GREATER, ">",
                        tokenLine, tokenColumn, tokenOffset
                    });
                }
                break;

            case '<':
                if (peek() == '<') {
                    advance();
                    tokens.push_back({
                        TokenType::SHIFT_LEFT, "<<",
                        tokenLine, tokenColumn, tokenOffset
                    });
                } else {
                    tokens.push_back({
                        TokenType::LESS, "<",
                        tokenLine, tokenColumn, tokenOffset
                    });
                }
                break;
            
            case '(':
                tokens.push_back({
                    TokenType::LPAREN, "(",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;

            case ')':
                tokens.push_back({
                    TokenType::RPAREN, ")",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;
            case ',':
                tokens.push_back({
                    TokenType::COMMA, ",",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;
            case ':':
                tokens.push_back({
                    TokenType::COLON, ":",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;


           case '{':
    if (peek() == '{') {
        advance();
        tokens.push_back({
            TokenType::VERIFY_START, "{{",
            tokenLine, tokenColumn, tokenOffset
        });
    } else {
        tokens.push_back({
            TokenType::LBRACE, "{",
            tokenLine, tokenColumn, tokenOffset
        });
    }
    break;

          case '}':
    if (peek() == '}') {
        advance();
        tokens.push_back({
            TokenType::VERIFY_END, "}}",
            tokenLine, tokenColumn, tokenOffset
        });
    } else {
        tokens.push_back({
            TokenType::RBRACE, "}",
            tokenLine, tokenColumn, tokenOffset
        });
    }
    break;

            case '+':
                tokens.push_back({
                    TokenType::PLUS, "+",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;
            case '-':
                if (peek() == '>') {
                    advance();
                    tokens.push_back({
                        TokenType::ARROW, "->",
                        tokenLine, tokenColumn, tokenOffset
                    });
                } else {
                    tokens.push_back({
                        TokenType::MINUS, "-",
                        tokenLine, tokenColumn, tokenOffset
                    });
                }
                break;

            case '*':
                tokens.push_back({
                    TokenType::MULTIPLY, "*",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;

            case '/':
                tokens.push_back({
                    TokenType::DIVIDE, "/",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;
            case '&':
                tokens.push_back({
                    TokenType::AMPERSAND, "&",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;

            case '|':
                tokens.push_back({
                    TokenType::PIPE, "|",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;

            case '^':
                tokens.push_back({
                    TokenType::CARET, "^",
                    tokenLine, tokenColumn, tokenOffset
                });
                break;

            default:
                tokens.push_back({
                    TokenType::UNKNOWN,
                    std::string(1, c),
                    tokenLine, tokenColumn, tokenOffset
                });
                break;
        }
    }

    tokens.push_back({
        TokenType::END_OF_FILE,
        "",
        currentLine,
        currentColumn,
        cursor
    });

    return tokens;
}
