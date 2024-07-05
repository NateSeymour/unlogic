#ifndef UNLOGIC_WINDOW_H
#define UNLOGIC_WINDOW_H

#include <string>
#include <fstream>
#include <cstdint>
#include <gtkmm.h>
#include "SourceEditor.h"
#include "CanvasArea.h"
#include "graphic/Graph.h"

namespace unlogic
{
    class Window : public Gtk::Window
    {
    protected:
        Gtk::Paned divider_;

        SourceEditor source_editor_;
        CanvasArea canvas_;

        Graph graph_;

        void realize_renderer()
        {
            // Add example plot
            this->graph_.AddPlot("a(x) := x^2");
            this->graph_.AddPlot("b(x) := x^2 - 10");
            this->graph_.AddPlot("c(x) := x");
            this->graph_.AddPlot("d(x) := 0");
            this->graph_.AddPlot("e(x) := 1");
        }

        bool render(const Glib::RefPtr<Gdk::GLContext>& context)
        {
            this->canvas_.Clear();
            this->graph_.DrawOnto(this->canvas_);

            return true;
        }

    public:
        Window()
        {
            this->set_title("Unlogic");
            this->set_default_size(1000, 500);

            // Divider
            this->set_child(this->divider_);

            // Source Editor
            this->divider_.set_start_child(this->source_editor_);

            // Renderer
            this->divider_.set_end_child(this->canvas_);
            this->canvas_.signal_realize().connect(sigc::mem_fun(*this, &Window::realize_renderer));
            // this->canvas_.signal_unrealize().connect(sigc::mem_fun(*this, &Window::unrealize_renderer), false);
            this->canvas_.signal_render().connect(sigc::mem_fun(*this, &Window::render), false);
        }
    };
}

#endif //UNLOGIC_WINDOW_H
