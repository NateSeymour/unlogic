#ifndef UNLOGIC_DRAWABLE_H
#define UNLOGIC_DRAWABLE_H

#include <vector>
#include <memory>

namespace unlogic
{
    // Forward decls
    class Canvas;

    class Drawable
    {
    public:
        virtual void DrawOnto(Canvas &canvas) = 0;
    };
}

#endif //UNLOGIC_DRAWABLE_H
