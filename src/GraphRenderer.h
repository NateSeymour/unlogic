//
// Created by nathan on 5/27/24.
//

#ifndef UNLOGIC_GRAPHRENDERER_H
#define UNLOGIC_GRAPHRENDERER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "MathFunction.h"

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
    EvaluationContext &context_;
    MathFunction &function_;
    Range range_;
    double step_;
    sf::Vector2f scalar_ = {1.0, 1.0};

    std::vector<sf::Vertex> vertices_;
    std::vector<sf::Vector2f> points_;

    double thickness = 100.0;

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
            std::cout << point_count << std::endl;
            this->points_.clear();
            this->points_.reserve(point_count);
            for (std::size_t i = 0; i < point_count; i++)
            {
                double x = this->range_.start + (i * this->step_);
                this->context_.parameters["x"] = x;
                double y = this->function_.Evaluate(this->context_) * -1;

                this->points_.emplace_back(x * this->scalar_.x, y * this->scalar_.y);
            }

            // Calculate triangle fan
            sf::Color colors[] = { sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Magenta };

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

                double theta1 = std::atan2(dy, dx) * (180.0/3.1415);
                double theta2 = (90.0 - theta1) * 3.1415*180.0;

                double tx = (this->thickness / 2) * std::cos(theta2);
                double ty = (this->thickness / 2) * std::sin(theta2);

                std::cout << this->points_[i].x << " " << tx << " " << ty << std::endl;

                auto const &point = this->points_[i];

                auto color = colors[i % 4];

                this->vertices_.emplace_back(sf::Vector2f(point.x + tx, point.y + ty), color); // a1
                this->vertices_.emplace_back(sf::Vector2f(point.x - tx, point.y - ty), color); // a2
            }

            this->clean_ = true;
        }

        return this->vertices_;
    }

    explicit Curve(MathFunction &function, EvaluationContext &context, Range range, double step) : function_(function), context_(context),
                                                                                                   range_(range), step_(step) {}
};

class GraphRenderer
{
    EvaluationContext context_;
    MathFunction function_;
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

    explicit GraphRenderer(std::string function, Range range = {-10, 10}, double step = 0.1)
        : function_(std::move(function)),
          range_(range),
          step_(step),
          curve_(this->function_, this->context_, range, step) {}
};


#endif //UNLOGIC_GRAPHRENDERER_H
