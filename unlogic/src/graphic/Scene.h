//
// Created by Nathan on 11/12/2024.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "shape/Plot.h"

namespace unlogic
{
    struct Scene
    {
        bool draw_grid = true;
        std::vector<Plot2d> plots;

        void AddPlot(char const *name, Plot2dFunctionType function);
    };
} // namespace unlogic

#endif // SCENE_H
