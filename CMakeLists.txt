cmake_minimum_required(VERSION 3.22)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)

# External Libs
find_package(SFML 2.6.1 REQUIRED)
find_package(benchmark REQUIRED)
find_package(GTest REQUIRED)
find_package(BISON 3.8.2 REQUIRED)
find_package(FLEX 2.6.4 REQUIRED)

find_package(LLVM 17 CONFIG REQUIRED)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0)

include(FetchContent)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.90.8
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
        imgui-sfml
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
        GIT_TAG v2.6
)
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_SFML_IMGUI_DEMO ON)
FetchContent_MakeAvailable(imgui-sfml)


# llvm_map_components_to_libnames is being a PAIN IN MY ASS. It keeps trying to include a non-existent "LLVMjit" library. So I just brute force all
# libs into it because I am lazy.
if(APPLE)
    set(llvm_libs ${LLVM_AVAILABLE_LIBS})
else()
    set(llvm_libs -lLLVM-17)
endif()

# Parser
FLEX_TARGET(unlogic-scanner src/parser/Scanner.l ${CMAKE_CURRENT_BINARY_DIR}/parser/Scanner.cpp)
BISON_TARGET(unlogic-parser src/parser/Parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser/Parser.cpp)

# Workaround for strange flex packaging on MacOS
if(APPLE)
    unset(FLEX_LIBRARIES)
endif()

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
        src/std/StandardLibrary.cpp
        src/std/StandardLibrary.h
        src/Library.cpp
        src/Library.h
)
target_link_libraries(unlogic PUBLIC ${llvm_libs} ${FLEX_LIBRARIES})
target_include_directories(unlogic PUBLIC ${LLVM_INCLUDE_DIRS} src ${CMAKE_CURRENT_BINARY_DIR})

add_library(unlogic-graphic
        src/graphic/Graph.cpp
        src/graphic/Graph.h
        src/graphic/SFMLX.h
)
target_link_libraries(unlogic-graphic PUBLIC unlogic sfml-graphics ImGui-SFML::ImGui-SFML)

add_executable(unlogic-cli
        src/cli/main.cpp
)
target_link_libraries(unlogic-cli PUBLIC unlogic unlogic-graphic sfml-window)

add_executable(unlogic-calculator
        src/calculator/main.cpp
)
target_link_directories(unlogic-calculator PUBLIC ${GTKMM_LIBRARY_DIRS})
target_include_directories(unlogic-calculator PUBLIC ${GTKMM_INCLUDE_DIRS})
target_link_libraries(unlogic-calculator PUBLIC ${GTKMM_LIBRARIES})

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
