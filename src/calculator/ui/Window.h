#ifndef UNLOGIC_WINDOW_H
#define UNLOGIC_WINDOW_H

#include <string>
#include <fstream>
#include <cstdint>
#include <gtkmm.h>
#include <iostream>
#include "parser/Parser.h"
#include "util/format.h"
#include "SourceEditor.h"
#include "CanvasArea.h"
#include "graphic/Graph.h"

namespace unlogic
{
    class Window : public Gtk::Window
    {
    protected:
        Glib::Dispatcher event_compilation_finished;

        Gtk::Paned divider_;

        std::shared_ptr<SourceBuffer> source_buffer_;
        SourceEditor source_editor_;

        CanvasArea canvas_;
        Graph graph_;

        Compiler compiler_;

        void on_renderer_realize() { /* Empty */ }

        bool on_renderer_render(const Glib::RefPtr<Gdk::GLContext>& context)
        {
            this->canvas_.Clear();
            this->graph_.DrawOnto(this->canvas_);

            return true;
        }

        void on_renderer_mouse_pressed(int press_count, double x, double y)
        {
            std::cout << x << ", " << y << std::endl;
        }

        void on_renderer_drag_begin(double x, double y) {}

        void on_renderer_drag_update(double x, double y) {}

        void on_renderer_drag_end(double x, double y) {}

        void on_source_buffer_changed()
        {
            auto source = this->source_buffer_->get_text();
            auto program = this->compiler_.Compile(source.c_str());

            program.Run();
        }

    public:
        Window()
        {
            this->set_title("Unlogic");
            this->set_default_size(1000, 500);

            this->divider_.set_position(500);

            // Divider
            this->set_child(this->divider_);

            // Source Buffer
            this->source_buffer_ = std::make_shared<SourceBuffer>();
            this->source_buffer_->signal_changed().connect(sigc::mem_fun(*this, &Window::on_source_buffer_changed), false);
            this->source_buffer_->set_text("given f(x) := 3*x + 5;");

            // Source Editor
            this->source_editor_.set_buffer(this->source_buffer_);
            this->divider_.set_start_child(this->source_editor_);

            // Renderer
            this->divider_.set_end_child(this->canvas_);

            auto renderer_mouse_press_controller = Gtk::GestureClick::create();
            renderer_mouse_press_controller->signal_pressed().connect(sigc::mem_fun(*this, &Window::on_renderer_mouse_pressed), false);
            this->canvas_.add_controller(renderer_mouse_press_controller);

            auto renderer_drag_controller = Gtk::GestureDrag::create();
            renderer_drag_controller->signal_drag_begin().connect(sigc::mem_fun(*this, &Window::on_renderer_drag_begin), false);
            renderer_drag_controller->signal_drag_update().connect(sigc::mem_fun(*this, &Window::on_renderer_drag_update), false);
            renderer_drag_controller->signal_drag_end().connect(sigc::mem_fun(*this, &Window::on_renderer_drag_end), false);

            this->canvas_.signal_realize().connect(sigc::mem_fun(*this, &Window::on_renderer_realize));
            this->canvas_.signal_render().connect(sigc::mem_fun(*this, &Window::on_renderer_render), false);
        }
    };
}

#endif //UNLOGIC_WINDOW_H
