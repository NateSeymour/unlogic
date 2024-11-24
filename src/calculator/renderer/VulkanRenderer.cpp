#include "VulkanRenderer.h"
#include <array>
#include "VulkanWindow.h"
#include "graphic/ugl/Vertex.h"

using namespace ui;

void VulkanRenderer::initResources()
{
    this->dev_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());

    this->vertex_buffer_provider_ = std::make_unique<VulkanVertexBufferProvider>(this->window_);

    this->grid_pipeline_ = std::make_unique<VulkanPipeline>(this->window_, ":/shaders/grid.vert.qsb", ":/shaders/grid.frag.qsb");
    this->plot_pipeline_ = std::make_unique<VulkanPipeline>(this->window_, ":/shaders/plot.vert.qsb", ":/shaders/plot.frag.qsb", VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP);
}

void VulkanRenderer::releaseResources()
{
    this->window_->grid.ReleaseBuffer();

    for (auto &plot: this->window_->scene->plots)
    {
        plot.ReleaseBuffer();
    }

    this->vertex_buffer_provider_.reset();

    this->grid_pipeline_.reset();
    this->plot_pipeline_.reset();
}

void VulkanRenderer::startNextFrame()
{
    if (!this->window_->scene)
    {
        this->window_->frameReady();
        return;
    }

    // Set camera for all pipelines
    *this->grid_pipeline_->camera = this->window_->camera;
    *this->plot_pipeline_->camera = this->window_->camera;

    // Begin render pass
    VkCommandBuffer cmd = this->window_->currentCommandBuffer();

    unlogic::Color background = this->window_->scene->background;

    VkClearColorValue clearColor = {
            background.r,
            background.g,
            background.b,
            background.a,
    };
    VkClearDepthStencilValue clearDS = {1.0f, 0};
    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = this->window_->defaultRenderPass();
    rpBeginInfo.framebuffer = this->window_->currentFramebuffer();
    QSize const sz = this->window_->swapChainImageSize();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = 2;
    rpBeginInfo.pClearValues = clearValues;

    this->dev_->vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Set Viewport
    VkViewport viewport{
            .x = 0.f,
            .y = 0.f,
            .width = (float)sz.width(),
            .height = (float)sz.height(),
            .minDepth = 0.f,
            .maxDepth = 1.f,
    };
    this->dev_->vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{
            .offset = {0, 0},
            .extent =
                    {
                            .width = (std::uint32_t)sz.width(),
                            .height = (std::uint32_t)sz.height(),
                    },
    };
    this->dev_->vkCmdSetScissor(cmd, 0, 1, &scissor);

    if (this->window_->scene->draw_grid)
    {
        this->grid_pipeline_->DrawVertexBuffer(this->window_->grid.GetOrCreateVertexBuffer(this->vertex_buffer_provider_.get()));
    }

    for (auto &plot: this->window_->scene->plots)
    {
        this->plot_pipeline_->DrawVertexBuffer(plot.GetOrCreateVertexBuffer(this->vertex_buffer_provider_.get()));
    }

    this->dev_->vkCmdEndRenderPass(cmd);

    this->window_->frameReady();
    this->window_->requestUpdate(); // render continuously, throttled by the presentation rate
}
