//
// Created by Nathan on 11/30/2024.
//

#include "VulkanBuffer.h"
#include <QVulkanDeviceFunctions>

using namespace ui;

VulkanBuffer::VulkanBuffer(QVulkanWindow *window, BufferType type, std::size_t element_size, std::size_t count) :
    window_(window), type_(type), element_size_(element_size), count_(count)
{
    this->dev_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());

    VkBufferCreateInfo buffer_info{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = this->GetSize(),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    switch (type)
    {
        case BufferType::Vertex:
        {
            buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            break;
        }

        case BufferType::Uniform:
        {
            buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            break;
        }
    }

    if (this->dev_->vkCreateBuffer(this->window_->device(), &buffer_info, nullptr, &this->buffer_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create camera buffer");
    }

    VkMemoryRequirements memory_requirements;
    this->dev_->vkGetBufferMemoryRequirements(this->window_->device(), this->buffer_, &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info{
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memory_requirements.size,
            .memoryTypeIndex = this->window_->hostVisibleMemoryIndex(),
    };

    if (this->dev_->vkAllocateMemory(this->window_->device(), &memory_allocate_info, nullptr, &this->memory_) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate memory");
    }

    this->dev_->vkBindBufferMemory(this->window_->device(), this->buffer_, this->memory_, 0);
    this->dev_->vkMapMemory(this->window_->device(), this->memory_, 0, buffer_info.size, 0, (void **)&this->data_);
}

VkBuffer VulkanBuffer::GetNativeHandle() const noexcept
{
    return this->buffer_;
}

std::size_t VulkanBuffer::GetSize() const noexcept
{
    return this->element_size_ * this->count_;
}

std::size_t VulkanBuffer::GetElementSize() const noexcept
{
    return this->element_size_;
}

std::size_t VulkanBuffer::GetCount() const noexcept
{
    return this->count_;
}

void VulkanBuffer::Bind(VkCommandBuffer cmd)
{
    if (this->type_ != BufferType::Vertex)
    {
        throw std::runtime_error("can only bind vertex buffers");
    }

    std::array buffers = {this->buffer_};
    constexpr std::array<VkDeviceSize, 1> offsets = {0};
    this->dev_->vkCmdBindVertexBuffers(cmd, 0, 1, buffers.data(), offsets.data());
}

VulkanBuffer::~VulkanBuffer()
{
    this->dev_->vkUnmapMemory(this->window_->device(), this->memory_);
    this->dev_->vkDestroyBuffer(this->window_->device(), this->buffer_, nullptr);
    this->dev_->vkFreeMemory(this->window_->device(), this->memory_, nullptr);
}
