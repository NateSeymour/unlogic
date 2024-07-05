#ifndef UNLOGIC_PLOT_H
#define UNLOGIC_PLOT_H

#include <cmath>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "Compiler.h"
#include "graphic/Drawable.h"

namespace unlogic
{
    class Plot : public Shape
    {
        unlogic::Callable<double> function_;
        glm::vec4 color_;
        double precision_ = 0.1;
        double thickness_ = 0.1;
        std::string title_ = "My Plot";
        glm::vec2 domain_;

    protected:
        void GenerateVertices(std::vector<Vertex> &vertices) override;

    public:
        Plot(unlogic::Callable<double> function, glm::vec2 domain, glm::vec4 color = {1.f, 0.f, 0.f, 1.f}) : function_(function), domain_(domain), color_(color) {}
    };
}

#endif //UNLOGIC_PLOT_H
