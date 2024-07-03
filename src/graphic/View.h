#ifndef UNLOGIC_VIEW_H
#define UNLOGIC_VIEW_H

#include "graphic/Math.h"

namespace mf
{
    struct Viewport
    {
        Vector2f position;
        Vector2f size;
    };

    class View
    {
        Vector2f size_;
        Vector2f center_;

    public:
        Vector2f const &GetCenter() const
        {
            this->center_;
        }

        Vector2f const &GetSize() const
        {
            return this->size_;
        }

        Viewport CalculateViewport() const
        {

        }

        View(Vector2f size, Vector2f center) : size_(size), center_(center) {}
    };
}

#endif //UNLOGIC_VIEW_H
