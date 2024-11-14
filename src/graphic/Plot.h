//
// Created by Nathan on 11/14/2024.
//

#ifndef PLOT_H
#define PLOT_H

#include <string>
#include <glm/glm.hpp>
#include "ugl/shapes/Line.h"
#include "Color.h"

namespace unlogic
{
    using Plot2dFunctionType = double(*)(double);

    class Plot2d
    {
        std::string name_ = "My Plot";
        Color color_ = Color::Red;

        Plot2dFunctionType fn_ = nullptr;

        glm::vec2 domain_ = { -10.f, 10.f };
        glm::vec2 range_ = { -10.f, 10.f };

        float precision_ = 0.1f;
        float line_thickness_ = 10.f;

        Line line_;

        Plot2d(std::string name, Plot2dFunctionType fn);

    public:
        static Plot2d FromFunction(std::string name, Plot2dFunctionType fn)
        {
            return {std::move(name), fn};
        }
    };
}

#endif //PLOT_H
