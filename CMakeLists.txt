cmake_minimum_required(VERSION 3.22)
project(unlogic)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# External Libs
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui ShaderTools)

find_package(LLVM 18.1.3 CONFIG REQUIRED)
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

llvm_map_components_to_libnames(llvm_libs support core irreader native target orcjit)

include(FetchContent)
FetchContent_Declare(
        buffalo
        GIT_REPOSITORY https://github.com/NateSeymour/buffalo.git
        GIT_TAG v1.1.1
)
FetchContent_MakeAvailable(buffalo)

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
        src/compiler/std/StandardLibrary.cpp
        src/compiler/std/StandardLibrary.h
        src/compiler/Library.cpp
        src/compiler/Library.h
        src/util/io.cpp
        src/util/io.h
        src/Error.h
        src/parser/Node.cpp
        src/parser/Node.h
        src/parser/Parser.cpp
        src/parser/Parser.h
        src/compiler/transformer/IRGenerator.cpp
        src/compiler/transformer/IRGenerator.h
        src/compiler/std/RuntimeLibrary.cpp
        src/compiler/std/RuntimeLibrary.h
        src/compiler/transformer/IRGenerationContext.h
        src/graphic/Scene.cpp
        src/graphic/Scene.h
        src/graphic/Color.cpp
        src/graphic/Color.h
        src/graphic/ugl/Vertex.h
        src/graphic/shape/Plot.h
        src/graphic/shape/Plot.cpp
        src/graphic/shape/Shape.cpp
        src/graphic/shape/Shape.h
        src/graphic/VertexBuffer.h
        src/graphic/Camera.h
        src/graphic/shape/Rect.cpp
        src/graphic/shape/Rect.h
        src/util/overload.h
)
target_link_libraries(unlogic PUBLIC ${llvm_libs} glm::glm buffalo)
target_include_directories(unlogic PUBLIC ${LLVM_INCLUDE_DIRS} src)

# CALCULATOR
qt_standard_project_setup()
qt_add_executable(unlogic-calculator
        src/calculator/main.cpp
        src/calculator/Window.cpp
        src/calculator/Window.h
        src/calculator/CompilerController.cpp
        src/calculator/CompilerController.h
        src/calculator/CompilerWorker.cpp
        src/calculator/CompilerWorker.h
        src/calculator/CompilationStatus.h
        src/calculator/renderer/VulkanRenderer.h
        src/calculator/renderer/VulkanWindow.h
        src/calculator/renderer/VulkanWindow.cpp
        src/calculator/renderer/VulkanInstance.h
        src/calculator/renderer/VulkanInstance.cpp
        src/calculator/renderer/VulkanRenderer.cpp
        src/calculator/renderer/VulkanVertexBuffer.h
        src/calculator/resource/shaders/plot.frag
        src/calculator/resource/shaders/plot.vert
        src/calculator/renderer/VulkanPipeline.cpp
        src/calculator/renderer/VulkanPipeline.h
        src/calculator/renderer/VulkanVertexBuffer.cpp
)
target_compile_definitions(unlogic-calculator PRIVATE QT_NO_KEYWORDS)
qt_add_shaders(unlogic-calculator "shaders"
        NOGLSL
        NOHSL
        NOMSL
        PREFIX "/shaders"
        BASE src/calculator/resource/shaders
        FILES
        src/calculator/resource/shaders/plot.frag
        src/calculator/resource/shaders/plot.vert
        src/calculator/resource/shaders/grid.frag
        src/calculator/resource/shaders/grid.vert
)
qt_add_resources(unlogic-calculator "fonts"
        PREFIX "/fonts"
        BASE src/calculator/resource
        FILES
        src/calculator/resource/Roboto-Medium.ttf
        src/calculator/resource/SourceCodePro.ttf
)
qt_add_resources(unlogic-calculator "styles"
        PREFIX "/styles"
        BASE src/calculator/resource
        FILES
        src/calculator/resource/stylesheet.qss
)
target_link_libraries(unlogic-calculator PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::ShaderTools Qt6::GuiPrivate unlogic)
set_target_properties(unlogic-calculator PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)

# TESTS
add_executable(unlogic-test
        test/unlogic.test.cpp
        test/main.test.cpp
)
target_link_libraries(unlogic-test PUBLIC GTest::gtest unlogic)
