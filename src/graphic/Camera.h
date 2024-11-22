//
// Created by Nathan on 11/22/2024.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

namespace unlogic
{
    struct Camera
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
}

#endif //CAMERA_H
