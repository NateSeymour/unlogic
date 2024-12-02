#ifndef COLOR_H
#define COLOR_H

namespace unlogic
{
    struct Color
    {
        float r = 0.0;
        float g = 0.0;
        float b = 0.0;
        float a = 1.0;

        static Color White;
        static Color Black;
        static Color Red;
        static Color Orange;
        static Color Yellow;
        static Color Green;
        static Color Blue;
        static Color Cyan;
        static Color Purple;
        static Color Pink;
    };
} // namespace unlogic

#endif // COLOR_H
