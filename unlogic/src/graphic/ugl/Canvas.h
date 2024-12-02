#ifndef UNLOGIC_CANVAS_H
#define UNLOGIC_CANVAS_H

#include <gtkmm.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstddef>
#include <filesystem>
#include <utility>
#include <vector>
#include <atomic>
#include <epoxy/gl.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "graphic/ugl/VertexBuffer.h"
#include "Drawable.h"
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

        [[nodiscard]] GLuint LoadShaderFromFile(std::filesystem::path const &path);

        void Initialize(glm::ivec2 const &screen_size);

        void Destroy();

        virtual std::shared_ptr<Gdk::GLContext> GetContext() = 0;

    public:
        void SetScreenSize(glm::ivec2 const &size);

        [[nodiscard]] glm::vec3 const &GetCamera() const { return this->camera_; }
        void SetCamera(glm::vec3 const &camera) { this->camera_ = camera; }

        [[nodiscard]] glm::ivec2 const &GetScreen() const { return this->screen_; }
        void SetScreen(glm::ivec2 const &screen) { this->screen_ = screen; }

        [[nodiscard]] glm::vec4 GetWorldBounds() const;

        [[nodiscard]] glm::vec2 GetWorldSize() const;

        void Clear(float r = 1.f, float g = 1.f, float b = 1.f, float a = 1.f);

        void DrawPrimitive(VertexBuffer const &vbo);

        void Draw(Drawable &drawable);
    };
}

#endif //UNLOGIC_CANVAS_H
