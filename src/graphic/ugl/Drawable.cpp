#include "Drawable.h"
#include "calculator/ui/CanvasArea.h"

void unlogic::Shape::DrawOnto(Canvas &canvas)
{
    canvas.DrawPrimitive(this->GetVertexBuffer());
}

void unlogic::Shape::SetPrecision(float precision)
{
    this->precision_ = precision;

    this->Update();
}

void unlogic::Shape::SetColor(const glm::vec4 &color)
{
    this->color_ = color;

    this->Update();
}

void unlogic::Shape::SetLineThickness(float thickness)
{
    this->line_thickness_ = thickness;

    this->Update();
}

void unlogic::Shape::Update()
{
    this->vbo_ = std::move(this->GenerateVertexBuffer());
}
