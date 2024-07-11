#ifndef UNLOGIC_DRAWABLE_H
#define UNLOGIC_DRAWABLE_H

#include <vector>
#include <memory>
#include <epoxy/gl.h>
#include "graphic/ugl/VertexBuffer.h"

namespace unlogic
{
    // Forward decls
    class Canvas;

    class Drawable
    {
    public:
        virtual void DrawOnto(Canvas &canvas) = 0;
    };

    class Shape : public Drawable
    {
        std::shared_ptr<VertexBuffer> vbo_;

    protected:
        [[nodiscard]] virtual std::shared_ptr<VertexBuffer> GenerateVertexBuffer() const = 0;

        void Update()
        {
            this->vbo_ = std::move(this->GenerateVertexBuffer());
        }

    public:
        VertexBuffer const &GetVertexBuffer()
        {
            return *this->vbo_;
        }

        void DrawOnto(Canvas &canvas) override;

        virtual ~Shape() = default;
    };
}

#endif //UNLOGIC_DRAWABLE_H
