# 1. Создать проект
mkdir hft-dsl && cd hft-dsl

# 2. Сохранить все файлы выше в правильные папки

# 3. Инициализировать Binaryen как подмодуль
git init
git submodule add https://github.com/WebAssembly/binaryen.git third_party/binaryen
git submodule update --init --recursive

# 4. Собрать
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# 5. Запустить
./hftc ../examples/simple.hft strategy.wasm

# 6. Проверить (prerequisite wabt: sudo apt install wabt)
wasm2wat strategy.wasm