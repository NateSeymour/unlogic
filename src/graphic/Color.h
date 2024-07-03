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
        static const Color Red;
        static const Color Green;
        static const Color Blue;

        constexpr Color(float r, float g, float b, float a = 1.f) : r(r), g(g), b(b), a(a) {}
    };

    inline constexpr Color Color::White(1.f, 1.f, 1.f, 1.f);
    inline constexpr Color Color::Black(0.f, 0.f, 0.f, 1.f);
    inline constexpr Color Color::Red(1.f, 0.f, 0.f, 1.f);
    inline constexpr Color Color::Green(0.f, 1.f, 0.f, 1.f);
    inline constexpr Color Color::Blue(0.f, 0.f, 1.f, 1.f);
}

#endif //UNLOGIC_COLOR_H
