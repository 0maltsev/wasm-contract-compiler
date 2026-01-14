#include "Lexer.h"
#include <cctype>
#include <stdexcept>

void Lexer::skip_whitespace() {
    while (peek() && std::isspace(peek())) advance();
}

Token Lexer::read_number() {
    size_t start = pos;
    while (std::isdigit(peek()) || peek() == '.') advance();
    std::string num_str = source.substr(start, pos - start);
    return Token{TokenType::NUMBER, num_str, std::stod(num_str)};
}

Token Lexer::read_identifier() {
    size_t start = pos;
    while (std::isalpha(peek()) || peek() == '_') advance();
    std::string ident = source.substr(start, pos - start);
    
    if (ident == "if") return {TokenType::IF, ident};
    if (ident == "then") return {TokenType::THEN, ident};
    if (ident == "else") return {TokenType::ELSE, ident};
    if (ident == "on") return {TokenType::ON, ident};
    if (ident == "f64") return {TokenType::F64, ident};
    if (ident == "i32") return {TokenType::I32, ident};
    
    return {TokenType::IDENT, ident};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < source.size()) {
        skip_whitespace();
        if (pos >= source.size()) break;

        char c = peek();
        switch (c) {
            case '>': 
                if (peek(1) == '=') { /* не поддерживаем */ }
                else { tokens.push_back({TokenType::GT, ">"}); advance(); }
                break;
            case '(': tokens.push_back({TokenType::LPAREN, "("}); advance(); break;
            case ')': tokens.push_back({TokenType::RPAREN, ")"}); advance(); break;
            case '{': tokens.push_back({TokenType::LBRACE, "{"}); advance(); break;
            case '}': tokens.push_back({TokenType::RBRACE, "}"}); advance(); break;
            case ':': tokens.push_back({TokenType::COLON, ":"}); advance(); break;
            case '-':
                if (peek(1) == '>') { tokens.push_back({TokenType::ARROW, "->"}); advance(); advance(); }
                else throw std::runtime_error("Unexpected character");
                break;
            default:
                if (std::isdigit(c) || c == '.') {
                    tokens.push_back(read_number());
                } else if (std::isalpha(c)) {
                    tokens.push_back(read_identifier());
                } else {
                    throw std::runtime_error("Unexpected character: " + std::string(1, c));
                }
        }
    }
    tokens.push_back({TokenType::EOF_TOKEN, ""});
    return tokens;
}