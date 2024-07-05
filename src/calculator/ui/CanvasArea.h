//
// Created by nathan on 7/3/24.
//

#ifndef UNLOGIC_CANVASAREA_H
#define UNLOGIC_CANVASAREA_H

#include <gtkmm.h>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstddef>
#include <epoxy/gl.h>
#include <filesystem>
#include <utility>
#include <vector>
#include <atomic>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "graphic/Drawable.h"
#include "util/io.h"

namespace unlogic
{
    class Canvas
    {
        std::atomic_bool initialized_ = false;

        std::vector<GLuint> shaders_;
        GLuint shader_program_;
        glm::vec3 camera_ = {0.f, 0.f, -50.f};

    protected:
        GLuint LoadShaderFromFile(std::filesystem::path const &path)
        {
            GLenum shader_type = path.extension() == ".vert" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
            GLuint shader = glCreateShader(shader_type);

            std::string source = unlogic::load_file(path);

            char const* source_array[] = { source.c_str(), nullptr };
            glShaderSource(shader, 1, source_array, nullptr);
            glCompileShader(shader);

            this->shaders_.push_back(shader);

            return shader;
        }

        void BeginDraw()
        {
            if(!this->initialized_)
            {
                this->Initialize();
                this->initialized_ = true;
            }

            this->GetContext()->make_current();

            // Generate view and projection matrices
            glm::mat4 view(1.f);
            view = glm::translate(view, this->camera_);

            float height = (float)this->GetContext()->get_surface()->get_height();
            float width = (float)this->GetContext()->get_surface()->get_width();

            // TODO: Change to perspective projection to more easily "zoom" in and out by moving camera representation
            glm::mat4 projection = glm::perspective(glm::radians(45.f), width / height, 0.f, 100.f);

            // Setup shader
            glUseProgram(this->shader_program_);

            GLint view_uniform = glGetUniformLocation(this->shader_program_, "view");
            glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

            GLint projection_uniform = glGetUniformLocation(this->shader_program_, "projection");
            glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));
        }

        void Initialize()
        {
            this->GetContext()->make_current();

            // Compile and link shaders
            GLuint vertex_shader = this->LoadShaderFromFile("resource/shaders/plot.vert");
            GLuint fragment_shader = this->LoadShaderFromFile("resource/shaders/plot.frag");

            this->shader_program_ = glCreateProgram();
            glAttachShader(this->shader_program_, vertex_shader);
            glAttachShader(this->shader_program_, fragment_shader);
            glLinkProgram(this->shader_program_);

            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);
        }

        virtual std::shared_ptr<Gdk::GLContext> GetContext() = 0;

    public:
        void Clear(float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f)
        {
            this->BeginDraw();

            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void DrawPrimitive(std::vector<Vertex> const &vertices, PrimitiveType type)
        {
            this->BeginDraw();

            GLuint vao, vbo = -1;

            // Generate object buffers if not already exist
            glGenBuffers(1, &vbo);
            glGenVertexArrays(1, &vao);

            // Bind vertex array
            glBindVertexArray(vao);

            // Bind data to buffers
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position)); // Position
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color)); // Color
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);

            // Draw
            glDrawArrays((GLenum)type, 0, vertices.size());

            // Cleanup
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
        }

        void Draw(Drawable &drawable)
        {
            drawable.DrawOnto(*this);
        }
    };

    class CanvasArea : public Gtk::GLArea, public Canvas
    {
    protected:
        std::shared_ptr<Gdk::GLContext> GetContext() override
        {
            return this->get_context();
        }

    public:
        CanvasArea() : Gtk::GLArea() {}
    };
} // unlogic

#endif //UNLOGIC_CANVASAREA_H
