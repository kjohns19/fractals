#include <fractals/fractal/julia.hpp>
#include <fractals/util/view.hpp>

#include <SFML/Graphics.hpp>

Julia::Julia(const sf::Vector2u& size, long double x, long double y):
    Fractal(size),
    d_x(x),
    d_y(y) {}

void Julia::resetPoint(long double x, long double y, Point& point)
{
    point.x = x;
    point.y = y;
}

void Julia::setValue(long double x, long double y)
{
    if (d_x != x || d_y != y)
    {
        d_x = x;
        d_y = y;
        int it = iterations();
        setView(getView());
        iterate(it);
    }
}

std::unique_ptr<Fractal> Julia::clone(const sf::Vector2u& size, const View& view) const
{
    std::unique_ptr<Fractal> fractal = std::make_unique<Julia>(size, d_x, d_y);
    fractal->setView(view);
    return fractal;
}

void Julia::doIterate(
        int count,
        const std::vector<size_t>& valid,
        std::vector<Fractal::Point>& points,
        size_t startPoint,
        size_t numPoints,
        std::vector<size_t>& done)
{
    for(size_t j = 0; j < numPoints; j++)
    {
        size_t index = valid[startPoint + j];
        auto& point = points[index];
        double nx, ny;

        for(int i = 0; i < count; i++)
        {
            if (point.x*point.x + point.y*point.y < (1 << 16))
            {
                nx = point.x*point.x - point.y*point.y + d_x;
                ny = 2*point.x*point.y + d_y;
                if (point.x == nx && point.y == ny)
                {
                    point.value+=count-i;
                    point.remove = true;
                    done.push_back(index);
                    break;
                }
                point.x = nx;
                point.y = ny;
                point.value++;
            }
            else
            {
                point.remove = true;
                break;
            }
        }
    }
}
