#include "VulkanRenderer.h"
#include <array>
#include "VulkanWindow.h"
#include "graphic/ugl/Vertex.h"

using namespace ui;

void VulkanRenderer::initResources()
{
    this->dev_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());

    CreateVulkanPipelineInfo grid_info{
            .window = this->window_,
            .vert_shader = ":/shaders/grid.vert.qsb",
            .frag_shader = ":/shaders/grid.frag.qsb",
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    };
    this->grid_pipeline_ = std::make_unique<VulkanPipeline>(grid_info);

    CreateVulkanPipelineInfo plot_info{
            .window = this->window_,
            .vert_shader = ":/shaders/plot.vert.qsb",
            .frag_shader = ":/shaders/plot.frag.qsb",
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    };
    this->plot_pipeline_ = std::make_unique<VulkanPipeline>(plot_info);

    for (int i = 0; i < this->window_->concurrentFrameCount(); i++)
    {
        this->contexts_.push_back({
                .camera_buffer = VulkanBuffer::Create<unlogic::Camera>(this->window_, BufferType::Uniform),
        });
    }
}

void VulkanRenderer::releaseResources()
{
    this->contexts_.clear();
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

    // Begin render pass
    VkCommandBuffer cmd = this->window_->currentCommandBuffer();

    VkClearColorValue clearColor = {0.f, 0.f, 0.f, 1.f};
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

    auto &ctx = this->contexts_[this->window_->currentFrame()];

    ctx.camera_buffer.Write<unlogic::Camera>(&this->window_->camera);

    this->grid_pipeline_->Bind(cmd);

    this->plot_pipeline_->Bind(cmd);

    this->dev_->vkCmdEndRenderPass(cmd);

    this->window_->frameReady();
    this->window_->requestUpdate(); // render continuously, throttled by the presentation rate
}
