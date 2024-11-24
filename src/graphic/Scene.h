//
// Created by Nathan on 11/12/2024.
//

#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "Camera.h"
#include "VertexBuffer.h"
#include "Color.h"
#include "Plot.h"

namespace unlogic
{
    struct Scene
    {
        std::unique_ptr<VertexBufferProvider> vertex_buffer_provider;

        Camera camera;
        Color background = Color::White;
        bool draw_gridlines = true;

        std::vector<std::unique_ptr<VertexBuffer>> drawables;

        std::vector<Plot2d> plots;

        std::unique_ptr<VertexBuffer> Rect(std::array<glm::vec2, 4> const &corners, Color color = Color::White)
        {
            auto vertex_buffer = this->vertex_buffer_provider->GetVertexBuffer();

            std::array<Vertex, 6> vertices = {
                    Vertex{corners[0], color},
                    Vertex{corners[1], color},
                    Vertex{corners[2], color},
                    Vertex{corners[2], color},
                    Vertex{corners[3], color},
                    Vertex{corners[0], color},
            };

            vertex_buffer->Allocate(vertices.data(), vertices.size());

            return std::move(vertex_buffer);
        }

        void AddPlot(char const *name, Plot2dFunctionType function)
        {
            std::array colors = {
                    Color::Red,
                    Color::Green,
                    Color::Blue,
            };
            Color color = colors[std::rand() % colors.size()];

            this->plots.emplace_back(this->vertex_buffer_provider.get(), name, function, color);
        }

        Scene() = delete;

        Scene(std::unique_ptr<VertexBufferProvider> vertex_buffer_provider) : vertex_buffer_provider(std::move(vertex_buffer_provider)) {}
    };
} // namespace unlogic

#endif // SCENE_H
