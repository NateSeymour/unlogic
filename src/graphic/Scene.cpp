//
// Created by Nathan on 11/12/2024.
//

#include "Scene.h"
#include <array>
#include "Color.h"

using namespace unlogic;

void Scene::AddPlot(char const *name, Plot2dFunctionType function)
{
    std::array colors = {
            Color::Red,
            Color::Green,
            Color::Blue,
            Color::Orange,
            Color::Cyan,
            Color::Pink,
            Color::Purple,
    };
    Color color = colors[this->plots.size() % colors.size()];

    this->plots.emplace_back(name, function, color);
}
