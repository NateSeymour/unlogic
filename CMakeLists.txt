cmake_minimum_required(VERSION 3.22)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)

# External Libs
find_package(PkgConfig)
pkg_check_modules(Pistache IMPORTED_TARGET libpistache)

find_package(SFML REQUIRED)
find_package(benchmark REQUIRED)
find_package(cryptopp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(GTest REQUIRED)

find_package(LLVM 17 CONFIG REQUIRED)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Unlogic
add_library(unlogic
        src/TokenViewer.cpp
        src/TokenViewer.h
        src/Node.cpp
        src/Node.h
        src/Parser.cpp
        src/Parser.h
        src/Exception.cpp
        src/Exception.h
        src/Compiler.cpp
        src/Compiler.h)
target_link_libraries(unlogic PUBLIC LLVM-17)
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
)
target_link_libraries(unlogic-test PRIVATE unlogic unlogic-graphic GTest::gtest_main)
target_include_directories(unlogic-test PRIVATE src)

if(Pistache_FOUND)
    add_executable(unlogic-server
            src/server/main.cpp)
    target_link_libraries(unlogic-server PUBLIC unlogic PkgConfig::Pistache cryptopp::cryptopp nlohmann_json::nlohmann_json)
else()
    message(WARNING "Pistache not found. Not configuring unlogic-server module...")
endif()