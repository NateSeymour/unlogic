#ifndef UNLOGIC_VULKANPIPELINE_H
#define UNLOGIC_VULKANPIPELINE_H

#include <QVulkanDeviceFunctions>
#include <QVulkanWindow>
#include "graphic/Camera.h"
#include "graphic/VertexBuffer.h"

namespace ui
{
    class VulkanPipeline
    {
        QVulkanWindow *window_;
        QVulkanDeviceFunctions *dev_;

        VkDescriptorPool descriptor_pool_ = nullptr;
        VkDescriptorSetLayout descriptor_set_layout_ = nullptr;
        VkDescriptorSet descriptor_set_ = nullptr;
        VkPipelineLayout pipeline_layout_ = nullptr;
        VkPipeline pipeline_ = nullptr;

        VkBuffer camera_buffer_ = nullptr;
        VkDeviceMemory camera_memory_ = nullptr;

    public:
        unlogic::Camera *camera = nullptr;

        VkShaderModule LoadShader(char const *path);
        VkPipeline NativeHandle();

        void DrawVertexBuffer(unlogic::VertexBuffer *vertex_buffer);

        void Destroy();

        VulkanPipeline(QVulkanWindow *window, char const *vert, char const *frag);
        ~VulkanPipeline();
    };
} // namespace ui

#endif // UNLOGIC_VULKANPIPELINE_H
