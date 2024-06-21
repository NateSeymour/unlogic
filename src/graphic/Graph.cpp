#include <numbers>
#include "Graph.h"

using namespace unlogic;

void Graph::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Create new view
    sf::View const &view = target.getView();
    auto [width, height] = view.getSize();

    sf::Vector2f const &center = view.getCenter();
    sf::Vector2f domain(center.x - (width / 2), center.x + (width / 2));
    sf::Vector2f range(center.y - (height / 2), center.y + (height / 2));

    // Draw Axis
    sf::RectangleShape x_axis({width, this->axis_thickness_});
    x_axis.setFillColor(sf::Color::Black);
    x_axis.setPosition(domain.x, 0.f - this->axis_thickness_ / 2.f);

    target.draw(x_axis, states);

    sf::RectangleShape y_axis({this->axis_thickness_, height});
    y_axis.setFillColor(sf::Color::Black);
    y_axis.setPosition(0.f - this->axis_thickness_ / 2.f, range.y * -1);

    target.draw(y_axis, states);

    // Draw Gridlines
    auto grid_thickness = this->axis_thickness_ / 2.f;

    // X gridlines
    for(int i = std::floor(domain.x); i < (int)std::ceil(domain.y); i++)
    {
        sf::RectangleShape gridline({grid_thickness, height});
        gridline.setFillColor(sf::Color(0, 0, 0, 10));
        gridline.setPosition(i - (grid_thickness / 2.f), range.y * -1);

        target.draw(gridline, states);
    }

    // Y gridlines
    for(int i = std::floor(range.x); i < (int)std::ceil(range.y); i++)
    {
        sf::RectangleShape gridline({width, grid_thickness});
        gridline.setFillColor(sf::Color(0, 0, 0, 10));
        gridline.setPosition(domain.x, (i*-1) - (this->axis_thickness_ / 2.f));

        target.draw(gridline, states);
    }

    // Draw Plots
    for(auto const &plot : this->plots_)
    {
        target.draw(plot, states);
    }
}

void Plot::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(this->vertices_.data(), this->vertices_.size(), sf::TrianglesStrip, states);
}

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
        double theta2 = ((std::numbers::pi/2) - theta1);

        double tx = (this->thickness_ / 2) * std::cos(theta2);
        double ty = (this->thickness_ / 2) * std::sin(theta2);

        auto const &point = this->points_[i];

        this->vertices_.emplace_back(sf::Vector2f(point.x + tx, point.y - ty), this->color_); // a1
        this->vertices_.emplace_back(sf::Vector2f(point.x - tx, point.y + ty), this->color_); // a2
    }
}
