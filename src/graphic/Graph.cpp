#include <numbers>
#include "Graph.h"

void Plot::update()
{
    // Calculate points
    std::size_t point_count = std::ceil((this->domain_.y - this->domain_.x) / this->precision_);
    this->points_.clear();
    this->points_.reserve(point_count);
    for (std::size_t i = 0; i < point_count; i++)
    {
        double x = this->domain_.x + ((double)i * this->precision_);
        double y = this->function_(x);

        this->points_.emplace_back(x, y);
    }

    // Calculate triangle strip
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
        double theta2 = ((std::numbers::pi/2) - theta1);

        double tx = (this->thickness_ / 2) * std::cos(theta2);
        double ty = (this->thickness_ / 2) * std::sin(theta2);

        auto const &point = this->points_[i];

        this->vertices_.emplace_back(mf::Vector2f(point.x + tx, point.y - ty), this->color_); // a1
        this->vertices_.emplace_back(mf::Vector2f(point.x - tx, point.y + ty), this->color_); // a2
    }
}
