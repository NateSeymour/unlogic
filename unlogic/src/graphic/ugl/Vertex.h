#ifndef UNLOGIC_VERTEX_H
#define UNLOGIC_VERTEX_H

#include <glm/glm.hpp>
#include "graphic/Color.h"

namespace unlogic
{
    struct Vertex
    {
        glm::vec2 position;
        Color color;
    };
}

#endif //UNLOGIC_VERTEX_H
