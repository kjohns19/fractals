#include <fractals/fractal/Mandelbrot.hpp>
#include <fractals/color/ColorScheme.hpp>
#include <iostream>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics.hpp>

Mandelbrot::Mandelbrot(const sf::Vector2u& size):
    Fractal(size) {}

void Mandelbrot::resetPoint(long double x, long double y, Point& point)
{
    point.x = 0;
    point.y = 0;
}

std::unique_ptr<Fractal> Mandelbrot::clone(const sf::Vector2u& size, const View& view) const
{
    std::unique_ptr<Fractal> fractal(new Mandelbrot(size));
    fractal->setView(view);
    return fractal;
}

void Mandelbrot::doIterate(
        int count,
        const std::vector<size_t>& valid,
        std::vector<Fractal::Point>& points,
        size_t startPoint,
        size_t numPoints,
        std::vector<size_t>& done)
{
    auto& view = getView();
    auto& size = getSize();
    for(size_t j = 0; j < numPoints; j++)
    {
        size_t index = valid[startPoint + j];
        auto& point = points[index];

        long double cx = view.width;
        long double cy = view.height;

        cx = cx * (index % size.x) / size.x + view.left;
        cy = cy * (index / size.x) / size.y + view.top;
        long double newx, newy;

        for(int i = 0; i < count; i++)
        {
            if (point.x*point.x + point.y*point.y < (1 << 16))
            {
                newx = point.x*point.x - point.y*point.y + cx;
                newy = 2*point.x*point.y + cy;
                if (point.x == newx && point.y == newy)
                {
                    point.value+=count-i;
                    point.remove = true;
                    done.push_back(index);
                    break;
                }
                point.x = newx;
                point.y = newy;
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
