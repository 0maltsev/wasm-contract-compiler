cmake_minimum_required(VERSION 3.15)
project(hftc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/binaryen/CMakeLists.txt")
    message(FATAL_ERROR
        "Binaryen not found! Run:\n"
        "  git submodule update --init --recursive"
    )
endif()

add_subdirectory(third_party/binaryen)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(hftc ${SOURCES})

target_include_directories(hftc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(hftc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/binaryen/src
)

target_link_libraries(hftc
    PRIVATE
        binaryen
)
