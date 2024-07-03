#ifndef UNLOGIC_WINDOW_H
#define UNLOGIC_WINDOW_H

#include <string>
#include <fstream>
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

        void realize_renderer()
        {
            // Load and compile shaders
            this->renderer_.make_current();

            // Compile and link shaders
            GLuint vertex_shader = this->renderer_.LoadShaderFromFile("resource/shaders/plot.vert");
            GLuint fragment_shader = this->renderer_.LoadShaderFromFile("resource/shaders/plot.frag");

            GLuint program = glCreateProgram();
            glAttachShader(program, vertex_shader);
            glAttachShader(program, fragment_shader);
            glLinkProgram(program);

            glUseProgram(program);

            // Setup inputs
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }

        void unrealize_renderer()
        {
            this->renderer_.make_current();
        }

        bool render(const Glib::RefPtr<Gdk::GLContext>& context)
        {
            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

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
            this->renderer_.signal_realize().connect(sigc::mem_fun(*this, &Window::realize_renderer));
            this->renderer_.signal_unrealize().connect(sigc::mem_fun(*this, &Window::unrealize_renderer), false);
            this->renderer_.signal_render().connect(sigc::mem_fun(*this, &Window::render), false);
        }
    };
}

#endif //UNLOGIC_WINDOW_H
