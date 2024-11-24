#ifndef UNLOGIC_VULKANRENDERER_H
#define UNLOGIC_VULKANRENDERER_H

#include <QVulkanDeviceFunctions>
#include <QVulkanWindowRenderer>
#include "VulkanPipeline.h"
#include "VulkanVertexBuffer.h"

namespace ui
{
    class VulkanWindow;

    class VulkanRenderer : public QVulkanWindowRenderer
    {
        VulkanWindow *window_ = nullptr;

        QVulkanDeviceFunctions *dev_ = nullptr;

        std::unique_ptr<VulkanVertexBufferProvider> vertex_buffer_provider_;

        std::unique_ptr<VulkanPipeline> grid_pipeline_;
        std::unique_ptr<VulkanPipeline> plot_pipeline_;

        std::unique_ptr<unlogic::VertexBuffer> grid_ = nullptr;

    public:
        // Vulkan Commands
        void initResources() override;
        void releaseResources() override;
        void startNextFrame() override;

        VulkanRenderer(VulkanWindow *window) : window_(window) {}
    };
} // namespace ui

#endif // UNLOGIC_VULKANRENDERER_H
