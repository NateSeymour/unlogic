#ifndef UNLOGIC_RENDERTARGET_H
#define UNLOGIC_RENDERTARGET_H

#include "graphic/Drawable.h"
#include "graphic/View.h"

namespace mf
{
    class RenderTarget
    {
        View view_;

    public:
        void Draw(Drawable const &drawable);
        void Draw();
    };
}

#endif //UNLOGIC_RENDERTARGET_H
