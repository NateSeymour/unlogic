#include <QApplication>
#include <QtWidgets/QtWidgets>
#include "Window.h"
#include "compiler/Compiler.h"
#include "renderer/VulkanInstance.h"

int main(int argc, char **argv)
{
    unlogic::Compiler::InitializeGlobalCompilerRuntime();

    QApplication app(argc, argv);

    // Initialize global Vulkan Instance
    ui::vk_global = new QVulkanInstance;
    ui::vk_global->setLayers({
            "VK_LAYER_KHRONOS_validation",
    });

    ui::vk_global->setExtensions({
#if !NDEBUG
            "VK_EXT_memory_budget",
#endif
            "VK_KHR_portability_subset",
            "VK_KHR_surface",
            "VK_KHR_swapchain",
    });

    if (!ui::vk_global->create())
    {
        return 1;
    }

    QFontDatabase::addApplicationFont(":/fonts/SourceCodePro.ttf");

    // Create Window
    ui::Window window;
    window.resize(1000, 500);
    window.show();

    return app.exec();
}
