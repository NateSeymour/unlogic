//
// Created by Nathan on 11/14/2024.
//

#ifndef PLOT_H
#define PLOT_H

#include <string>
#include <glm/glm.hpp>
#include "Color.h"
#include "VertexBuffer.h"

namespace unlogic
{
    using Plot2dFunctionType = double (*)(double);

    class Plot2d
    {
        std::string name_;
        Color color_ = Color::Red;

        Plot2dFunctionType fn_ = nullptr;

        glm::vec2 domain_ = {-10.f, 10.f};
        glm::vec2 range_ = {-10.f, 10.f};

        float precision_ = 0.1f;
        float line_thickness_ = 0.05f;

        VertexBufferProvider *buffer_provider_ = nullptr;

    public:
        std::unique_ptr<VertexBuffer> vertex_buffer;

        Plot2d(VertexBufferProvider *buffer_provider, std::string name, Plot2dFunctionType fn, Color color);
    };
} // namespace unlogic

#endif // PLOT_H
