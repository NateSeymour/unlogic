//
// Created by nathan on 5/27/24.
//

#ifndef UNLOGIC_GRAPH_H
#define UNLOGIC_GRAPH_H

#include <cmath>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include "Compiler.h"
#include "graphic/ugl/Drawable.h"
#include "graphic/ugl/shapes/Line.h"

namespace unlogic
{
    class Graph : public Drawable
    {
        std::vector<Line> plots_;
        glm::vec2 domain_;
        float axis_thickness_ = 0.1f;

    public:
        void DrawOnto(Canvas &canvas) override;

        std::vector<Line> &GetLines()
        {
            return this->plots_;
        }

        void AddPlot(std::string const &function)
        {
            this->plots_.emplace_back(this->compiler_.CompileFunction<double>(function), this->domain_);
        }

        Graph(std::initializer_list<std::string> functions, glm::vec2 domain = {-100.0, 100.0}) : domain_(domain)
        {
            for(auto const &function : functions)
            {
                this->AddPlot(function);
            }
        }

        Graph(std::string const &function, glm::vec2 domain = {-100.0, 100.0}) : Graph(std::initializer_list<std::string>{function}, domain) {}

        Graph(glm::vec2 domain = {-100.0, 100.0}) : domain_(domain) {};
    };
}

#endif //UNLOGIC_GRAPH_H
