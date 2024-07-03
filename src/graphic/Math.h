#ifndef UNLOGIC_MATH_H
#define UNLOGIC_MATH_H

namespace mf
{
    template<typename T>
    struct Vector
    {
        T x;
        T y;
    };

    typedef Vector<float> Vector2f;
    typedef Vector<double> Vector2d;
    typedef Vector<int> Vector2i;
    typedef Vector<unsigned> Vector2u;
}

#endif //UNLOGIC_MATH_H
