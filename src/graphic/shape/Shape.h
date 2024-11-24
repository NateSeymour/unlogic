#ifndef UNLOGIC_SHAPE_H
#define UNLOGIC_SHAPE_H

#include <vector>
#include "graphic/VertexBuffer.h"
#include "graphic/ugl/Vertex.h"
#include "graphic/Color.h"

namespace unlogic
{
    class Shape
    {
        std::unique_ptr<VertexBuffer> buffer_;

    protected:
        Color color;
        std::vector<Vertex> vertices;

    public:
        [[nodiscard]] std::vector<Vertex> const &GetVertices() const
        {
            return this->vertices;
        }

        VertexBuffer *GetOrCreateVertexBuffer(VertexBufferProvider *provider)
        {
            if (!this->buffer_)
            {
                this->buffer_ = provider->GetVertexBuffer();
                this->buffer_->Allocate(this->vertices.data(), this->vertices.size());
            }

            return this->buffer_.get();
        }

        void ReleaseBuffer()
        {
            this->buffer_.reset();
        }
    };
} // namespace unlogic

#endif // UNLOGIC_SHAPE_H
