#ifndef UNLOGIC_VULKANPIPELINE_H
#define UNLOGIC_VULKANPIPELINE_H

#include <QVulkanWindow>
#include <QVulkanDeviceFunctions>

namespace ui
{
    class VulkanPipeline
    {
        QVulkanWindow *window_;
        QVulkanDeviceFunctions *dev_;

        VkDescriptorSetLayout descriptor_set_layout_ = nullptr;
        VkPipelineLayout pipeline_layout_ = nullptr;
        VkPipeline pipeline_ = nullptr;

    public:
        VkShaderModule loadShader(char const *path);
        VkPipeline NativeHandle();

        void Destroy();

        VulkanPipeline(QVulkanWindow *window, char const *vert, char const *frag);
        ~VulkanPipeline();
    };
}

#endif //UNLOGIC_VULKANPIPELINE_H
