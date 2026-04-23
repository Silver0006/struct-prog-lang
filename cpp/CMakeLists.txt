cmake_minimum_required(VERSION 3.23)

project("Trivial Parser C++")

# Python 3.9 or newer. Nanobind requires this declared before it's called
find_package(Python COMPONENTS Interpreter Development REQUIRED)

include(FetchContent)
FetchContent_Declare(nanobind
    GIT_REPOSITORY https://github.com/wjakob/nanobind
    GIT_TAG        v2.12.0
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    TRUE
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(nanobind)

nanobind_add_module(parserCPP parser.cpp)

target_compile_options(parserCPP PRIVATE
    -O3 -flto=auto
)
