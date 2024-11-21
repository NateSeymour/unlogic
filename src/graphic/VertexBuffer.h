//
// Created by Nathan on 11/21/2024.
//

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <memory>
#include "ugl/Vertex.h"

namespace unlogic
{
    class VertexBuffer
    {
        std::size_t vertex_count_ = 0;

    public:
        void SetVertexCount(std::size_t vertex_count)
        {
            this->vertex_count_ = vertex_count;
        }

        [[nodiscard]] std::size_t GetVertexCount() const
        {
            return this->vertex_count_;
        }

        virtual void Release() = 0;
        virtual void Allocate(Vertex const *vertices, std::size_t vertex_count) = 0;
        virtual void *GetNativeHandle() = 0;

        virtual ~VertexBuffer() = default;
    };

    class VertexBufferProvider
    {
    public:
        virtual std::unique_ptr<VertexBuffer> GetVertexBuffer() = 0;

        virtual ~VertexBufferProvider() = default;
    };
}

#endif //VERTEXBUFFER_H
