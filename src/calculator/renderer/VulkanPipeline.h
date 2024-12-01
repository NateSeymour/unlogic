#ifndef UNLOGIC_VULKANPIPELINE_H
#define UNLOGIC_VULKANPIPELINE_H

#include <QVulkanDeviceFunctions>
#include <QVulkanWindow>
#include "graphic/Camera.h"

namespace ui
{
    struct CreateVulkanPipelineInfo
    {
        QVulkanWindow *window = nullptr;

        VkDescriptorSetLayout descriptor_set_layout_ = nullptr;

        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        char const *vert_shader = nullptr;
        char const *frag_shader = nullptr;
    };

    class VulkanPipeline
    {
        QVulkanWindow *window_;
        QVulkanDeviceFunctions *dev_;

        VkPipelineLayout pipeline_layout_ = nullptr;
        VkPipeline pipeline_ = nullptr;

    protected:
        VkShaderModule LoadShader(char const *path);

    public:
        [[nodiscard]] VkPipelineLayout GetLayout() const noexcept;

        void Bind(VkCommandBuffer cmd);
        void BindDescriptorSets(VkCommandBuffer cmd, VkDescriptorSet const *descriptor_set, std::size_t count);
        void Destroy();

        VulkanPipeline(CreateVulkanPipelineInfo const &info);
        virtual ~VulkanPipeline();
    };
} // namespace ui

#endif // UNLOGIC_VULKANPIPELINE_H
