#include "CodeGen.h"
#include <wasm.h>
#include <wasm-builder.h>
#include <wasm-binary.h>
#include <stdexcept>

std::vector<uint8_t> BinaryenCodeGen::compile_to_wasm(const HftFunction& func) {
    wasm::Module module;
    wasm::Builder builder(module);

    std::function<wasm::Expression*(const Expr*)> emit_expr;

    emit_expr = [&](const Expr* expr) -> wasm::Expression* {
        if (auto* lit = dynamic_cast<const LiteralExpr*>(expr)) {
            double val = lit->value;
            if (val == static_cast<double>(static_cast<int64_t>(val))) {
                return builder.makeConst(wasm::Literal(static_cast<int32_t>(val)));
            } else {
                return builder.makeConst(wasm::Literal(val));
            }
        }
        if (dynamic_cast<const VarExpr*>(expr)) {
            return builder.makeLocalGet(0, wasm::Type::f64);
        }
        if (auto* bin = dynamic_cast<const BinaryExpr*>(expr)) {
            wasm::Expression* left = emit_expr(bin->left.get());
            wasm::Expression* right = emit_expr(bin->right.get());
            if (bin->op == BinaryExpr::Op::GT) {
                return builder.makeBinary(wasm::BinaryOp::GtFloat64, left, right);
            }
        }
        if (auto* iff = dynamic_cast<const IfExpr*>(expr)) {
            wasm::Expression* cond = emit_expr(iff->cond.get());
            wasm::Expression* if_true = emit_expr(iff->then_expr.get());
            wasm::Expression* if_false = emit_expr(iff->else_expr.get());
            return builder.makeIf(cond, if_true, if_false);
        }
        throw std::runtime_error("Unsupported expression");
    };

    wasm::Expression* body = emit_expr(func.body.get());

    wasm::Signature sig(
    {wasm::Type::f64},
    wasm::Type::i32
);

    auto function = builder.makeFunction(
        wasm::Name("on_market_update"),
        sig,
        {},
        body
    );


    module.addFunction(std::move(function));

    module.addExport(builder.makeExport(
        "on_market_update",
        "on_market_update",
        wasm::ExternalKind::Function
    ));
    
    std::vector<uint8_t> buffer;
    wasm::ModuleWriter writer;
    writer.write(module, buffer);
    return buffer;
}
