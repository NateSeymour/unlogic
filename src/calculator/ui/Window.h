#ifndef UNLOGIC_WINDOW_H
#define UNLOGIC_WINDOW_H

#include <cstdint>
#include <gtkmm.h>
#include <epoxy/gl.h>
#include "SourceEditor.h"
#include "GlAreaEx.h"
#include "graphic/Graph.h"

namespace unlogic
{
    class Window : public Gtk::Window
    {
    protected:
        Gtk::Paned divider_;

        SourceEditor source_editor_;
        GlAreaEx renderer_;

        Graph graph_;

        bool render(const Glib::RefPtr<Gdk::GLContext>& context)
        {
            this->renderer_.Clear(mf::Color::White);
            this->renderer_.Draw(this->graph_);

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
            this->divider_.set_end_child(this->renderer_);
            this->renderer_.signal_render().connect(sigc::mem_fun(*this, &Window::render), false);
        }
    };
}

#endif //UNLOGIC_WINDOW_H
