#include <QApplication>
#include <QtWidgets/QtWidgets>
#include "renderer/VulkanInstance.h"
#include "Window.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Initialize global Vulkan Instance
    ui::vk_global = new QVulkanInstance;
    ui::vk_global->setLayers({
            "VK_LAYER_GOOGLE_threading",
            "VK_LAYER_LUNARG_parameter_validation",
            "VK_LAYER_LUNARG_object_tracker",
            "VK_LAYER_LUNARG_core_validation",
            "VK_LAYER_LUNARG_image",
            "VK_LAYER_LUNARG_swapchain",
            "VK_LAYER_GOOGLE_unique_objects"
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