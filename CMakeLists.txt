cmake_minimum_required(VERSION 3.22)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)

# External Libs
find_package(LLVM 17 CONFIG REQUIRED)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM gtkmm-4.0 epoxy pango)

include(FetchContent)
FetchContent_Declare(
        ctre
        GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
        GIT_TAG v3.9.0
)
FetchContent_MakeAvailable(ctre)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
        GTEST
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)
FetchContent_MakeAvailable(GTEST)

# llvm_map_components_to_libnames is being a PAIN IN MY ASS. It keeps trying to include a non-existent "LLVMjit" library. So I just brute force all
# libs into it because I am lazy.
if(APPLE)
    set(llvm_libs ${LLVM_AVAILABLE_LIBS})
else()
    set(llvm_libs -lLLVM-17)
endif()

# Unlogic
add_library(unlogic STATIC
        src/parser/Node.cpp
        src/parser/Node.h
        src/Compiler.cpp
        src/Compiler.h
        src/std/StandardLibrary.cpp
        src/std/StandardLibrary.h
        src/Library.cpp
        src/Library.h
        src/util/io.cpp
        src/util/io.h
        src/parser/Lex.h
        src/Error.h
        src/parser/Unlogic.cpp
)
target_link_libraries(unlogic PUBLIC ${llvm_libs} ctre::ctre)
target_include_directories(unlogic PUBLIC ${LLVM_INCLUDE_DIRS} src ${CMAKE_CURRENT_BINARY_DIR})

add_executable(unlogic-calculator
        src/calculator/main.cpp
        src/calculator/ui/Window.cpp
        src/calculator/ui/Window.h
        src/calculator/ui/SourceEditor.h
        src/calculator/ui/CanvasArea.h
        src/graphic/Graph.cpp
        src/graphic/Graph.h
        src/graphic/ugl/Drawable.h
        src/graphic/ugl/Drawable.cpp
        src/graphic/ugl/shapes/Line.cpp
        src/graphic/ugl/shapes/Line.h
        src/graphic/ugl/Canvas.cpp
        src/graphic/ugl/Canvas.h
        src/graphic/ugl/VertexBuffer.cpp
        src/graphic/ugl/VertexBuffer.h
)
target_link_libraries(unlogic-calculator PUBLIC unlogic glm::glm ${GTKMM_LIBRARIES})
target_link_directories(unlogic-calculator PUBLIC ${GTKMM_LIBRARY_DIRS})
target_include_directories(unlogic-calculator PUBLIC ${GTKMM_INCLUDE_DIRS})

add_executable(unlogic-test
        test/unlogic.test.cpp
        test/unlogic-graphic.test.cpp
        test/main.test.cpp
)
target_link_libraries(unlogic-test PUBLIC unlogic unlogic-graphic GTest::gtest)
target_include_directories(unlogic-test PUBLIC src)
