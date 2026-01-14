#pragma once
#include <string>
#include <memory>

enum class Type { F64, I32 };

struct Expr {
    virtual ~Expr() = default;
};

struct LiteralExpr : Expr {
    double value;
    LiteralExpr(double v) : value(v) {}
};

struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n) : name(std::move(n)) {}
};

struct BinaryExpr : Expr {
    enum Op { GT } op;
    std::unique_ptr<Expr> left, right;
    BinaryExpr(Op o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

struct IfExpr : Expr {
    std::unique_ptr<Expr> cond, then_expr, else_expr;
    IfExpr(std::unique_ptr<Expr> c, std::unique_ptr<Expr> t, std::unique_ptr<Expr> e)
        : cond(std::move(c)), then_expr(std::move(t)), else_expr(std::move(e)) {}
};

struct Function {
    std::string event_name; // "market_update"
    std::string param_name;
    Type param_type = Type::F64;
    Type return_type = Type::I32;
    std::unique_ptr<Expr> body;
};