#ifndef UNLOGIC_RENDERTARGET_H
#define UNLOGIC_RENDERTARGET_H

#include <epoxy/gl.h>
#include "graphic/View.h"
#include "graphic/Math.h"
#include "graphic/Color.h"

namespace mf
{
    class Drawable;

    enum class Primitive
    {
        kTriangleStrip = GL_TRIANGLE_STRIP,
    };

    class RenderTarget
    {
        View view_;

    protected:
        void UpdateViewport()
        {
            // TODO: Implement
        }

        void Initialize()
        {
            this->UpdateViewport();
        }

    public:
        void Draw(Drawable const &drawable) {};

        void Draw(std::vector<Vertex> const &vertices, Primitive primitive)
        {
            unsigned int vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);


        };

        void Clear(mf::Color color)
        {
            glClearColor(color.r, color.g, color.b, color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        virtual Vector2u GetSize() const = 0;

        void SetView(View const &view)
        {
            this->view_ = view;
            this->UpdateViewport();
        }

        View const &GetView() const
        {
            return this->view_;
        }
    };
}

#endif //UNLOGIC_RENDERTARGET_H
