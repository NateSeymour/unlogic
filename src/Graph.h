//
// Created by nathan on 5/27/24.
//

#ifndef UNLOGIC_GRAPH_H
#define UNLOGIC_GRAPH_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "Program.h"

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
    unlogic::Function function_;
    sf::Color color_ = sf::Color::Red;
    double precision_ = 0.1;
    double thickness_ = 0.1;
    std::string title_ = "My Plot";

    sf::Vector2f domain_;

    std::vector<sf::Vertex> vertices_;
    std::vector<sf::Vector2f> points_;

protected:
    void update()
    {
        // Calculate points
        std::size_t point_count = std::ceil((this->domain_.y - this->domain_.x) / this->precision_);
        this->points_.clear();
        this->points_.reserve(point_count);
        for (std::size_t i = 0; i < point_count; i++)
        {
            double x = this->domain_.x + ((double)i * this->precision_);
            double y = this->function_({x});

            this->points_.emplace_back(x, y);
        }

        // Calculate triangle fan
        this->vertices_.clear();
        this->vertices_.reserve(point_count * 2);
        double dx = 0.0;
        double dy = 0.0;
        for(std::size_t i = 0; i < point_count; i++)
        {
            if(i < point_count - 1)
            {
                dx = this->points_[i + 1].x - this->points_[i].x;
                dy = this->points_[i + 1].y - this->points_[i].y;
            }

            double theta1 = std::atan2(dy, dx);
            double theta2 = ((3.1415/2) - theta1);

            double tx = (this->thickness_ / 2) * std::cos(theta2);
            double ty = (this->thickness_ / 2) * std::sin(theta2);

            auto const &point = this->points_[i];

            this->vertices_.emplace_back(sf::Vector2f(point.x + tx, point.y - ty), sf::Color::Red); // a1
            this->vertices_.emplace_back(sf::Vector2f(point.x - tx, point.y + ty), sf::Color::Red); // a2
        }
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        target.draw(this->vertices_.data(), this->vertices_.size(), sf::TrianglesStrip, states);
    }

public:
    Plot(unlogic::Function function, sf::Vector2f domain) : function_(std::move(function)), domain_(domain)
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
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        // Save reference to old view
        auto old_view = target.getView();
        auto target_size = target.getSize();

        // Create new view
        auto width = this->domain_.y - this->domain_.x;
        auto height = width * ((float)target_size.y / (float)target_size.x);
        target.setView(sf::View(this->center_, {width, height}));

        // Draw Axis
        float axis_thickness = width / 100.f;
        sf::RectangleShape x_axis({width, axis_thickness});
        x_axis.setFillColor(sf::Color::Black);
        x_axis.setPosition(this->domain_.x, 0.f - axis_thickness / 2.f);

        target.draw(x_axis, states);

        sf::RectangleShape y_axis({axis_thickness, height});
        y_axis.setFillColor(sf::Color::Black);
        y_axis.setPosition(0.f - axis_thickness / 2.f, height / -2.f);

        target.draw(y_axis, states);

        // Draw Plots
        for(auto const &plot : this->plots_)
        {
            target.draw(plot, states);
        }

        // Return to old view
        target.setView(old_view);
    }

public:
    Graph(std::initializer_list<std::string> functions, sf::Vector2f domain = {-10.0, 10.0}) : domain_(domain)
    {
        for(auto const &function : functions)
        {
            this->plots_.emplace_back(unlogic::compile(function), domain);
        }
    }
};


#endif //UNLOGIC_GRAPH_H
