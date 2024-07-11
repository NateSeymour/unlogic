#include "Drawable.h"
#include "calculator/ui/CanvasArea.h"

void unlogic::Shape::DrawOnto(Canvas &canvas)
{
    canvas.DrawPrimitive(this->GetVertexBuffer());
}
