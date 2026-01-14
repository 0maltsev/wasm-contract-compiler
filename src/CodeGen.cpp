#include "CodeGen.h"
#include <wasm.h>
#include <wasm-builder.h>
#include <binaryen-c.h>
#include <stdexcept>

std::vector<uint8_t> BinaryenCodeGen::compile_to_wasm(const HftFunction& func) {
    wasm::Module module;
    wasm::Builder builder(module);

    std::function<wasm::Expression*(const Expr*)> emit_expr;

    emit_expr = [&](const Expr* expr) -> wasm::Expression* {
        if (auto* lit = dynamic_cast<const LiteralExpr*>(expr)) {
            double val = lit->value;
            if (val == static_cast<double>(static_cast<int64_t>(val))) {
                return builder.makeConst(wasm::Literal(int32_t(val)));
            } else {
                return builder.makeConst(wasm::Literal(val));
            }
        }
        if (dynamic_cast<const VarExpr*>(expr)) {
            return builder.makeLocalGet(0, wasm::Type::f64);
        }
        if (auto* bin = dynamic_cast<const BinaryExpr*>(expr)) {
            auto* left = emit_expr(bin->left.get());
            auto* right = emit_expr(bin->right.get());
            if (bin->op == BinaryExpr::Op::GT) {
                return builder.makeBinary(wasm::BinaryOp::GtFloat64, left, right);
            }
        }
        if (auto* iff = dynamic_cast<const IfExpr*>(expr)) {
            return builder.makeIf(
                emit_expr(iff->cond.get()),
                emit_expr(iff->then_expr.get()),
                emit_expr(iff->else_expr.get())
            );
        }
        throw std::runtime_error("Unsupported expression");
    };

    auto* body = emit_expr(func.body.get());

    wasm::Signature sig({wasm::Type::f64}, wasm::Type::i32);

    module.addFunction(builder.makeFunction(
        "on_market_update",
        sig,
        {},
        body
    ));

    module.addExport(builder.makeExport(
        "on_market_update",
        "on_market_update",
        wasm::ExternalKind::Function
    ));

    BinaryenModuleRef cmod =
    reinterpret_cast<BinaryenModuleRef>(&module);

std::vector<uint8_t> buffer(1024 * 1024); // 1 MB
size_t written = BinaryenModuleWrite(
    cmod,
    reinterpret_cast<char*>(buffer.data()),
    buffer.size()
);
if (written == 0) {
    throw std::runtime_error("BinaryenModuleWrite failed");
}
buffer.resize(written);
return buffer;

}
