#ifndef UNLOGIC_SHAPE_H
#define UNLOGIC_SHAPE_H

#include <vector>
#include "graphic/ugl/Vertex.h"
#include "graphic/Color.h"

namespace unlogic
{
    class Shape
    {
    protected:
        Color color = Color::Green;
        std::vector<Vertex> vertices;

    public:
        [[nodiscard]] std::vector<Vertex> const &GetVertices() const
        {
            return this->vertices;
        }
    };
} // namespace unlogic

#endif // UNLOGIC_SHAPE_H
