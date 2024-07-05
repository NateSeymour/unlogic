#include "Graph.h"

void unlogic::Graph::DrawOnto(Canvas &canvas)
{
    for(auto &plot : this->plots_)
    {
        plot.DrawOnto(canvas);
    }
}
