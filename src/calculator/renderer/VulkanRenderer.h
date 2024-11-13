#ifndef UNLOGIC_VULKANRENDERER_H
#define UNLOGIC_VULKANRENDERER_H

#include <QVulkanWindowRenderer>
#include <QVulkanDeviceFunctions>

namespace ui
{
    class VulkanWindow;

    class VulkanRenderer : public QVulkanWindowRenderer
    {
        VulkanWindow *window_ = nullptr;
        QVulkanDeviceFunctions *device_functions_ = nullptr;

    public:
        void initResources() override;
        void startNextFrame() override;

        VulkanRenderer(VulkanWindow *window) : window_(window) {}
    };
}

#endif //UNLOGIC_VULKANRENDERER_H
