#pragma once
#include <vector>
#include <string>

enum class TokenType {
    IDENT, NUMBER, GT, LPAREN, RPAREN, LBRACE, RBRACE,
    IF, THEN, ELSE, ON, F64, I32, ARROW, COLON, EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string lexeme;
    double number_value = 0.0;
};

class Lexer {
    std::string source;
    size_t pos = 0;
public:
    Lexer(std::string src) : source(std::move(src)) {}
    std::vector<Token> tokenize();
private:
    char peek(int ahead = 0) const { 
        return (pos + ahead < source.size()) ? source[pos + ahead] : '\0'; 
    }
    char advance() { return pos < source.size() ? source[pos++] : '\0'; }
    void skip_whitespace();
    Token read_number();
    Token read_identifier();
};