#ifndef UNLOGIC_DRAWABLE_H
#define UNLOGIC_DRAWABLE_H

#include "graphic/RenderTarget.h"

namespace mf
{
    class Drawable
    {
    public:
        virtual void Draw(RenderTarget &target) const = 0;
    };
} // mf

#endif //UNLOGIC_DRAWABLE_H
