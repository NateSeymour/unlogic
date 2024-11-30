//
// Created by Nathan on 11/14/2024.
//

#ifndef PLOT_H
#define PLOT_H

#include <string>
#include <glm/glm.hpp>
#include "graphic/shape/Shape.h"
#include "graphic/Color.h"

namespace unlogic
{
    using Plot2dFunctionType = double (*)(double);

    class Plot2d : public Shape
    {
        std::string name_;
        Color color_ = Color::Red;

        Plot2dFunctionType fn_ = nullptr;

        glm::vec2 domain_ = {-100.f, 100.f};
        glm::vec2 range_ = {-10.f, 10.f};

        float precision_ = 0.1f;
        float line_thickness_ = 0.15f;

    public:
        Plot2d(std::string name, Plot2dFunctionType fn, Color color);
    };
} // namespace unlogic

#endif // PLOT_H
