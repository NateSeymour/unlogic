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
        std::vector<Plot> plots_;
        sf::Vector2f center_ = {0.0, 0.0};
        sf::Vector2f domain_;

    protected:
        void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    public:
        Graph(std::initializer_list<std::string> functions, sf::Vector2f domain = {-10.0, 10.0}) : domain_(domain)
        {
            sf::Color color_wheel[] = {sf::Color::Red, sf::Color::Blue, sf::Color::Cyan, sf::Color::Green};
            int wheel_index = 0;

            unlogic::Compiler compiler;
            for(auto const &function : functions)
            {
                this->plots_.emplace_back(compiler.CompileFunction<double>(function), domain, color_wheel[(wheel_index++)%sizeof(color_wheel)]);
            }
        }

        Graph(std::string const &function, sf::Vector2f domain = {-10.0, 10.0}) : Graph(std::initializer_list<std::string>{function}, domain) {}

        Graph() = delete;
    };
}

#endif //UNLOGIC_GRAPH_H