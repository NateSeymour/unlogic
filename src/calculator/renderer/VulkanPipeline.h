#ifndef UNLOGIC_VULKANPIPELINE_H
#define UNLOGIC_VULKANPIPELINE_H

#include <QVulkanDeviceFunctions>
#include <QVulkanWindow>
#include "graphic/Camera.h"

namespace ui
{
    class VulkanPipeline
    {
        QVulkanWindow *window_;
        QVulkanDeviceFunctions *dev_;

        VkDescriptorPool descriptor_pool_ = nullptr;
        VkDescriptorSetLayout descriptor_set_layout_ = nullptr;
        VkPipelineLayout pipeline_layout_ = nullptr;
        VkPipeline pipeline_ = nullptr;

        VkBuffer camera_buffer_ = nullptr;
        VkDeviceMemory camera_memory_ = nullptr;

    public:
        unlogic::Camera *camera = nullptr;

        VkShaderModule loadShader(char const *path);
        VkPipeline NativeHandle();

        void Destroy();

        VulkanPipeline(QVulkanWindow *window, char const *vert, char const *frag);
        ~VulkanPipeline();
    };
} // namespace ui

#endif // UNLOGIC_VULKANPIPELINE_H
