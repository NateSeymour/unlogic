//
// Created by nathan on 7/3/24.
//

#ifndef UNLOGIC_CANVASAREA_H
#define UNLOGIC_CANVASAREA_H

#include <gtkmm.h>
#include <iostream>
#include "graphic/ugl/Canvas.h"

namespace unlogic
{
    class CanvasArea : public Gtk::GLArea, public Canvas
    {
    protected:
        void on_canvas_realize()
        {
            this->Initialize({this->get_width(), this->get_height()});
        }

        void on_canvas_unrealize()
        {
            this->Destroy();
        }

        void on_canvas_resize(int width, int height)
        {
            this->SetScreenSize({width, height});
        }

        std::shared_ptr<Gdk::GLContext> GetContext() override
        {
            return this->get_context();
        }

    public:
        CanvasArea() : Gtk::GLArea()
        {
            this->signal_realize().connect(sigc::mem_fun(*this, &CanvasArea::on_canvas_realize), false);
            this->signal_unrealize().connect(sigc::mem_fun(*this, &CanvasArea::on_canvas_unrealize), false);
            this->signal_resize().connect(sigc::mem_fun(*this, &CanvasArea::on_canvas_resize), true);
        }
    };
} // unlogic

#endif //UNLOGIC_CANVASAREA_H
