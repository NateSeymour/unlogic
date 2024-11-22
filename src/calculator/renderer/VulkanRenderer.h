#ifndef UNLOGIC_VULKANRENDERER_H
#define UNLOGIC_VULKANRENDERER_H

#include <QVulkanWindowRenderer>
#include <QVulkanDeviceFunctions>
#include "VulkanPipeline.h"
#include "graphic/VertexBuffer.h"

namespace ui
{
    class VulkanWindow;

    class VulkanRenderer : public QVulkanWindowRenderer
    {
        VulkanWindow *window_ = nullptr;

        QVulkanDeviceFunctions *dev_ = nullptr;

        std::unique_ptr<VulkanPipeline> grid_pipeline_;
        std::unique_ptr<VulkanPipeline> plot_pipeline_;

        std::unique_ptr<unlogic::VertexBuffer> grid_ = nullptr;

    public:
        // Vulkan Commands
        void initResources() override;
        void releaseResources() override;
        void startNextFrame() override;

        // Draw Commands
        void drawVertexBuffer(unlogic::VertexBuffer *vertex_buffer, VulkanPipeline &pipeline);

        VulkanRenderer(VulkanWindow *window) : window_(window) {}
    };
}

#endif //UNLOGIC_VULKANRENDERER_H
