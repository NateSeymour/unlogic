//
// Created by Nathan on 11/12/2024.
//

#ifndef SCENE_H
#define SCENE_H

#include <cstdlib>
#include <vector>
#include "Color.h"
#include "shape/Plot.h"

namespace unlogic
{
    struct Scene
    {
        bool draw_grid = true;
        std::vector<Plot2d> plots;

        void AddPlot(char const *name, Plot2dFunctionType function)
        {
            std::array colors = {
                    Color::Red,
                    Color::Green,
                    Color::Blue,
                    Color::Orange,
                    Color::Yellow,
                    Color::Cyan,
                    Color::Pink,
                    Color::Purple,
            };
            Color color = colors[std::rand() % colors.size()];

            this->plots.emplace_back(name, function, color);
        }
    };
} // namespace unlogic

#endif // SCENE_H
