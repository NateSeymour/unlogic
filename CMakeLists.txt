cmake_minimum_required(VERSION 3.22)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)

# External Libs
find_package(SFML REQUIRED)
find_package(benchmark REQUIRED)
find_package(GTest REQUIRED)
find_package(BISON 3.8 REQUIRED)
find_package(FLEX REQUIRED)

find_package(LLVM 17 CONFIG REQUIRED)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# llvm_map_components_to_libnames is being a PAIN IN MY ASS on MacOS. It keeps trying to include a non-existent "LLVMjit" library. So I just brute force all
# libs into it because I am lazy.
if(APPLE)
    set(llvm_libs ${LLVM_AVAILABLE_LIBS})
else()
    llvm_map_components_to_libnames(llvm_libs core support irreader engine native)
endif()

# Parser
FLEX_TARGET(unlogic-scanner src/parser/Scanner.l ${CMAKE_CURRENT_BINARY_DIR}/parser/Scanner.cpp)
BISON_TARGET(unlogic-parser src/parser/Parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser/Parser.cpp)

# Unlogic
add_library(unlogic STATIC
        src/parser/Node.cpp
        src/parser/Node.h
        src/Exception.cpp
        src/Exception.h
        src/Compiler.cpp
        src/Compiler.h
        ${FLEX_unlogic-scanner_OUTPUTS}
        ${BISON_unlogic-parser_OUTPUTS}
        src/parser/ParserInterface.h
        src/parser/ParserInterface.cpp
)
target_link_libraries(unlogic PUBLIC ${llvm_libs})
target_include_directories(unlogic PUBLIC ${LLVM_INCLUDE_DIRS} src ${CMAKE_CURRENT_BINARY_DIR})

add_library(unlogic-graphic
        src/graphic/Graph.cpp
        src/graphic/Graph.h
        src/graphic/SFMLX.h
)
target_link_libraries(unlogic-graphic PUBLIC unlogic sfml-graphics)

add_executable(unlogic-cli src/cli/main.cpp)
target_link_libraries(unlogic-cli PUBLIC unlogic unlogic-graphic sfml-window)

add_executable(unlogic-test
        test/unlogic.test.cpp
        test/unlogic-graphic.test.cpp
        test/main.test.cpp
)
target_link_libraries(unlogic-test PUBLIC unlogic unlogic-graphic GTest::gtest)
target_include_directories(unlogic-test PUBLIC src)

add_executable(unlogic-benchmark
        benchmark/unlogic-graphic.benchmark.cpp
)
target_link_libraries(unlogic-benchmark PRIVATE unlogic unlogic-graphic benchmark::benchmark_main)
target_include_directories(unlogic-benchmark PRIVATE src)
