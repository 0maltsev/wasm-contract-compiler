#include "CodeGen.h"
#include <binaryen/wasm.h>
#include <binaryen/wasm-builder.h>
#include <stdexcept>

using namespace wasm;

std::vector<uint8_t> BinaryenCodeGen::compile_to_wasm(const Function& func) {
    Module module;
    Builder builder(module);

    // Тип функции: (param f64) -> i32
    auto* sig = module.addFunctionType(
        Name("ft"), 
        Type::i32, 
        {Type::f64}
    );

    auto* function = new Function();
    function->name = Name("on_market_update");
    function->type = sig->name;

    // Параметр — local[0]
    function->vars.push_back(Type::f64);

    // Генерация тела
    auto emit_expr = [&](const Expr* expr) -> Expression* {
        if (auto* lit = dynamic_cast<const LiteralExpr*>(expr)) {
            if (lit->value == static_cast<int64_t>(lit->value)) {
                return builder.makeConst(Literal(int32_t(lit->value)));
            } else {
                return builder.makeConst(Literal(lit->value));
            }
        }
        if (dynamic_cast<const VarExpr*>(expr)) {
            return builder.makeLocalGet(0, Type::f64);
        }
        if (auto* bin = dynamic_cast<const BinaryExpr*>(expr)) {
            auto* left = emit_expr(bin->left.get());
            auto* right = emit_expr(bin->right.get());
            if (bin->op == BinaryExpr::Op::GT) {
                return builder.makeBinary(GtFloat64, left, right);
            }
        }
        if (auto* iff = dynamic_cast<const IfExpr*>(expr)) {
            auto* cond = emit_expr(iff->cond.get());
            auto* if_true = emit_expr(iff->then_expr.get());
            auto* if_false = emit_expr(iff->else_expr.get());
            return builder.makeIf(cond, if_true, if_false);
        }
        throw std::runtime_error("Unsupported expression");
    };

    function->body = emit_expr(func.body.get());
    module.addFunction(function);

    // Экспорт
    module.addExport(builder.makeExport(
        "on_market_update", "on_market_update", ExternalKind::Function
    ));

    // Сериализация
    BufferWithRandomAccess buffer;
    WasmBinaryWriter writer(&module, buffer);
    writer.write();
    return std::vector<uint8_t>(buffer.begin(), buffer.end());
}