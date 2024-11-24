#include "Rect.h"

using namespace unlogic;

Rect::Rect(std::array<glm::vec2, 4> const &corners)
{
    this->vertices = {
            Vertex{corners[0], this->color},
            Vertex{corners[1], this->color},
            Vertex{corners[2], this->color},
            Vertex{corners[2], this->color},
            Vertex{corners[3], this->color},
            Vertex{corners[0], this->color},
    };
}

Rect::Rect(glm::vec2 const &center, glm::vec2 const &size)
{
    std::array<glm::vec2, 4> corners = {
            glm::vec2{center.x - (size.x / 2), center.y - (size.y / 2)},
            glm::vec2{center.x + (size.x / 2), center.y - (size.y / 2)},
            glm::vec2{center.x + (size.x / 2), center.y + (size.y / 2)},
            glm::vec2{center.x - (size.x / 2), center.y + (size.y / 2)},
    };

    this->vertices = {
            Vertex{corners[0], this->color},
            Vertex{corners[1], this->color},
            Vertex{corners[2], this->color},
            Vertex{corners[2], this->color},
            Vertex{corners[3], this->color},
            Vertex{corners[0], this->color},
    };
}
