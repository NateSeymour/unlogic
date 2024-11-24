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
        alignas(4) float resolution = 50.f;
        alignas(4) float dpi_scalar = 1.f;

        alignas(8) glm::vec2 center = {0.f, 0.f};
        alignas(8) glm::vec2 window;

        alignas(16) glm::mat4 model;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 projection;
    };
} // namespace unlogic

#endif // CAMERA_H
