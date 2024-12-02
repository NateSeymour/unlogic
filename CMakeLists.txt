cmake_minimum_required(VERSION 3.22)
project(unlogic-parent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CONFIGURE_CALCULATOR "Configure unlogic-calculator" ON)

add_subdirectory(unlogic)

if (${CONFIGURE_CALCULATOR})
    add_subdirectory(calculator)
endif ()