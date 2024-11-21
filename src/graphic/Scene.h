//
// Created by Nathan on 11/12/2024.
//

#ifndef SCENE_H
#define SCENE_H

#include "Color.h"

namespace unlogic
{
    struct Scene
    {
        Color background = Color::White;
        bool draw_gridlines = true;
    };
} // unlogic

#endif //SCENE_H
