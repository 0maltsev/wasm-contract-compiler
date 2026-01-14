#pragma once
#include "Ast.h"
#include "Lexer.h"
#include <vector>
#include <memory>

class Parser {
    std::vector<Token> tokens;
    size_t current = 0;
public:
    Parser(std::vector<Token> toks) : tokens(std::move(toks)) {}
    std::unique_ptr<Function> parse_function();
private:
    bool match(TokenType type);
    Token consume(TokenType type, const char* msg);
    std::unique_ptr<Expr> parse_expression();
    std::unique_ptr<Expr> parse_if();
    std::unique_ptr<Expr> parse_primary();
    std::unique_ptr<Expr> parse_binary();
};