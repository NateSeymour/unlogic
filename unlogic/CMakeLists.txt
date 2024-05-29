cmake_minimum_required(VERSION 3.27)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)

# Testing
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# External Libs
find_package(SFML REQUIRED)

# Unlogic
add_library(unlogic
        src/TokenViewer.cpp
        src/TokenViewer.h
        src/Node.cpp
        src/Node.h
        src/MathFunction.cpp
        src/MathFunction.h
        src/GraphRenderer.cpp
        src/GraphRenderer.h)
target_link_libraries(unlogic PUBLIC sfml::sfml)

add_executable(unlogic-cli src/main.cpp)
target_link_libraries(unlogic-cli PUBLIC unlogic)