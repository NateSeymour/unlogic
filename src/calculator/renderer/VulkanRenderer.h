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

        VkPipelineLayout pipeline_layout_;
        VkPipeline pipeline_;

        QVulkanDeviceFunctions *dev_ = nullptr;

    public:
        VkShaderModule loadShader(std::string_view path);

        void initResources() override;
        void releaseResources() override;
        void startNextFrame() override;

        VulkanRenderer(VulkanWindow *window) : window_(window) {}
    };
}

#endif //UNLOGIC_VULKANRENDERER_H
