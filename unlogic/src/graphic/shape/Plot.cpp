//
// Created by Nathan on 11/14/2024.
//
#include <numbers>
#include <vector>
#include "Plot.h"

using namespace unlogic;

Plot2d::Plot2d(std::string name, Plot2dFunctionType fn, Color color) : name_(std::move(name)), fn_(fn), color_(color)
{
    std::size_t point_count = std::ceil((this->domain_.y - this->domain_.x) / this->precision_);

    std::vector<glm::vec2> points;
    points.reserve(point_count);
    for (std::size_t i = 0; i < point_count; i++)
    {
        double x = this->domain_.x + ((double)i * this->precision_);
        double y = this->fn_(x);

        points.emplace_back(x, y);
    }

    this->vertices.clear();
    this->vertices.reserve(points.size() * 2);
    double dx = 0.0;
    double dy = 0.0;
    for (std::size_t i = 0; i < points.size(); i++)
    {
        if (i < points.size() - 1)
        {
            dx = points[i + 1].x - points[i].x;
            dy = points[i + 1].y - points[i].y;
        }

        double theta1 = std::atan2(dy, dx);
        double theta2 = ((std::numbers::pi / 2) - theta1);

        double tx = (this->line_thickness_ / 2) * std::cos(theta2);
        double ty = (this->line_thickness_ / 2) * std::sin(theta2);

        auto const &point = points[i];

        this->vertices.push_back({glm::vec2(point.x + tx, point.y - ty), this->color_}); // a1
        this->vertices.push_back({glm::vec2(point.x - tx, point.y + ty), this->color_}); // a2
    }
}
