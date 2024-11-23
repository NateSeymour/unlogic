//
// Created by Nathan on 11/21/2024.
//

#ifndef VULKANVERTEXBUFFER_H
#define VULKANVERTEXBUFFER_H

#include <QVulkanDeviceFunctions>
#include <QVulkanWindow>
#include "graphic/VertexBuffer.h"

namespace ui
{
    class VulkanVertexBuffer : public unlogic::VertexBuffer
    {
        QVulkanWindow *window_ = nullptr;
        QVulkanDeviceFunctions *dev_ = nullptr;

        VkBuffer vk_buffer_ = nullptr;
        VkDeviceMemory vk_memory_ = nullptr;

    public:
        void Release() override
        {
            if (this->vk_buffer_)
            {
                this->dev_->vkDestroyBuffer(this->window_->device(), this->vk_buffer_, nullptr);
                this->vk_buffer_ = nullptr;
            }

            if (this->vk_memory_)
            {
                this->dev_->vkFreeMemory(this->window_->device(), this->vk_memory_, nullptr);
                this->vk_memory_ = nullptr;
            }
        }

        void Allocate(unlogic::Vertex const *vertices, std::size_t vertex_count) override
        {
            this->Release();
            this->SetVertexCount(vertex_count);

            VkBufferCreateInfo buffer_info{
                    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                    .size = sizeof(unlogic::Vertex) * vertex_count,
                    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            };

            if (this->dev_->vkCreateBuffer(this->window_->device(), &buffer_info, nullptr, &this->vk_buffer_) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create buffer");
            }

            VkMemoryAllocateInfo memory_allocate_info{
                    .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                    .allocationSize = buffer_info.size,
                    .memoryTypeIndex = this->window_->hostVisibleMemoryIndex(),
            };

            if (this->dev_->vkAllocateMemory(this->window_->device(), &memory_allocate_info, nullptr, &this->vk_memory_) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to allocate memory");
            }

            this->dev_->vkBindBufferMemory(this->window_->device(), this->vk_buffer_, this->vk_memory_, 0);

            void *data = nullptr;
            this->dev_->vkMapMemory(this->window_->device(), this->vk_memory_, 0, buffer_info.size, 0, &data);
            memcpy(data, vertices, buffer_info.size);
            this->dev_->vkUnmapMemory(this->window_->device(), this->vk_memory_);
        }

        void *GetNativeHandle() override
        {
            return (void *)this->vk_buffer_;
        }

        VulkanVertexBuffer(QVulkanWindow *window) : window_(window)
        {
            this->dev_ = this->window_->vulkanInstance()->deviceFunctions(this->window_->device());
        }

        ~VulkanVertexBuffer() override
        {
            this->VulkanVertexBuffer::Release();
        }
    };

    class VulkanVertexBufferProvider : public unlogic::VertexBufferProvider
    {
        QVulkanWindow *window_ = nullptr;

    public:
        std::unique_ptr<unlogic::VertexBuffer> GetVertexBuffer() override
        {
            return std::make_unique<VulkanVertexBuffer>(this->window_);
        }

        VulkanVertexBufferProvider(QVulkanWindow *window) : window_(window) {}
    };
} // namespace ui

#endif // VULKANVERTEXBUFFER_H
