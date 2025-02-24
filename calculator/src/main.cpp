#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <iostream>
#include "Window.h"
#include "renderer/VulkanInstance.h"

int main(int argc, char **argv)
{
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

    // Fonts
    QFontDatabase::addApplicationFont(":/fonts/SourceCodePro.ttf");

    // Styles
    QFile stylesheet(":/styles/stylesheet.qss");
    if (stylesheet.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        app.setStyleSheet(stylesheet.readAll());
        stylesheet.close();
    }
    else
    {
        std::cerr << "[MAIN] Failed to load styles!" << std::endl;
    }

    // Create Window
    ui::Window window;
    window.resize(1000, 500);
    window.show();

    return app.exec();
}
