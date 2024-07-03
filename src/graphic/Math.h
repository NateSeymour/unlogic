#ifndef UNLOGIC_MATH_H
#define UNLOGIC_MATH_H

#include "graphic/Color.h"

namespace mf
{
    template<typename T>
    struct Vector
    {
        T x;
        T y;

        Vector(T x, T y) : x(x), y(y) {}
    };

    typedef Vector<float> Vector2f;
    typedef Vector<double> Vector2d;
    typedef Vector<int> Vector2i;
    typedef Vector<unsigned> Vector2u;

    struct Vertex
    {
        Vector2f position;
        Color color;

        Vertex(Vector2f position, Color color = Color::Red) : position(std::move(position)), color(std::move(color)) {}
    };
}

#endif //UNLOGIC_MATH_H
