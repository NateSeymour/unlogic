#ifndef UNLOGIC_DRAWABLE_H
#define UNLOGIC_DRAWABLE_H

#include <vector>
#include <atomic>
#include <epoxy/gl.h>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace unlogic
{
    // Forward decls
    class Canvas;

    struct Vertex
    {
        glm::vec2 position;
        glm::vec4 color;

        Vertex(glm::vec2 const &position, glm::vec4 const &color = {1.f, 0.f, 0.f, 1.f}) : position(position), color(color) {}
    };

    enum class PrimitiveType : GLenum
    {
        kTriangles = GL_TRIANGLES,
        kTriangleStrip = GL_TRIANGLE_STRIP,
        kTriangleFan = GL_TRIANGLE_FAN,
    };

    class Drawable
    {
    public:
        virtual void DrawOnto(Canvas &canvas) = 0;
    };

    class Shape : public Drawable
    {
        std::vector<Vertex> vertices_;

    protected:
        virtual void GenerateVertices(std::vector<Vertex> &vertices) = 0;

    public:
        const PrimitiveType primitive_type = PrimitiveType::kTriangleStrip;

        std::vector<Vertex> const &GetVertices()
        {
            if(this->vertices_.empty())
            {
                this->GenerateVertices(this->vertices_);
            }

            return this->vertices_;
        }

        void DrawOnto(Canvas &canvas) override;
    };
}

#endif //UNLOGIC_DRAWABLE_H
