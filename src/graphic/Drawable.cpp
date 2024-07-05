#include "graphic/Drawable.h"
#include "calculator/ui/CanvasArea.h"

void unlogic::Shape::DrawOnto(Canvas &canvas)
{
    canvas.DrawPrimitive(this->GetVertices(), this->primitive_type);
}
