#ifndef UNLOGIC_VULKANRENDERER_H
#define UNLOGIC_VULKANRENDERER_H

#include <QVulkanWindowRenderer>
#include <QVulkanDeviceFunctions>
#include "graphic/VertexBuffer.h"

namespace ui
{
    class VulkanWindow;

    class VulkanRenderer : public QVulkanWindowRenderer
    {
        VulkanWindow *window_ = nullptr;

        VkPipelineLayout graph_pipeline_layout_;
        VkPipeline graph_pipeline_;

        VkPipelineLayout plot_pipeline_layout_;
        VkPipeline plot_pipeline_;

        QVulkanDeviceFunctions *dev_ = nullptr;

        std::unique_ptr<unlogic::VertexBuffer> gridlines_ = nullptr;

    public:
        // Vulkan Utils
        VkShaderModule loadShader(std::string_view path);
        void createStandardPipeline(VkPipeline &pipeline, VkPipelineLayout &layout, char const *vert_shader_path, char const *frag_shader_path);

        // Vulkan Commands
        void initResources() override;
        void releaseResources() override;
        void startNextFrame() override;

        // Draw Commands
        void drawVertexBuffer(unlogic::VertexBuffer *vertex_buffer, VkPipeline pipeline);

        VulkanRenderer(VulkanWindow *window) : window_(window) {}
    };
}

#endif //UNLOGIC_VULKANRENDERER_H
