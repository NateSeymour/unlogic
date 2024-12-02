#ifndef UNLOGIC_VULKANRENDERER_H
#define UNLOGIC_VULKANRENDERER_H

#include <QVulkanDeviceFunctions>
#include <QVulkanWindowRenderer>
#include <chrono>
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"
#include "graphic/Scene.h"

namespace ui
{
    class VulkanWindow;

    struct VulkanFrameContext
    {
        std::atomic<bool> valid = false;

        VkDescriptorSet descriptor_set = nullptr;

        std::shared_ptr<unlogic::Scene> scene;
        std::unique_ptr<VulkanBuffer> camera_buffer;
        std::unique_ptr<VulkanBuffer> grid_buffer;
        std::vector<std::unique_ptr<VulkanBuffer>> plot_buffers;
    };

    class VulkanRenderer : public QVulkanWindowRenderer
    {
        VulkanWindow *window_ = nullptr;

        QVulkanDeviceFunctions *dev_ = nullptr;

        VkDescriptorSetLayout descriptor_set_layout_;
        VkDescriptorPool descriptor_pool_;

        std::unique_ptr<VulkanPipeline> grid_pipeline_;
        std::unique_ptr<VulkanPipeline> plot_pipeline_;

        std::array<VulkanFrameContext, QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT> contexts_;

    public:
        void invalidateFrameContexts() noexcept;

        void initResources() override;
        void releaseResources() override;
        void startNextFrame() override;

        VulkanRenderer(VulkanWindow *window) : window_(window) {}
    };
} // namespace ui

#endif // UNLOGIC_VULKANRENDERER_H
