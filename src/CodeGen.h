#pragma once
#include "Ast.h"
#include <vector>
#include <cstdint>

class BinaryenCodeGen {
public:
    static std::vector<uint8_t> compile_to_wasm(const HftFunction& func);
};