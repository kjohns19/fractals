#include <fractals/fractal/fractal.hpp>
#include <fractals/color/color_scheme.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics.hpp>

#include <numeric>
#include <future>
#include <iostream>
#include <cmath>

namespace frac {

Fractal::Fractal(const sf::Vector2u& size)
: d_points(size.x * size.y)
, d_valid()
, d_done()
, d_hist(1, 0)
, d_size(size)
, d_view()
, d_iterations(0)
, d_fastDraw(true) {}

std::unique_ptr<Fractal> Fractal::clone() const
{
    return clone(getSize(), getView());
}
std::unique_ptr<Fractal> Fractal::clone(const sf::Vector2u& size) const
{
    return clone(size, getView());
}
std::unique_ptr<Fractal> Fractal::clone(const View& view) const
{
    return clone(getSize(), view);
}

void Fractal::setView(const View& view)
{
    d_view = view;
    reset();
}

void Fractal::reset()
{
    d_valid.resize(d_size.x * d_size.y);
    d_done.clear();
    d_hist.resize(1, 0);

    long double left = d_view.x - d_view.width/2;
    long double top  = d_view.y - d_view.height/2;

    long double realX, realY;
    for(unsigned y = 0; y < d_size.y; y++)
        for(unsigned x = 0; x < d_size.x; x++)
        {
            size_t index = x + d_size.x * y;
            d_valid[index] = index;

            realX = d_view.width * x / d_size.x + left;
            realY = d_view.height * y / d_size.y + top;

            Point& point = d_points[index];
            resetPoint(realX, realY, point);
            point.value = 0;
            point.remove = false;
        }
    d_iterations = 0;
}

void Fractal::iterate(int count)
{
    const size_t maxSize = 50000;
    size_t jobCount = d_valid.size() / maxSize;
    if (d_valid.size() % maxSize)
        jobCount++;
    d_hist.resize(d_hist.size() + count, 0);

    for(size_t pos: d_done)
        d_points[pos].value+=count;

    std::vector<size_t> indices(jobCount);
    std::iota(indices.begin(), indices.end(), 0);
    std::vector<std::vector<size_t>> allDone(jobCount);

    d_threadPool.map(indices.begin(), indices.end(), [=, &allDone](int i) {
        size_t jobSize = std::min(maxSize, d_valid.size()-i*maxSize);
        doIterate(count, d_valid, d_points, i*maxSize, jobSize, allDone[i]);
    });

    for(auto& done: allDone)
        d_done.insert(d_done.end(), done.begin(), done.end());

    for(size_t i = 0; i < d_valid.size();)
    {
        Point& p = d_points[d_valid[i]];
        if (p.remove)
        {
            d_hist[p.value]++;
            d_valid[i] = d_valid.back();
            d_valid.pop_back();
        }
        else
            i++;
    }

    d_iterations+=count;
}

void Fractal::getColors(const ColorScheme& cs, std::vector<sf::Color>& colors)
{
    colors.resize(d_iterations+1);
    bool histogram = false;
    if (histogram)
    {
        size_t total = d_points.size();
        double hue = 0;
        for(int i = 0; i <= d_iterations; i++)
        {
            hue = (i == d_iterations) ? 1 : (hue + 1.0 * d_hist[i] / total);
            colors[i] = cs.getColor(hue);
        }
    }
    else
    {
        for(int i = 0; i <= d_iterations; i++)
            colors[i] = cs.getColor(1.0 * i / d_iterations);
    }

}

void Fractal::draw(sf::RenderTarget& target, const ColorScheme& cs)
{
    std::vector<sf::Color> colors;
    getColors(cs, colors);

    bool fastDraw = getDrawMode();

    std::vector<sf::Color> transitionColors;
    int transitionCount = 10;
    if (!fastDraw)
    {
        transitionColors.resize(colors.size() * transitionCount);
        for(size_t i = 0; i < colors.size(); i++)
        {
            sf::Color& col1 = colors[i];
            sf::Color& col2 = colors[(i+1)%colors.size()];
            for(int j = 0; j < transitionCount; j++)
            {
                double percent = 1.0 * j / transitionCount;
                transitionColors[i*transitionCount + j] = sf::Color(
                    col1.r + (col2.r - col1.r) * percent,
                    col1.g + (col2.g - col1.g) * percent,
                    col1.b + (col2.b - col1.b) * percent);
            }
        }
    }

    sf::VertexArray vertices(sf::Points, d_size.x * (d_size.y + 1));

    if (d_iterations > 0)
    {
        for(unsigned y = 0; y < d_size.y; y++)
            for(unsigned x = 0; x < d_size.x; x++)
            {
                size_t index = x + d_size.x * y;
                vertices[index].position = sf::Vector2f(x+0.5, y+0.5);

                if (fastDraw)
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
                        double nu = std::log2(std::log2(zn)) - 1;
                        it += 1 - nu;

                        if (it < 0)
                            vertices[index].color = colors[0];
                        else if (it+1 > colors.size()-1)
                            vertices[index].color = colors.back();
                        else
                        {
                            double percent = it - (long) it;
                            int transitionIndex = (int)(percent * transitionCount);
                            int base = (int)std::floor(it);
                            vertices[index].color = transitionColors[base*transitionCount+transitionIndex];
                        }
                    }
                }
            }
    }
    target.draw(vertices);
}

} // close namespace frac
