#include <fractals/fractal/Julia.hpp>
#include <fractals/color/ColorScheme.hpp>

#include <iostream>

#include <SFML/Graphics.hpp>

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
            point.x = view.width * x / d_size.x + view.left;
            point.y = view.height * y / d_size.y + view.top;
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

        double nx, ny;
        for(int i = 0; i < count; i++)
        {
            if (d_func(point.x, point.y, nx, ny))
            {
                point.x = nx;
                point.y = ny;
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

void Julia::getColors(const ColorScheme& cs, std::vector<sf::Color>& colors)
{
    for(int i = 0; i <= d_iterations; i++)
        colors.push_back(cs.getColor(1.0 * i / d_iterations));
}

void Julia::draw(sf::RenderTarget& target, const ColorScheme& cs)
{
    std::vector<sf::Color> colors;
    getColors(cs, colors);

    sf::VertexArray vertices(sf::Points, d_size.x * d_size.y);

    if (d_iterations > 0)
        for(unsigned x = 0; x < d_size.x; x++)
            for(unsigned y = 0; y < d_size.y; y++)
            {
                size_t index = x * d_size.y + y;
                vertices[index].position = sf::Vector2f(x, y);

                if (getDrawMode())
                    vertices[index].color = colors[d_points[index].value];
                else
                {
                    Point& point = d_points[index];
                    if (point.value == d_iterations)
                        vertices[index].color = colors[d_iterations];
                    else
                    {
                        double it = point.value;
                        double zn = point.x*point.x + point.y*point.y;
                        double nu = std::log2(std::log2(zn)/2);
                        it += 1 - nu;

                        sf::Color col1 = colors[(int)std::floor(it)];
                        sf::Color col2 = colors[(int)std::floor(it) + 1];

                        double percent = it - (long) it;
                        vertices[index].color = sf::Color(
                            col1.r + (col2.r - col1.r) * percent,
                            col1.g + (col2.g - col1.g) * percent,
                            col1.b + (col2.b - col1.b) * percent);
                    }
                }
            }

    target.draw(vertices);
}
