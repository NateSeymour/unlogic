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

        BufferType type_;
        std::size_t element_size_ = 0;
        std::size_t count_ = 0;

        void *data_ = nullptr;

    public:
        template<typename T>
        static std::unique_ptr<VulkanBuffer> Create(QVulkanWindow *window, BufferType type, std::size_t count = 1)
        {
            return std::make_unique<VulkanBuffer>(window, type, sizeof(T), count);
        }

        template<typename T>
        void Write(T const *value, std::size_t count = 1)
        {
            if (this->GetSize() != sizeof(T) * count)
            {
                throw std::runtime_error("attempting to write to buffer of incorrect size!");
            }

            memcpy(this->data_, value, sizeof(T) * count);
        }

        [[nodiscard]] VkBuffer GetNativeHandle() const noexcept;
        [[nodiscard]] std::size_t GetSize() const noexcept;
        [[nodiscard]] std::size_t GetElementSize() const noexcept;
        [[nodiscard]] std::size_t GetCount() const noexcept;

        void Bind(VkCommandBuffer cmd);

        VulkanBuffer() = delete;
        VulkanBuffer(VulkanBuffer const &) = delete;
        VulkanBuffer(QVulkanWindow *window, BufferType type, std::size_t element_size, std::size_t count);

        ~VulkanBuffer();
    };
} // namespace ui

#endif // VULKANBUFFER_H
