cmake_minimum_required(VERSION 3.22)
project(unlogic-calculator)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(APPLE_CODESIGN_IDENTITY "Identity to codesign with" NONE)

find_package(Vulkan REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets ShaderTools)

qt_standard_project_setup()
qt_add_executable(unlogic-calculator
        src/main.cpp
        src/Window.cpp
        src/Window.h
        src/CompilerController.cpp
        src/CompilerController.h
        src/CompilerWorker.cpp
        src/CompilerWorker.h
        src/CompilationStatus.h
        src/renderer/VulkanRenderer.h
        src/renderer/VulkanWindow.h
        src/renderer/VulkanWindow.cpp
        src/renderer/VulkanInstance.h
        src/renderer/VulkanInstance.cpp
        src/renderer/VulkanRenderer.cpp
        src/renderer/VulkanPipeline.cpp
        src/renderer/VulkanPipeline.h
        src/renderer/VulkanBuffer.cpp
        src/renderer/VulkanBuffer.h
        resource/shaders/plot.frag
        resource/shaders/plot.vert
        resource/shaders/grid.frag
        resource/shaders/grid.vert
        src/widgets/Notebook.cpp
        src/widgets/Notebook.h
)
target_compile_definitions(unlogic-calculator PRIVATE QT_NO_KEYWORDS)
qt_add_shaders(unlogic-calculator "shaders"
        NOGLSL
        NOHSL
        NOMSL
        PREFIX "/shaders"
        BASE resource/shaders
        FILES
        resource/shaders/plot.frag
        resource/shaders/plot.vert
        resource/shaders/grid.frag
        resource/shaders/grid.vert
)
qt_add_resources(unlogic-calculator "fonts"
        PREFIX "/fonts"
        BASE resource
        FILES
        resource/Roboto-Medium.ttf
        resource/SourceCodePro.ttf
)
qt_add_resources(unlogic-calculator "styles"
        PREFIX "/styles"
        BASE resource
        FILES
        resource/stylesheet.qss
)
target_link_libraries(unlogic-calculator PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::ShaderTools Qt6::GuiPrivate unlogic)
set_target_properties(unlogic-calculator PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)

if (MSVC)
    target_compile_options(unlogic-calculator PUBLIC "/Zc:__cplusplus")
endif ()

if (APPLE)
    add_custom_command(TARGET unlogic-calculator POST_BUILD
            COMMAND "mkdir"
            ARGS "-p" "unlogic-calculator.app/Contents/Frameworks"
    )
    add_custom_command(TARGET unlogic-calculator POST_BUILD
            COMMAND "cp"
            ARGS "-f" "/opt/homebrew/lib/libvulkan.1.dylib" "unlogic-calculator.app/Contents/Frameworks/"
    )
    add_custom_command(TARGET unlogic-calculator POST_BUILD
            COMMAND "macdeployqt"
            ARGS "unlogic-calculator.app" "-dmg" "-always-overwrite" "-verbose=2" "-codesign=${APPLE_CODESIGN_IDENTITY}")
endif ()
