//
// Created by nathan on 5/27/24.
//

#ifndef UNLOGIC_GRAPHRENDERER_H
#define UNLOGIC_GRAPHRENDERER_H

#include <SFML/Graphics.hpp>
#include <cmath>
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

class CurveShape : public sf::Shape
{
    EvaluationContext &context_;
    MathFunction &function_;
    Range range_;
    double step_;

public:
    std::size_t getPointCount() const override
    {
        return std::ceil(std::ceil(this->range_.Length()) / this->step_);
    }

    sf::Vector2f getPoint(std::size_t index) const override
    {
        double x = this->range_.start + index * this->step_;
        this->context_.parameters["x"] = x;
        return {(float)x, (float)this->function_.Evaluate(this->context_) * -1};
    }

    CurveShape(MathFunction &function, EvaluationContext &context, Range range, double step) : function_(function), context_(context), range_(range), step_(step) {}
};

class GraphRenderer
{
    EvaluationContext context_;
    MathFunction function_;
    Range range_;
    double step_;
    sf::View view_;
    CurveShape curve_;

public:
    void Draw(sf::RenderTarget &surface)
    {
        // Clear surface for rendering
        surface.clear(sf::Color::White);

        // Setup view
        auto surface_size = surface.getSize();
        this->view_.reset(sf::FloatRect(surface_size.x * -0.5, surface_size.y * -0.5, surface_size.x, surface_size.y));

        surface.setView(this->view_);

        // Draw Axis
        float axis_width = 8.f;
        float axis_margin = 0.f;

        sf::RectangleShape x_axis({(float)surface_size.x - axis_margin * 2, axis_width});
        x_axis.setFillColor(sf::Color::Black);
        x_axis.setPosition(surface_size.x * -0.5f + axis_margin,axis_width * -0.5f);

        sf::RectangleShape y_axis({axis_width, (float)surface_size.y - axis_margin * 2});
        y_axis.setFillColor(sf::Color::Black);
        y_axis.setPosition(axis_width * -0.5f, surface_size.y * -0.5f + axis_margin);

        surface.draw(x_axis);
        surface.draw(y_axis);

        // Draw curve
        this->curve_.setFillColor(sf::Color::Transparent);
        this->curve_.setOutlineColor(sf::Color::Red);
        this->curve_.setOutlineThickness(10.f);
        surface.draw(this->curve_);
    }

    explicit GraphRenderer(std::string function, Range range = {-100, 100}, double step = 1)
        : function_(std::move(function)),
          range_(range),
          step_(step),
          curve_(this->function_, this->context_, range, step) {}
};


#endif //UNLOGIC_GRAPHRENDERER_H
