#ifndef UNLOGIC_VERTEXBUFFER_H
#define UNLOGIC_VERTEXBUFFER_H

#include <vector>
#include <cstdint>
#include <cstddef>
#include <epoxy/gl.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace unlogic
{
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

    class VertexBuffer
    {
        friend class Canvas;

    protected:
        GLuint vbo_;
        std::size_t size_;
        PrimitiveType primitive_type_;

    public:
        VertexBuffer(std::vector<Vertex> const &vertices, PrimitiveType primitive = PrimitiveType::kTriangleStrip)
        {
            this->size_ = vertices.size();
            this->primitive_type_ = primitive;

            // Generate object buffers
            glGenBuffers(1, &this->vbo_);

            // Bind data to buffers
            glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
        }

        ~VertexBuffer()
        {
            glDeleteBuffers(1, &this->vbo_);
        }
    };
} // unlogic

#endif //UNLOGIC_VERTEXBUFFER_H
