#ifndef UNLOGIC_WINDOW_H
#define UNLOGIC_WINDOW_H

#include <string>
#include <fstream>
#include <cstdint>
#include <cstddef>
#include <gtkmm.h>
#include <epoxy/gl.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

        GLuint plot_program_;

        glm::vec3 camera_position;

        void realize_renderer()
        {
            // Load and compile shaders
            this->renderer_.make_current();

            // Compile and link shaders
            GLuint vertex_shader = this->renderer_.LoadShaderFromFile("resource/shaders/plot.vert");
            GLuint fragment_shader = this->renderer_.LoadShaderFromFile("resource/shaders/plot.frag");

            this->plot_program_ = glCreateProgram();
            glAttachShader(this->plot_program_, vertex_shader);
            glAttachShader(this->plot_program_, fragment_shader);
            glLinkProgram(this->plot_program_);

            // Add example plot
            this->graph_.AddPlot("a(x) := x^2");
            this->graph_.AddPlot("b(x) := x^2 - 10");
            this->graph_.AddPlot("c(x) := x");
            this->graph_.AddPlot("d(x) := 0");
            this->graph_.AddPlot("e(x) := 1");
        }

        void unrealize_renderer()
        {
            this->renderer_.make_current();
        }

        void mousewheel_renderer()
        {

        }

        bool render(const Glib::RefPtr<Gdk::GLContext>& context)
        {
            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Generate view and projection matrices
            glm::mat4 view(1.f);
            view = glm::translate(view, this->camera_position);

            float height = (float)this->renderer_.get_height();
            float width = (float)this->renderer_.get_width();

            // TODO: Change to perspective projection to more easily "zoom" in and out by moving camera representation
            glm::mat4 projection = glm::perspective(glm::radians(45.f), width / height, 0.f, 100.f);

            // Setup shader
            glUseProgram(this->plot_program_);

            GLint view_uniform = glGetUniformLocation(this->plot_program_, "view");
            glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

            GLint projection_uniform = glGetUniformLocation(this->plot_program_, "projection");
            glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

            for(auto &plot : this->graph_.GetPlots())
            {
                plot.Draw();
            }

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
