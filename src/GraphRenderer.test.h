//
// Created by nathan on 5/27/24.
//

#ifndef UNLOGIC_GRAPHRENDERER_TEST_H
#define UNLOGIC_GRAPHRENDERER_TEST_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "Program.h"

struct Range
{
    double start;
    double end;

    double Length() const
    {
        return this->end - this->start;
    }
};

class Curve
{
    unlogic::Function &function_;
    Range range_;
    double step_;
    sf::Vector2f scalar_ = {1.0, 1.0};

    std::vector<sf::Vertex> vertices_;
    std::vector<sf::Vector2f> points_;

    double thickness = 10.0;

    bool clean_ = false;

    void Update()
    {
        this->clean_ = false;
    }

public:
    void SetScalar(sf::Vector2f const &scalar)
    {
        this->scalar_ = scalar;
        this->Update();
    }

    [[nodiscard]] std::vector<sf::Vertex> const &GetVertexBuffer()
    {
        if(!this->clean_)
        {
            // Calculate points
            std::size_t point_count = std::ceil(this->range_.Length() / this->step_);
            this->points_.clear();
            this->points_.reserve(point_count);
            for (std::size_t i = 0; i < point_count; i++)
            {
                double x = this->range_.start + (i * this->step_);
                double y = this->function_({x});

                this->points_.emplace_back(x * this->scalar_.x, y * this->scalar_.y);
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

                double tx = (this->thickness / 2) * std::cos(theta2);
                double ty = (this->thickness / 2) * std::sin(theta2);

                auto const &point = this->points_[i];

                this->vertices_.emplace_back(sf::Vector2f(point.x + tx, point.y - ty), sf::Color::Red); // a1
                this->vertices_.emplace_back(sf::Vector2f(point.x - tx, point.y + ty), sf::Color::Red); // a2
            }

            this->clean_ = true;
        }

        return this->vertices_;
    }

    explicit Curve(unlogic::Function &function, Range range, double step) : function_(function), range_(range), step_(step) {}
};

class GraphRenderer
{
    unlogic::Function function_;
    Range range_;
    double step_;
    sf::View view_;
    Curve curve_;
    sf::Vector2u old_surface_size_;
    sf::Vector2f viewport_plane_;

public:
    void Draw(sf::RenderTarget &surface) {
        // Clear surface for rendering
        surface.clear(sf::Color::White);

        // Setup view
        auto surface_size = surface.getSize();

        if (surface_size != this->old_surface_size_)
        {
            this->viewport_plane_ = sf::Vector2f((float)surface_size.x, (float)surface_size.y);
            this->view_.reset(sf::FloatRect(viewport_plane_.x * -0.5f, viewport_plane_.y * -0.5f, viewport_plane_.x, viewport_plane_.y));

            surface.setView(this->view_);

            this->curve_.SetScalar(sf::Vector2f(viewport_plane_.x / this->range_.Length(), viewport_plane_.y / this->range_.Length()));

            this->old_surface_size_ = surface_size;
        }

        // Draw Axis
        float axis_width = 8.f;
        float axis_margin = 0.f;

        sf::RectangleShape x_axis({(float)viewport_plane_.x - axis_margin * 2, axis_width});
        x_axis.setFillColor(sf::Color::Black);
        x_axis.setPosition(viewport_plane_.x * -0.5f + axis_margin, axis_width * -0.5f);

        sf::RectangleShape y_axis({axis_width, (float)viewport_plane_.y - axis_margin * 2});
        y_axis.setFillColor(sf::Color::Black);
        y_axis.setPosition(axis_width * -0.5f, viewport_plane_.y * -0.5f + axis_margin);

        surface.draw(x_axis);
        surface.draw(y_axis);

        // Draw curve
        auto buffer = this->curve_.GetVertexBuffer();
        surface.draw(buffer.data(), buffer.size(), sf::TriangleStrip);
    }

    explicit GraphRenderer(std::string const &function, Range range = {-10, 10}, double step = 0.1)
        : function_(unlogic::compile(function)), range_(range), step_(step), curve_(this->function_, range, step) {}
};


#endif //UNLOGIC_GRAPHRENDERER_TEST_H
