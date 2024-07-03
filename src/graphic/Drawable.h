#ifndef UNLOGIC_DRAWABLE_H
#define UNLOGIC_DRAWABLE_H

#include <vector>

namespace unlogic
{
    template<typename T>
    struct Vec2
    {
        T x;
        T y;

        Vec2(T x, T y) : x(x), y(y) {}
    };

    typedef Vec2<float> Vec2f;
    typedef Vec2<double> Vec2d;
    typedef Vec2<int> Vec2i;
    typedef Vec2<unsigned> Vec2u;

    struct Vertex
    {
        Vec2f position;

        Vertex(Vec2f const &position) : position(position) {}
    };

    class Drawable
    {
        std::vector<Vertex> vertices_;

    public:
        virtual bool UpdateRequired() const = 0;
        virtual void Update(std::vector<Vertex> &vertices) const = 0;

        void Draw()
        {
            if(this->UpdateRequired())
            {
                this->Update(this->vertices_);
            }
        }
    };
}

#endif //UNLOGIC_DRAWABLE_H
