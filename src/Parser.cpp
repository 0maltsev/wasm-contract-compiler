#include "Parser.h"
#include <stdexcept>

bool Parser::match(TokenType type) {
    if (current >= tokens.size()) return false;
    if (tokens[current].type == type) {
        current++;
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, const char* msg) {
    if (match(type)) return tokens[current - 1];
    throw std::runtime_error(msg);
}

std::unique_ptr<Expr> Parser::parse_primary() {
    if (match(TokenType::NUMBER)) {
        return std::make_unique<LiteralExpr>(tokens[current - 1].number_value);
    }
    if (match(TokenType::IDENT)) {
        return std::make_unique<VarExpr>(tokens[current - 1].lexeme);
    }
    throw std::runtime_error("Expected primary expression");
}

std::unique_ptr<Expr> Parser::parse_binary() {
    auto left = parse_primary();
    if (match(TokenType::GT)) {
        auto right = parse_primary();
        return std::make_unique<BinaryExpr>(BinaryExpr::Op::GT, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Expr> Parser::parse_if() {
    consume(TokenType::IF, "Expected 'if'");
    auto cond = parse_binary();
    consume(TokenType::THEN, "Expected 'then'");
    auto then_branch = parse_expression();
    consume(TokenType::ELSE, "Expected 'else'");
    auto else_branch = parse_expression();
    return std::make_unique<IfExpr>(std::move(cond), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Expr> Parser::parse_expression() {
    if (tokens[current].type == TokenType::IF) {
        return parse_if();
    }
    return parse_binary();
}

std::unique_ptr<HftFunction> Parser::parse_function() {
    consume(TokenType::ON, "Expected 'on'");
    auto event_tok = consume(TokenType::IDENT, "Expected event name");
    consume(TokenType::LPAREN, "Expected '('");
    auto param_tok = consume(TokenType::IDENT, "Expected parameter name");
    consume(TokenType::COLON, "Expected ':'");
    Type param_type = Type::F64;
    if (match(TokenType::F64)) { param_type = Type::F64; }
    else if (match(TokenType::I32)) { param_type = Type::I32; }
    else throw std::runtime_error("Unsupported parameter type");
    consume(TokenType::RPAREN, "Expected ')'");
    consume(TokenType::ARROW, "Expected '->'");
    Type return_type = Type::I32;
    if (match(TokenType::I32)) { return_type = Type::I32; }
    else throw std::runtime_error("Only i32 return supported");
    consume(TokenType::LBRACE, "Expected '{'");
    auto body = parse_expression();
    consume(TokenType::RBRACE, "Expected '}'");

    auto func = std::make_unique<HftFunction>();
    func->event_name = event_tok.lexeme;
    func->param_name = param_tok.lexeme;
    func->param_type = param_type;
    func->return_type = return_type;
    func->body = std::move(body);
    return func;
}