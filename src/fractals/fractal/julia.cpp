#include <fractals/fractal/julia.hpp>
#include <fractals/util/view.hpp>

#include <SFML/Graphics.hpp>

#include <cmath>

namespace frac {

Julia::Julia(const sf::Vector2u& size, long double x, long double y, long double power)
: Fractal(size)
, d_x(x)
, d_y(y)
, d_power(power) {}

void Julia::resetPoint(long double x, long double y, Point& point)
{
    point.x = x;
    point.y = y;
}

void Julia::setValue(long double x, long double y, long double power)
{
    if (d_x != x || d_y != y || d_power != power)
    {
        d_x = x;
        d_y = y;
        d_power = power;
        int it = iterations();
        setView(getView());
        iterate(it);
    }
}

std::unique_ptr<Fractal> Julia::clone(const sf::Vector2u& size, const View& view) const
{
    std::unique_ptr<Fractal> fractal = std::make_unique<Julia>(size, d_x, d_y, d_power);
    fractal->setView(view);
    fractal->setDrawMode(getDrawMode());
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
                double powVal = std::pow(point.x*point.x + point.y*point.y, d_power/2);
                double tanVal = d_power * std::atan2(point.y, point.x);
                nx = powVal * std::cos(tanVal) + d_x;
                ny = powVal * std::sin(tanVal) + d_y;
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

} // close namespace frac
