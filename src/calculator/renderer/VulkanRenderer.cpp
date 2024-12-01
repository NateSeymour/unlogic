#include "VulkanRenderer.h"
#include <array>
#include "VulkanWindow.h"
#include "graphic/ugl/Vertex.h"

using namespace ui;

void VulkanRenderer::invalidateFrameContexts() noexcept
{
    for (auto &ctx: this->contexts_)
    {
        ctx.valid = false;
    }
}

void VulkanRenderer::initResources()
{
    this->dev_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());

    VkDescriptorSetLayoutBinding descriptor_layout_binding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS,
            .pImmutableSamplers = nullptr,
    };

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &descriptor_layout_binding,
    };

    if (this->dev_->vkCreateDescriptorSetLayout(this->window_->device(), &descriptor_set_layout_info, nullptr, &this->descriptor_set_layout_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout");
    }

    VkDescriptorPoolSize descriptor_pool_size{
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT,
    };

    VkDescriptorPoolCreateInfo descriptor_pool_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT,
            .poolSizeCount = 1,
            .pPoolSizes = &descriptor_pool_size,
    };

    if (this->dev_->vkCreateDescriptorPool(this->window_->device(), &descriptor_pool_info, nullptr, &this->descriptor_pool_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool");
    }

    CreateVulkanPipelineInfo grid_info{
            .window = this->window_,
            .descriptor_set_layout_ = this->descriptor_set_layout_,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .vert_shader = ":/shaders/grid.vert.qsb",
            .frag_shader = ":/shaders/grid.frag.qsb",
    };
    this->grid_pipeline_ = std::make_unique<VulkanPipeline>(grid_info);

    CreateVulkanPipelineInfo plot_info{
            .window = this->window_,
            .descriptor_set_layout_ = this->descriptor_set_layout_,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
            .vert_shader = ":/shaders/plot.vert.qsb",
            .frag_shader = ":/shaders/plot.frag.qsb",
    };
    this->plot_pipeline_ = std::make_unique<VulkanPipeline>(plot_info);

    for (int i = 0; i < this->window_->concurrentFrameCount(); i++)
    {
        this->contexts_[i].valid = false;
        this->contexts_[i].camera_buffer = VulkanBuffer::Create<unlogic::Camera>(this->window_, BufferType::Uniform);
        this->contexts_[i].plot_buffers.clear();

        VkDescriptorSetAllocateInfo descriptor_set_allocate_info{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = this->descriptor_pool_,
                .descriptorSetCount = 1,
                .pSetLayouts = &this->descriptor_set_layout_,
        };

        if (this->dev_->vkAllocateDescriptorSets(this->window_->device(), &descriptor_set_allocate_info, &this->contexts_[i].descriptor_set) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor set");
        }

        VkDescriptorBufferInfo descriptor_buffer_info{
                .buffer = this->contexts_[i].camera_buffer->GetNativeHandle(),
                .offset = 0,
                .range = this->contexts_[i].camera_buffer->GetSize(),
        };

        VkWriteDescriptorSet write_descriptor_set{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = this->contexts_[i].descriptor_set,
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pBufferInfo = &descriptor_buffer_info,
        };

        this->dev_->vkUpdateDescriptorSets(this->window_->device(), 1, &write_descriptor_set, 0, nullptr);
    }
}

void VulkanRenderer::releaseResources()
{
    for (auto &ctx: this->contexts_)
    {
        ctx.valid = false;
        ctx.scene.reset();
        ctx.camera_buffer.reset();
        ctx.grid_buffer.reset();
        ctx.plot_buffers.clear();
    }

    this->dev_->vkDestroyDescriptorPool(this->window_->device(), this->descriptor_pool_, nullptr);
    this->dev_->vkDestroyDescriptorSetLayout(this->window_->device(), this->descriptor_set_layout_, nullptr);

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

    VkClearColorValue clearColor = {1.f, 1.f, 1.f, 1.f};
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
    ctx.camera_buffer->Write<unlogic::Camera>(&this->window_->camera);

    if (!ctx.valid)
    {
        ctx.plot_buffers.clear();
        ctx.scene = this->window_->scene;

        auto const &grid_vertices = this->window_->grid.GetVertices();
        ctx.grid_buffer = VulkanBuffer::Create<unlogic::Vertex>(this->window_, BufferType::Vertex, grid_vertices.size());
        ctx.grid_buffer->Write<unlogic::Vertex>(grid_vertices.data(), grid_vertices.size());

        for (auto const &plot: ctx.scene->plots)
        {
            auto const &plot_vertices = plot.GetVertices();
            auto plot_buffer = VulkanBuffer::Create<unlogic::Vertex>(this->window_, BufferType::Vertex, plot_vertices.size());
            plot_buffer->Write<unlogic::Vertex>(plot_vertices.data(), plot_vertices.size());
        }

        ctx.valid = true;
    }

    this->grid_pipeline_->Bind(cmd);
    this->grid_pipeline_->BindDescriptorSets(cmd, &ctx.descriptor_set, 1);

    ctx.grid_buffer->Bind(cmd);
    this->dev_->vkCmdDraw(cmd, ctx.grid_buffer->GetCount(), 1, 0, 0);

    this->plot_pipeline_->Bind(cmd);
    this->plot_pipeline_->BindDescriptorSets(cmd, &ctx.descriptor_set, 1);

    for (auto &plot_buffer: ctx.plot_buffers)
    {
        plot_buffer->Bind(cmd);
        this->dev_->vkCmdDraw(cmd, plot_buffer->GetCount(), 1, 0, 0);
    }

    this->dev_->vkCmdEndRenderPass(cmd);

    this->window_->frameReady();
    this->window_->requestUpdate(); // render continuously, throttled by the presentation rate
}
