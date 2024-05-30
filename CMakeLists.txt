cmake_minimum_required(VERSION 3.22)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)

# External Libs
find_package(PkgConfig)
pkg_check_modules(Pistache IMPORTED_TARGET libpistache)

find_package(SFML REQUIRED)
find_package(cryptopp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(GTest REQUIRED)

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

add_executable(unlogic-test
        test/function.test.cpp
        test/GraphRenderer.cpp)
target_link_libraries(unlogic-test PRIVATE unlogic GTest::gtest_main)
target_include_directories(unlogic-test PRIVATE src)

if(Pistache_FOUND)
    add_executable(unlogic-server
            src/server/main.cpp)
    target_link_libraries(unlogic-server PUBLIC unlogic PkgConfig::Pistache cryptopp::cryptopp nlohmann_json::nlohmann_json)
else()
    message(WARNING "Pistache not found. Not configuring unlogic-server module...")
endif()