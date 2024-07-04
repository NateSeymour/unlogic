#ifndef UNLOGIC_DRAWABLE_H
#define UNLOGIC_DRAWABLE_H

#include <vector>
#include <epoxy/gl.h>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace unlogic
{
    struct Vertex
    {
        glm::vec2 position;
        glm::vec4 color;

        Vertex(glm::vec2 const &position, glm::vec4 const &color = {1.f, 0.f, 0.f, 1.f}) : position(position), color(color) {}
    };

    class Drawable
    {
        std::vector<Vertex> vertices_;
        bool fresh_ = false;

        bool initialized_ = false;

        GLuint vbo_, vao_;

    protected:
        void Invalidate()
        {
            this->fresh_ = false;
        }

        virtual void Update(std::vector<Vertex> &vertices) const = 0;

    public:
        void Draw()
        {
            // Generate object buffers if not already exist
            if(!this->initialized_)
            {
                glGenBuffers(1, &this->vbo_);
                glGenVertexArrays(1, &this->vao_);

                this->initialized_ = true;
            }

            // Bind vertex array
            glBindVertexArray(this->vao_);

            // Regenerate vertices if deemed necessary
            if(!this->fresh_)
            {
                this->Update(this->vertices_);

                // Bind data to buffers
                glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
                glBufferData(GL_ARRAY_BUFFER, this->vertices_.size() * sizeof(Vertex), this->vertices_.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); // Position
                glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); // Color
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);

                this->fresh_ = true;
            }

            glDrawArrays(GL_TRIANGLE_STRIP, 0, this->vertices_.size());
        }
    };
}

#endif //UNLOGIC_DRAWABLE_H
