//
// Created by nathan on 7/3/24.
//

#ifndef UNLOGIC_GLAREAEX_H
#define UNLOGIC_GLAREAEX_H

#include <gtkmm.h>
#include <string>
#include <fstream>
#include <cstdint>
#include <epoxy/gl.h>
#include <filesystem>
#include <vector>

namespace unlogic
{
    class GlAreaEx : public Gtk::GLArea
    {
        std::vector<GLuint> shaders_;

        std::string LoadFile(std::filesystem::path const &path)
        {
            std::ifstream fp(path, std::ios::in | std::ios::binary);
            return {std::istreambuf_iterator<char>{fp}, std::istreambuf_iterator<char>{}};
        }

    public:
        GLuint LoadShaderFromFile(std::filesystem::path const &path)
        {
            GLenum shader_type = path.extension() == ".vert" ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
            GLuint shader = glCreateShader(shader_type);

            std::string source = this->LoadFile(path);

            char const* source_array[] = { source.c_str(), nullptr };
            glShaderSource(shader, 1, source_array, nullptr);
            glCompileShader(shader);

            this->shaders_.push_back(shader);

            return shader;
        }

        void Destroy()
        {
            for(auto shader : this->shaders_)
            {
                glDeleteShader(shader);
            }
        }
    };
} // unlogic

#endif //UNLOGIC_GLAREAEX_H
