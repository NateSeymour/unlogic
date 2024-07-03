#ifndef UNLOGIC_COLOR_H
#define UNLOGIC_COLOR_H

namespace mf
{
    struct Color
    {
        float r;
        float g;
        float b;
        float a;

        static const Color White;
        static const Color Black;

        constexpr Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}
    };
}

#endif //UNLOGIC_COLOR_H
