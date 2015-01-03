#include <fractals/fractal/Julia.hpp>
#include <fractals/color/ColorScheme.hpp>

#include <iostream>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

Julia::Julia(const sf::Vector2u& size, const sf::Rect<double>& view, const Func& func):
    d_points(size.x * size.y),
    d_valid(size.x * size.y),
    d_size(size),
    d_func(func),
    d_iterations(0)
{
    setView(view);
}

void Julia::setView(const sf::Rect<double>& view)
{
    d_valid.resize(d_size.x * d_size.y);

    for(unsigned x = 0; x < d_size.x; x++)
        for(unsigned y = 0; y < d_size.y; y++)
        {
            size_t index = x * d_size.y + y;
            d_valid[index] = index;

            Point& point = d_points[index];
            point.z.real(view.width * x / d_size.x + view.left);
            point.z.imag(view.height * y / d_size.y + view.top);
            point.value = 0;
        }
    d_iterations = 0;
}

void Julia::iterate(int count)
{
    for(size_t j = 0; j < d_valid.size(); j++)
    {
        size_t index = d_valid[j];
        Point& point = d_points[index];
        //double tmp;

        std::complex<double> zn;
        for(int i = 0; i < count; i++)
        {
            if (d_func(point.z, zn))
            {
                point.z = zn;
                point.value++;
            }
            else
            {
                d_valid[j] = d_valid.back();
                d_valid.pop_back();
                j--;
                break;
            }
        }
    }
    d_iterations+=count;
}

void Julia::draw(sf::RenderTarget& target, const ColorScheme& cs)
{
    /*
    if (image.getSize() != d_size)
        image.create(d_size.x, d_size.y);

    std::vector<sf::Color> colors;
    for(int i = 0; i <= d_iterations; i++)
        colors.push_back(cs.color(1.0 * i / d_iterations));

    if (d_iterations > 0)
        for(unsigned x = 0; x < d_size.x; x++)
            for(unsigned y = 0; y < d_size.y; y++)
            {
                size_t index = x * d_size.y + y;
                image.setPixel(x, y, colors[d_points[index].value]);
            }
    */
}
