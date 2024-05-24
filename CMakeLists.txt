cmake_minimum_required(VERSION 3.27)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)

add_executable(unlogic src/main.cpp
        src/TokenViewer.cpp
        src/TokenViewer.h
        src/Node.cpp
        src/Node.h
        src/MathFunction.cpp
        src/MathFunction.h)
