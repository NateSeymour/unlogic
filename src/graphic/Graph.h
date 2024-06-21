//
// Created by nathan on 5/27/24.
//

#ifndef UNLOGIC_GRAPH_H
#define UNLOGIC_GRAPH_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "../Compiler.h"

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

    class Plot : public sf::Drawable, public sf::Transformable
    {
        unlogic::Callable<double> function_;
        sf::Color color_;
        double precision_ = 0.1;
        double thickness_ = 0.1;
        std::string title_ = "My Plot";

        sf::Vector2f domain_;

        std::vector<sf::Vertex> vertices_;
        std::vector<sf::Vector2f> points_;

    protected:
        void update();
        void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    public:
        Plot(unlogic::Callable<double> function, sf::Vector2f domain, sf::Color color = sf::Color::Red) : function_(std::move(function)), domain_(domain), color_(color)
        {
            this->update();
        }
    };

    class Graph : public sf::Drawable, public sf::Transformable
    {
        unlogic::Compiler compiler_;
        std::vector<Plot> plots_;
        sf::Vector2f domain_;
        sf::View view_;
        float axis_thickness_ = 0.1f;

        sf::Color color_wheel_[4] = {sf::Color::Red, sf::Color::Blue, sf::Color::Cyan, sf::Color::Green};
        int wheel_index_ = 0;

    protected:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    public:
        void AddPlot(std::string const &function)
        {
            auto color = this->color_wheel_[(this->wheel_index_++)%sizeof(this->color_wheel_)];
            this->plots_.emplace_back(this->compiler_.CompileFunction<double>(function), this->domain_, color);
        }

        Graph(std::initializer_list<std::string> functions, sf::Vector2f domain = {-10.0, 10.0}) : domain_(domain)
        {
            for(auto const &function : functions)
            {
                this->AddPlot(function);
            }
        }

        Graph(std::string const &function, sf::Vector2f domain = {-10.0, 10.0}) : Graph(std::initializer_list<std::string>{function}, domain) {}

        Graph(sf::Vector2f domain = {-10.0, 10.0}) : domain_(domain) {};
    };
}

#endif //UNLOGIC_GRAPH_H
