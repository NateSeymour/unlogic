#include <QApplication>
#include <QtWidgets/QtWidgets>
#include "renderer/VulkanInstance.h"
#include "Window.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Initialize global Vulkan Instance
    ui::vk_global = new QVulkanInstance;
    ui::vk_global->setLayers({});

    ui::vk_global->setExtensions({
#if !NDEBUG
         "VK_LAYER_KHRONOS_validation",
#endif
         "VK_KHR_portability_subset",
         "VK_KHR_surface",
         "VK_KHR_swapchain",
    });

    if(!ui::vk_global->create())
    {
        return 1;
    }

    // Create Window
    ui::Window window;
    window.resize(1000, 500);
    window.show();

    return app.exec();
}