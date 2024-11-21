#include <array>
#include <QFile>
#include <rhi/qshader.h>
#include "VulkanRenderer.h"
#include "VulkanWindow.h"
#include "graphic/ugl/Vertex.h"

using namespace ui;

void VulkanRenderer::createStandardPipeline(VkPipeline &pipeline, VkPipelineLayout &layout, const char *vert_shader_path, const char *frag_shader_path)
{
    VkShaderModule vert_shader = this->loadShader(vert_shader_path);
    VkShaderModule frag_shader = this->loadShader(frag_shader_path);

    VkPipelineShaderStageCreateInfo vert_shader_stage_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vert_shader,
            .pName = "main",
    };

    VkPipelineShaderStageCreateInfo frag_shader_stage_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = frag_shader,
            .pName = "main",
    };

    std::array shader_stages = {vert_shader_stage_info, frag_shader_stage_info};

    VkVertexInputBindingDescription vertex_input_binding_description {
            .binding = 0,
            .stride = sizeof(unlogic::Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    std::array vertex_attribute_descriptions = {
            VkVertexInputAttributeDescription {
                    .location = 0,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32_SFLOAT,
                    .offset = offsetof(unlogic::Vertex, position),
            },
            VkVertexInputAttributeDescription {
                    .location = 1,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                    .offset = offsetof(unlogic::Vertex, color),
            },
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &vertex_input_binding_description,
            .vertexAttributeDescriptionCount = vertex_attribute_descriptions.size(),
            .pVertexAttributeDescriptions = vertex_attribute_descriptions.data(),
    };

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_TRUE,
    };

    std::array dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = dynamic_states.size(),
            .pDynamicStates = dynamic_states.data(),
    };

    VkPipelineViewportStateCreateInfo viewport_state_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.f,
            .depthBiasClamp = 0.f,
            .depthBiasSlopeFactor = 0.f,
            .lineWidth = 1.f,
    };

    VkPipelineMultisampleStateCreateInfo multisample_state_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pSetLayouts = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };

    if(this->dev_->vkCreatePipelineLayout(this->window_->device(), &pipeline_layout_info, nullptr, &layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipeline_info {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = shader_stages.size(),
            .pStages = shader_stages.data(),
            .pVertexInputState = &vertex_input_info,
            .pInputAssemblyState = &input_assembly_info,
            .pViewportState = &viewport_state_info,
            .pRasterizationState = &rasterization_state_info,
            .pMultisampleState = &multisample_state_info,
            .pDepthStencilState = nullptr,
            .pColorBlendState = nullptr,
            .pDynamicState = &dynamic_state_info,
            .layout = layout,
            .renderPass = this->window_->defaultRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
    };

    if(this->dev_->vkCreateGraphicsPipelines(this->window_->device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline");
    }

    this->dev_->vkDestroyShaderModule(this->window_->device(), vert_shader, nullptr);
    this->dev_->vkDestroyShaderModule(this->window_->device(), frag_shader, nullptr);
}

void VulkanRenderer::initResources()
{
    this->dev_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());

    this->createStandardPipeline(this->plot_pipeline_, this->plot_pipeline_layout_, ":/shaders/plot.vert.qsb", ":/shaders/plot.frag.qsb");
    this->createStandardPipeline(this->graph_pipeline_, this->graph_pipeline_layout_, ":/shaders/graph.vert.qsb", ":/shaders/graph.frag.qsb");

    std::array corners = {
        glm::vec2 { -1.f, -1.f },
        glm::vec2 { 1.f, -1.f },
        glm::vec2 { 1.f, 1.f },
        glm::vec2 { -1.f, 1.f },
    };
    this->gridlines_ = this->window_->scene->Rect(corners, unlogic::Color::Green);
}

void VulkanRenderer::releaseResources()
{
    this->dev_->vkDestroyPipeline(this->window_->device(), this->plot_pipeline_, nullptr);
    this->dev_->vkDestroyPipelineLayout(this->window_->device(), this->plot_pipeline_layout_, nullptr);

    this->dev_->vkDestroyPipeline(this->window_->device(), this->graph_pipeline_, nullptr);
    this->dev_->vkDestroyPipelineLayout(this->window_->device(), this->graph_pipeline_layout_, nullptr);
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
        this->drawVertexBuffer(this->gridlines_.get(), this->graph_pipeline_);
    }

    this->dev_->vkCmdEndRenderPass(cmd);

    this->window_->frameReady();
    this->window_->requestUpdate(); // render continuously, throttled by the presentation rate
}

VkShaderModule VulkanRenderer::loadShader(std::string_view path)
{
    QFile shader_file(path.data());
    if(!shader_file.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error("failed to open vertex shader");
    }

    QByteArray shader_data = shader_file.readAll();
    QShader shader = QShader::fromSerialized(shader_data);
    QShaderKey shader_key = std::invoke([&]() {
        for(auto const &key : shader.availableShaders())
        {
            if(key.source() == QShader::Source::SpirvShader)
            {
                return key;
            }
        }

        throw std::runtime_error("shader does not contain SPIR-V source");
    });
    QShaderCode shader_code = shader.shader(shader_key);

    VkShaderModuleCreateInfo create_info {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = (std::size_t)shader_code.shader().size(),
            .pCode = reinterpret_cast<const uint32_t*>(shader_code.shader().constData()),
    };

    VkShaderModule module;
    if(this->dev_->vkCreateShaderModule(this->window_->device(), &create_info, nullptr, &module) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to compile shader module");
    }

    return module;
}

void VulkanRenderer::drawVertexBuffer(unlogic::VertexBuffer *vertex_buffer, VkPipeline pipeline)
{
    VkCommandBuffer cmd = this->window_->currentCommandBuffer();
    this->dev_->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    std::array buffers = { static_cast<VkBuffer>(vertex_buffer->GetNativeHandle()) };
    constexpr std::array<VkDeviceSize, 1> offsets = { 0 };
    this->dev_->vkCmdBindVertexBuffers(cmd, 0, 1, buffers.data(), offsets.data());

    this->dev_->vkCmdDraw(cmd, vertex_buffer->GetVertexCount(), 1, 0, 0);
}
