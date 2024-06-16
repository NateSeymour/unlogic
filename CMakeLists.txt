cmake_minimum_required(VERSION 3.22)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)

# External Libs
find_package(SFML REQUIRED)
find_package(benchmark REQUIRED)
find_package(GTest REQUIRED)
find_package(BISON REQUIRED 3.8)
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
bison_target(unlogic-parser src/parser/Parser.y ${CMAKE_CURRENT_BINARY_DIR}/Parser.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Parser.h)
flex_target(unlogic-scanner src/parser/Scanner.l ${CMAKE_CURRENT_BINARY_DIR}/Scanner.cpp DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Scanner.h)

# Unlogic
add_library(unlogic STATIC
        src/TokenViewer.cpp
        src/TokenViewer.h
        src/parser/Node.cpp
        src/parser/Node.h
        src/Parser.cpp
        src/Parser.h
        src/Exception.cpp
        src/Exception.h
        src/Compiler.cpp
        src/Compiler.h
        ${BISON_unlogic-parser-OUTPUTS}
        ${FLEX_unlogic-scanner-OUTPUTS}
)
target_link_libraries(unlogic PUBLIC ${llvm_libs})
target_include_directories(unlogic PUBLIC ${LLVM_INCLUDE_DIRS})

add_library(unlogic-graphic
        src/graphic/Graph.cpp
        src/graphic/Graph.h
        src/graphic/SFMLX.h
)
target_link_libraries(unlogic-graphic PUBLIC unlogic sfml::sfml)
add_executable(unlogic-cli src/cli/main.cpp)
target_link_libraries(unlogic-cli PUBLIC unlogic unlogic-graphic)

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
