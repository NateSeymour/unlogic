//
// Created by Nathan on 11/30/2024.
//

#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include <QVulkanWindow>

namespace ui
{
    enum class BufferType
    {
        Vertex,
        Uniform,
    };

    class VulkanBuffer
    {
        QVulkanWindow *window_;
        QVulkanDeviceFunctions *dev_;

        VkBuffer buffer_ = nullptr;
        VkDeviceMemory memory_ = nullptr;

        std::size_t size_ = 0;
        void *data_ = nullptr;

    protected:
        VulkanBuffer(QVulkanWindow *window, BufferType type, std::size_t size);

    public:
        template<typename T>
        static VulkanBuffer Create(QVulkanWindow *window, BufferType type, std::size_t count = 1)
        {
            return {window, type, sizeof(T) * count};
        }

        template<typename T>
        void Write(T const *value, std::size_t count = 1)
        {
            if (this->size_ != sizeof(T) * count)
            {
                throw std::runtime_error("attempting to write to buffer of incorrect size!");
            }

            memcpy(this->data_, &value, sizeof(T) * count);
        }

        VulkanBuffer() = delete;
        VulkanBuffer(VulkanBuffer const &) = delete;
        VulkanBuffer(VulkanBuffer &&) = delete;
        ~VulkanBuffer();
    };
} // namespace ui

#endif // VULKANBUFFER_H
