//
// Created by nathan on 5/27/24.
//

#ifndef UNLOGIC_GRAPH_H
#define UNLOGIC_GRAPH_H

#include <cmath>
#include "Compiler.h"
#include "graphic/RenderTarget.h"
#include "graphic/Drawable.h"

namespace unlogic
{
    struct Range
    {
        double start;
        double end;

        constexpr double Length() const
        {
            return this->end - this->start;
        }
    };

    class Plot : public mf::Drawable
    {
        unlogic::Callable<double> function_;
        mf::Color color_;
        double precision_ = 0.1;
        double thickness_ = 0.1;
        std::string title_ = "My Plot";

        mf::Vector2f domain_;

        std::vector<mf::Vertex> vertices_;
        std::vector<mf::Vector2f> points_;

    protected:
        void update();
        void Draw(mf::RenderTarget &target) const override;

    public:
        Plot(unlogic::Callable<double> function, mf::Vector2f domain, mf::Color color = mf::Color::Red) : function_(std::move(function)), domain_(domain), color_(color)
        {
            this->update();
        }
    };

    class Graph : public mf::Drawable
    {
        unlogic::Compiler compiler_;
        std::vector<Plot> plots_;
        mf::Vector2f domain_;
        mf::View view_;
        float axis_thickness_ = 0.1f;

        mf::Color color_wheel_[3] = {mf::Color::Red, mf::Color::Blue, mf::Color::Green};
        int wheel_index_ = 0;

    protected:
        void Draw(mf::RenderTarget &target) const override;

    public:
        void AddPlot(std::string const &function)
        {
            auto color = this->color_wheel_[(this->wheel_index_++)%sizeof(this->color_wheel_)];
            this->plots_.emplace_back(this->compiler_.CompileFunction<double>(function), this->domain_, color);
        }

        Graph(std::initializer_list<std::string> functions, mf::Vector2f domain = {-100.0, 100.0}) : domain_(domain)
        {
            for(auto const &function : functions)
            {
                this->AddPlot(function);
            }
        }

        Graph(std::string const &function, mf::Vector2f domain = {-100.0, 100.0}) : Graph(std::initializer_list<std::string>{function}, domain) {}

        Graph(mf::Vector2f domain = {-100.0, 100.0}) : domain_(domain) {};
    };
}

#endif //UNLOGIC_GRAPH_H
