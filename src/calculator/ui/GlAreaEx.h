//
// Created by nathan on 7/3/24.
//

#ifndef UNLOGIC_GLAREAEX_H
#define UNLOGIC_GLAREAEX_H

#include <gtkmm.h>
#include "graphic/RenderTarget.h"

namespace unlogic
{
    class GlAreaEx : public Gtk::GLArea, public mf::RenderTarget
    {
    public:
        mf::Vector2u GetSize() const override
        {
            return {(unsigned)this->get_width(), (unsigned)this->get_height()};
        }
    };
} // unlogic

#endif //UNLOGIC_GLAREAEX_H
