cmake_minimum_required(VERSION 3.23)

project("Trivial C++ Implementation")

# Python 3.9 or newer. 
# Nanobind requires this declared before it's called
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

add_library(trivialHelper STATIC helper.cpp)
target_compile_options(trivialHelper PRIVATE -O3 -flto=auto)

nanobind_add_module(trivialCPP tokenizer.cpp)

target_link_libraries(trivialCPP PRIVATE trivialHelper)

target_compile_options(trivialCPP PRIVATE -O3 -flto=auto -DNANOBIND)
