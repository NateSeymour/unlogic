cmake_minimum_required(VERSION 3.28)
project(unlogic)

set(CMAKE_CXX_STANDARD 20)

add_executable(unlogic src/main.cpp
        src/TokenViewer.cpp
        src/TokenViewer.h)
