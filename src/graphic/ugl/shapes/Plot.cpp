#include <numbers>
#include "Plot.h"

std::shared_ptr<unlogic::VertexBuffer> unlogic::Plot::GenerateVertexBuffer() const
{
    // Calculate points
    std::vector<glm::vec2> points;

    std::size_t point_count = std::ceil((this->domain_.y - this->domain_.x) / this->precision_);
    points.clear();
    points.reserve(point_count);
    for (std::size_t i = 0; i < point_count; i++)
    {
        double x = this->domain_.x + ((double)i * this->precision_);
        double y = this->function_(x);

        points.emplace_back(x, y);
    }

    // Calculate triangle strip
    std::vector<Vertex> vertices;
    vertices.reserve(point_count * 2);
    double dx = 0.0;
    double dy = 0.0;
    for(std::size_t i = 0; i < point_count; i++)
    {
        if(i < point_count - 1)
        {
            dx = points[i + 1].x - points[i].x;
            dy = points[i + 1].y - points[i].y;
        }

        double theta1 = std::atan2(dy, dx);
        double theta2 = ((std::numbers::pi/2) - theta1);

        double tx = (this->thickness_ / 2) * std::cos(theta2);
        double ty = (this->thickness_ / 2) * std::sin(theta2);

        auto const &point = points[i];

        vertices.emplace_back(glm::vec2(point.x + tx, point.y - ty), this->color_); // a1
        vertices.emplace_back(glm::vec2(point.x - tx, point.y + ty), this->color_); // a2
    }

    return std::make_shared<unlogic::VertexBuffer>(vertices);
}
