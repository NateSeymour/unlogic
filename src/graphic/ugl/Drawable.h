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
        glm::vec4 color_ = {1.f, 0.f, 0.f, 1.f};
        float line_thickness_= 0.1f;
        float precision_ = 0.1f;

        [[nodiscard]] virtual std::shared_ptr<VertexBuffer> GenerateVertexBuffer() const = 0;

        void Update();

    public:
        void SetColor(glm::vec4 const &color);

        void SetLineThickness(float thickness);

        void SetPrecision(float precision);

        VertexBuffer const &GetVertexBuffer()
        {
            return *this->vbo_;
        }

        void DrawOnto(Canvas &canvas) override;

        virtual ~Shape() = default;
    };
}

#endif //UNLOGIC_DRAWABLE_H
