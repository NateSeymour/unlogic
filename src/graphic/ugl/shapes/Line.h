#ifndef UNLOGIC_LINE_H
#define UNLOGIC_LINE_H

#include <vector>
#include "graphic/ugl/Vertex.h"

namespace unlogic
{
    struct Line
    {
        std::vector<Vertex> vertices;
    };
}

#endif //UNLOGIC_LINE_H
