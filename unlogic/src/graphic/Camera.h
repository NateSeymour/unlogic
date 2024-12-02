//
// Created by Nathan on 11/22/2024.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <numbers>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace unlogic
{
    class Camera
    {
        alignas(4) float dpi = 1.f;
        alignas(4) float fov = 45.f;

        /**
         * Location of camera in WORLD.
         */
        alignas(16) glm::vec3 location = {0.f, 0.f, 20.0f};
        alignas(8) glm::vec2 window = {0.f, 0.f};

        alignas(16) glm::mat4 model = glm::mat4(1.f);
        alignas(16) glm::mat4 view = glm::mat4(1.f);
        alignas(16) glm::mat4 projection = glm::mat4(1.f);

    protected:
        void CalculateMVP()
        {
            this->location.z = glm::clamp(this->location.z, 5.f, 100.f);

            // Plots come out of function in world space already.
            this->model = glm::mat4(1.f);

            this->view = glm::lookAt(this->location, {this->location.x, this->location.y, 0.f}, {0.f, 1.f, 0.f});

            if (this->window.y != 0)
            {
                this->projection = glm::perspective(glm::radians(this->fov), this->window.x / this->window.y, 0.1f, 100.f);
            }
        }

    public:
        void SetDPI(float new_dpi)
        {
            this->dpi = new_dpi;
            this->CalculateMVP();
        }

        void SetWindowSize(glm::vec2 const &new_window)
        {
            this->window = new_window;
            this->CalculateMVP();
        }

        [[nodiscard]] glm::vec2 GetWindowSize() const
        {
            return this->window;
        }

        void TranslatePixel(glm::vec2 const &distance, float d = 0.f)
        {
            auto world_translation = glm::vec3((distance / this->window) * this->WorldDimensionsAtDepth(d), 0.f) * -1.f;
            this->location = glm::translate(glm::mat4(1.f), world_translation) * glm::vec4(this->location, 1.f);
            this->CalculateMVP();
        }

        void TranslateWorld(glm::vec3 const &translation)
        {
            this->location = glm::translate(glm::mat4(1.f), translation) * glm::vec4(this->location, 1.f);
            this->CalculateMVP();
        }

        [[nodiscard]] glm::vec2 WorldDimensionsAtDepth(float d = 0.f) const
        {
            float distance = this->location.z + d;
            float alpha = (180.f - this->fov) / 2;
            float base = (distance / std::tan(alpha * (std::numbers::pi / 180.f))) * 2;

            return {
                    base * (this->window.x / this->window.y),
                    base,
            };
        }

        [[nodiscard]] glm::vec3 ScreenToWorldCoordinate(glm::vec3 const &screen) const {}

        [[nodiscard]] glm::vec3 WorldToScreenCoordinate(glm::vec3 const &world) const {}
    };
} // namespace unlogic

#endif // CAMERA_H
