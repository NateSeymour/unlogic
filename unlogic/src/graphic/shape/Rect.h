#ifndef UNLOGIC_RECT_H
#define UNLOGIC_RECT_H

#include <glm/glm.hpp>
#include "Shape.h"

namespace unlogic
{
    class Rect : public Shape
    {
    public:
        Rect(std::array<glm::vec2, 4> const &corners);
        Rect(glm::vec2 const &center, glm::vec2 const &size);

        Rect() = default;
    };
} // namespace unlogic

#endif // UNLOGIC_RECT_H
