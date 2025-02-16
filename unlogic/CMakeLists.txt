cmake_minimum_required(VERSION 3.22)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# External Libs
include(FetchContent)
set(BUFFALO_ENABLE_TESTS OFF)
FetchContent_Declare(
        buffalo
        GIT_REPOSITORY https://github.com/NateSeymour/buffalo.git
        GIT_TAG v1.1.2
)
FetchContent_MakeAvailable(buffalo)

set(CITY_ENABLE_TESTS OFF)
FetchContent_Declare(
        city
        GIT_REPOSITORY https://github.com/NateSeymour/city.git
        GIT_TAG v0.3.6
)
FetchContent_MakeAvailable(city)

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
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(GTEST)

# UNLOGIC
add_library(unlogic STATIC
        src/parser/Node.h
        src/compiler/Compiler.cpp
        src/compiler/Compiler.h
        src/util/io.cpp
        src/util/io.h
        src/Error.h
        src/parser/Node.cpp
        src/parser/Node.h
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/compiler/transformer/IRGenerator.cpp
        src/compiler/transformer/IRGenerator.h
        src/compiler/transformer/debug/DebugPrinter.cpp
        src/compiler/transformer/debug/DebugPrinter.h
        src/compiler/transformer/IRGenerationContext.h
        src/graphic/Scene.cpp
        src/graphic/Scene.h
        src/graphic/Color.cpp
        src/graphic/Color.h
        src/graphic/Vertex.h
        src/graphic/shape/Plot.h
        src/graphic/shape/Plot.cpp
        src/graphic/shape/Shape.cpp
        src/graphic/shape/Shape.h
        src/graphic/Camera.h
        src/graphic/shape/Rect.cpp
        src/graphic/shape/Rect.h
        src/util/overload.h
        src/compiler/Scope.h
)
target_link_libraries(unlogic PUBLIC city glm::glm buffalo)
target_include_directories(unlogic PUBLIC src)

# TESTS
add_executable(unlogic-test
        test/unlogic.test.cpp
        test/main.test.cpp
)
target_link_libraries(unlogic-test PUBLIC GTest::gtest unlogic)
