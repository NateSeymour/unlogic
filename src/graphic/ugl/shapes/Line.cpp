#include <numbers>
#include "Line.h"

std::shared_ptr<unlogic::VertexBuffer> unlogic::Line::GenerateVertexBuffer() const
{
    std::vector<Vertex> vertices;
    vertices.reserve(this->points_.size() * 2);
    double dx = 0.0;
    double dy = 0.0;
    for(std::size_t i = 0; i < this->points_.size(); i++)
    {
        if(i < this->points_.size() - 1)
        {
            dx = this->points_[i + 1].x - this->points_[i].x;
            dy = this->points_[i + 1].y - this->points_[i].y;
        }

        double theta1 = std::atan2(dy, dx);
        double theta2 = ((std::numbers::pi/2) - theta1);

        double tx = (this->line_thickness_ / 2) * std::cos(theta2);
        double ty = (this->line_thickness_ / 2) * std::sin(theta2);

        auto const &point = this->points_[i];

        vertices.emplace_back(glm::vec2(point.x + tx, point.y - ty), this->color_); // a1
        vertices.emplace_back(glm::vec2(point.x - tx, point.y + ty), this->color_); // a2
    }

    return std::make_shared<unlogic::VertexBuffer>(vertices, PrimitiveType::kTriangleStrip);
}

void unlogic::Line::SetFunction(const unlogic::Executable &function, const glm::vec2 &domain)
{
    std::size_t point_count = std::ceil((domain.y - domain.x) / this->precision_);
    this->points_.clear();
    this->points_.reserve(point_count);
    for (std::size_t i = 0; i < point_count; i++)
    {
        double x = domain.x + ((double)i * this->precision_);
        double y = 0.0; // function(x);

        this->points_.emplace_back(x, y);
    }

    this->Update();
}

void unlogic::Line::SetPoints(const std::vector<glm::vec2> &points)
{
    this->points_ = points;

    this->Update();
}
