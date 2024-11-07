#ifndef UNLOGIC_LINE_H
#define UNLOGIC_LINE_H

#include <cmath>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "compiler/Compiler.h"
#include "graphic/ugl/Drawable.h"

namespace unlogic
{
    class Line : public Shape
    {
        std::string title_ = "My Plot";
        std::vector<glm::vec2> points_;

    protected:
        [[nodiscard]] std::shared_ptr<VertexBuffer> GenerateVertexBuffer() const override;

    public:
        void SetFunction(unlogic::Program const &function, glm::vec2 const &domain);

        void SetPoints(std::vector<glm::vec2> const &points);

        Line(unlogic::Program const &function, glm::vec2 const &domain)
        {
            this->SetFunction(function, domain);
        }

        Line(std::vector<glm::vec2> const &points)
        {
            this->SetPoints(points);
        }

        Line(glm::vec2 const &from, glm::vec2 const &to) : Line(std::vector<glm::vec2>{from, to}) {}
    };
}

#endif //UNLOGIC_LINE_H
