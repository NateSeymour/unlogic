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
        std::chrono::time_point<std::chrono::steady_clock> first_flight;
        VulkanBuffer camera_buffer;
        std::vector<VulkanBuffer> plot_buffers;
    };

    class VulkanRenderer : public QVulkanWindowRenderer
    {
        VulkanWindow *window_ = nullptr;

        QVulkanDeviceFunctions *dev_ = nullptr;

        std::unique_ptr<VulkanPipeline> grid_pipeline_;
        std::unique_ptr<VulkanPipeline> plot_pipeline_;

        std::vector<VulkanFrameContext> contexts_;

    public:
        void initResources() override;
        void releaseResources() override;
        void startNextFrame() override;

        VulkanRenderer(VulkanWindow *window) : window_(window) {}
    };
} // namespace ui

#endif // UNLOGIC_VULKANRENDERER_H
