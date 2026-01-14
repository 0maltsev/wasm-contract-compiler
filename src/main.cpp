#include "Lexer.h"
#include "Parser.h"
#include "CodeGen.h"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: hftc <input.hft> <output.wasm>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open input file\n";
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto func = parser.parse_function();

        auto wasm_bytes = BinaryenCodeGen::compile_to_wasm(*func);

        std::ofstream out(argv[2], std::ios::binary);
        out.write(reinterpret_cast<const char*>(wasm_bytes.data()), wasm_bytes.size());
        std::cout << "Compiled to " << argv[2] << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}