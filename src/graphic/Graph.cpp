#include <cmath>
#include "graphic/ugl/Canvas.h"
#include "graphic/Graph.h"

void unlogic::Graph::DrawOnto(unlogic::Canvas &canvas)
{
    auto world_bounds = canvas.GetWorldBounds();

    // x, y gridlines
    float gridline_thickness = this->axis_thickness_;
    float gray_intensity = 0.93f;
    for(int x = std::floor(world_bounds.x); x < std::ceil(world_bounds.z); x++)
    {
        Line gridline({(float)x, world_bounds.y}, {(float)x, world_bounds.w});
        gridline.SetLineThickness(gridline_thickness);
        gridline.SetColor({gray_intensity, gray_intensity, gray_intensity, 1.f});
        gridline.DrawOnto(canvas);
    }

    for(int y = std::floor(world_bounds.w); y < std::ceil(world_bounds.y); y++)
    {
        Line gridline({world_bounds.x, (float)y}, {world_bounds.z, (float)y});
        gridline.SetLineThickness(gridline_thickness);
        gridline.SetColor({gray_intensity, gray_intensity, gray_intensity, 1.f});
        gridline.DrawOnto(canvas);
    }

    // Draw x, y axis
    Line x_axis({world_bounds.x, 0.f}, {world_bounds.z, 0.f});
    x_axis.SetLineThickness(this->axis_thickness_);
    x_axis.SetColor({0.f, 0.f, 0.f, 1.f});
    x_axis.DrawOnto(canvas);

    Line y_axis({0.f, world_bounds.y}, {0.f, world_bounds.w});
    y_axis.SetLineThickness(this->axis_thickness_);
    y_axis.SetColor({0.f, 0.f, 0.f, 1.f});
    y_axis.DrawOnto(canvas);

    // Draw plots
    for(auto &plot : this->plots_)
    {
        plot.DrawOnto(canvas);
    }
}
