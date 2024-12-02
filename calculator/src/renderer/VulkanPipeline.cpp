#include "VulkanPipeline.h"
#include <QFile>
#include <rhi/qshader.h>
#include "graphic/Vertex.h"

VkShaderModule ui::VulkanPipeline::LoadShader(char const *path)
{
    QFile shader_file(path);
    if (!shader_file.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error("failed to open vertex shader");
    }

    QByteArray shader_data = shader_file.readAll();
    QShader shader = QShader::fromSerialized(shader_data);
    QShaderKey shader_key = std::invoke(
            [&]()
            {
                for (auto const &key: shader.availableShaders())
                {
                    if (key.source() == QShader::Source::SpirvShader)
                    {
                        return key;
                    }
                }

                throw std::runtime_error("shader does not contain SPIR-V source");
            });
    QShaderCode shader_code = shader.shader(shader_key);

    VkShaderModuleCreateInfo create_info{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = (std::size_t)shader_code.shader().size(),
            .pCode = reinterpret_cast<uint32_t const *>(shader_code.shader().constData()),
    };

    VkShaderModule module;
    if (this->dev_->vkCreateShaderModule(this->window_->device(), &create_info, nullptr, &module) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to compile shader module");
    }

    return module;
}

VkPipelineLayout ui::VulkanPipeline::GetLayout() const noexcept
{
    return this->pipeline_layout_;
}

void ui::VulkanPipeline::Bind(VkCommandBuffer cmd)
{
    this->dev_->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline_);
}

void ui::VulkanPipeline::BindDescriptorSets(VkCommandBuffer cmd, VkDescriptorSet const *descriptor_set, std::size_t count = 1)
{
    this->dev_->vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline_layout_, 0, count, descriptor_set, 0, nullptr);
}

ui::VulkanPipeline::VulkanPipeline(CreateVulkanPipelineInfo const &info) : window_(info.window)
{
    this->dev_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());

    VkShaderModule vert_shader = this->LoadShader(info.vert_shader);
    VkShaderModule frag_shader = this->LoadShader(info.frag_shader);

    VkPipelineShaderStageCreateInfo vert_shader_stage_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vert_shader,
            .pName = "main",
    };

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = frag_shader,
            .pName = "main",
    };

    std::array shader_stages = {vert_shader_stage_info, frag_shader_stage_info};

    VkVertexInputBindingDescription vertex_input_binding_description{
            .binding = 0,
            .stride = sizeof(unlogic::Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    std::array vertex_attribute_descriptions = {
            VkVertexInputAttributeDescription{
                    .location = 0,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32_SFLOAT,
                    .offset = offsetof(unlogic::Vertex, position),
            },
            VkVertexInputAttributeDescription{
                    .location = 1,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                    .offset = offsetof(unlogic::Vertex, color),
            },
    };

    VkPipelineVertexInputStateCreateInfo vertex_input_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = 1,
            .pVertexBindingDescriptions = &vertex_input_binding_description,
            .vertexAttributeDescriptionCount = vertex_attribute_descriptions.size(),
            .pVertexAttributeDescriptions = vertex_attribute_descriptions.data(),
    };

    bool enable_primitive_restart = info.topology == VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP ? VK_TRUE : VK_FALSE;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = info.topology,
            .primitiveRestartEnable = enable_primitive_restart,
    };

    std::array dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = dynamic_states.size(),
            .pDynamicStates = dynamic_states.data(),
    };

    VkPipelineViewportStateCreateInfo viewport_state_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_info{
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

    VkPipelineMultisampleStateCreateInfo multisample_state_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment_state{
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo color_blend_state_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &color_blend_attachment_state,
            .blendConstants = {0.f, 0.f, 0.f, 0.f},
    };

    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    };

    VkPipelineLayoutCreateInfo pipeline_layout_info{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 1,
            .pSetLayouts = &info.descriptor_set_layout_,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr,
    };

    if (this->dev_->vkCreatePipelineLayout(this->window_->device(), &pipeline_layout_info, nullptr, &this->pipeline_layout_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo pipeline_info{
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = shader_stages.size(),
            .pStages = shader_stages.data(),
            .pVertexInputState = &vertex_input_info,
            .pInputAssemblyState = &input_assembly_info,
            .pViewportState = &viewport_state_info,
            .pRasterizationState = &rasterization_state_info,
            .pMultisampleState = &multisample_state_info,
            .pDepthStencilState = &depth_stencil_state_info,
            .pColorBlendState = &color_blend_state_info,
            .pDynamicState = &dynamic_state_info,
            .layout = this->pipeline_layout_,
            .renderPass = this->window_->defaultRenderPass(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
    };

    if (this->dev_->vkCreateGraphicsPipelines(this->window_->device(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->pipeline_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline");
    }

    this->dev_->vkDestroyShaderModule(this->window_->device(), vert_shader, nullptr);
    this->dev_->vkDestroyShaderModule(this->window_->device(), frag_shader, nullptr);
}

void ui::VulkanPipeline::Destroy()
{
    this->dev_->vkDestroyPipeline(this->window_->device(), this->pipeline_, nullptr);
    this->dev_->vkDestroyPipelineLayout(this->window_->device(), this->pipeline_layout_, nullptr);
}

ui::VulkanPipeline::~VulkanPipeline()
{
    this->Destroy();
}

/*
this->dev_->vkDestroyBuffer(this->window_->device(), this->ubo_buffer_, nullptr);
    this->dev_->vkFreeMemory(this->window_->device(), this->ubo_memory_, nullptr);

    this->dev_->vkDestroyDescriptorPool(this->window_->device(), this->descriptor_pool_, nullptr);

*/

/*
void ui::VulkanPipeline::DrawVertexBuffer(unlogic::VertexBuffer *vertex_buffer)
{
    VkCommandBuffer cmd = this->window_->currentCommandBuffer();

    this->dev_->vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline_);
    this->dev_->vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline_layout_, 0, 1, &this->descriptor_set_, 0, nullptr);

    std::array buffers = {static_cast<VkBuffer>(vertex_buffer->NativeHandle())};
    constexpr std::array<VkDeviceSize, 1> offsets = {0};
    this->dev_->vkCmdBindVertexBuffers(cmd, 0, 1, buffers.data(), offsets.data());

    this->dev_->vkCmdDraw(cmd, vertex_buffer->GetVertexCount(), 1, 0, 0);
}
*/
