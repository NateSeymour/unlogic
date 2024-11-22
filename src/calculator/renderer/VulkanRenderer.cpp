#include <array>
#include "VulkanRenderer.h"
#include "VulkanWindow.h"
#include "graphic/ugl/Vertex.h"

using namespace ui;

void VulkanRenderer::initResources()
{
    this->dev_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());

    this->grid_pipeline_ = std::make_unique<VulkanPipeline>(this->window_, ":/shaders/grid.vert.qsb", ":/shaders/grid.frag.qsb");
    this->plot_pipeline_ = std::make_unique<VulkanPipeline>(this->window_, ":/shaders/plot.vert.qsb", ":/shaders/plot.frag.qsb");

    std::array corners = {
        glm::vec2 { -1.f, -1.f },
        glm::vec2 { 1.f, -1.f },
        glm::vec2 { 1.f, 1.f },
        glm::vec2 { -1.f, 1.f },
    };

    this->grid_ = this->window_->scene->Rect(corners, unlogic::Color::Green);
}

void VulkanRenderer::releaseResources()
{
    this->grid_->Release();

    this->grid_pipeline_.reset();
    this->plot_pipeline_.reset();
}

void VulkanRenderer::startNextFrame()
{
    if(!this->window_->scene)
    {
        this->window_->frameReady();
        return;
    }

    VkCommandBuffer cmd = this->window_->currentCommandBuffer();

    unlogic::Color background = this->window_->scene->background;

    VkClearColorValue clearColor = {
        background.r,
        background.g,
        background.b,
        background.a,
    };
    VkClearDepthStencilValue clearDS = { 1.0f, 0 };
    VkClearValue clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass = this->window_->defaultRenderPass();
    rpBeginInfo.framebuffer = this->window_->currentFramebuffer();
    const QSize sz = this->window_->swapChainImageSize();
    rpBeginInfo.renderArea.extent.width = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount = 2;
    rpBeginInfo.pClearValues = clearValues;

    this->dev_->vkCmdBeginRenderPass(cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Set Viewport
    VkViewport viewport {
            .x = 0.f,
            .y = 0.f,
            .width = (float)sz.width(),
            .height = (float)sz.height(),
            .minDepth = 0.f,
            .maxDepth = 1.f,
    };
    this->dev_->vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor {
            .offset = { 0, 0 },
            .extent = {
                    .width = (std::uint32_t)sz.width(),
                    .height = (std::uint32_t)sz.height(),
            },
    };
    this->dev_->vkCmdSetScissor(cmd, 0, 1, &scissor);

    // Commence drawing scene
    if(this->window_->scene->draw_gridlines)
    {
        this->drawVertexBuffer(this->grid_.get(), *this->grid_pipeline_);
    }

    this->dev_->vkCmdEndRenderPass(cmd);

    this->window_->frameReady();
    this->window_->requestUpdate(); // render continuously, throttled by the presentation rate
}

void VulkanRenderer::drawVertexBuffer(unlogic::VertexBuffer *vertex_buffer, VulkanPipeline &pipeline)
{
    VkCommandBuffer cmd = this->window_->currentCommandBuffer();
    this->dev_->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.NativeHandle());

    std::array buffers = { static_cast<VkBuffer>(vertex_buffer->GetNativeHandle()) };
    constexpr std::array<VkDeviceSize, 1> offsets = { 0 };
    this->dev_->vkCmdBindVertexBuffers(cmd, 0, 1, buffers.data(), offsets.data());

    this->dev_->vkCmdDraw(cmd, vertex_buffer->GetVertexCount(), 1, 0, 0);
}
