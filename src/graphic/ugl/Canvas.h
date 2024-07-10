#ifndef UNLOGIC_CANVAS_H
#define UNLOGIC_CANVAS_H

#include <gtkmm.h>
#include <iostream>
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
#include "util/format.h"

namespace unlogic
{
    class Canvas
    {
        GLuint shader_program_;

    protected:
        glm::ivec2 screen_;
        glm::vec3 camera_ = {0.f, 0.f, 0.f};
        float px_per_unit = 50.f;

        [[nodiscard]] GLuint LoadShaderFromFile(std::filesystem::path const &path)
        {
            GLenum shader_type = path.extension() == ".vert" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
            GLuint shader = glCreateShader(shader_type);

            std::string source = unlogic::load_file(path);

            char const* source_array[] = { source.c_str(), nullptr };
            glShaderSource(shader, 1, source_array, nullptr);
            glCompileShader(shader);

            return shader;
        }

        void Initialize(glm::ivec2 const &screen_size)
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

            this->SetScreenSize(screen_size);
        }

        void Destroy()
        {
            glDeleteProgram(this->shader_program_);
            this->shader_program_ = 0;
        }

        virtual std::shared_ptr<Gdk::GLContext> GetContext() = 0;

    public:
        void SetScreenSize(glm::ivec2 const &size)
        {
            this->screen_ = size;
        }

        glm::vec3 &GetCamera()
        {
            return this->camera_;
        }

        void Clear(float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f)
        {
            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void DrawPrimitive(std::vector<Vertex> const &vertices, PrimitiveType type)
        {
            // Setup camera and transformation matrices
            glm::mat4 view(1.f);
            view = glm::translate(view, this->camera_);

            glm::vec2 world_size = glm::vec2(this->screen_.x, this->screen_.y) / this->px_per_unit;
            glm::mat4 projection = glm::ortho(world_size.x * -1, world_size.x, world_size.y * -1, world_size.y);

            // Setup shader
            glUseProgram(this->shader_program_);

            GLint view_uniform = glGetUniformLocation(this->shader_program_, "view");
            glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));

            GLint projection_uniform = glGetUniformLocation(this->shader_program_, "projection");
            glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

            // Generate object buffers
            GLuint vao, vbo = -1;
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
}

#endif //UNLOGIC_CANVAS_H
