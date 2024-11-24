//
// Created by Nathan on 11/12/2024.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "VertexBuffer.h"
#include "Color.h"
#include "shape/Plot.h"

namespace unlogic
{
    struct Scene
    {
        Color background = Color::Green;
        bool draw_grid = true;
        std::vector<Plot2d> plots;

        void AddPlot(char const *name, Plot2dFunctionType function)
        {
            std::array colors = {
                    Color::Red,
                    Color::Green,
                    Color::Blue,
            };
            Color color = colors[std::rand() % colors.size()];

            this->plots.emplace_back(name, function, color);
        }
    };
} // namespace unlogic

#endif // SCENE_H
