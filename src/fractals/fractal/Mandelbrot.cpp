#include <fractals/fractal/Mandelbrot.hpp>
#include <fractals/color/ColorScheme.hpp>
#include <iostream>
#include <thread>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics.hpp>

Mandelbrot::Mandelbrot(const sf::Vector2u& size, const View& view):
    d_points(size.x * size.y),
    d_valid(size.x * size.y),
    d_done(),
    d_hist(1, 0),
    d_size(size),
    d_view(view),
    d_iterations(0)
{
    setView(view);
}

void Mandelbrot::setView(const View& view)
{
    d_valid.resize(d_size.x * d_size.y);
    d_done.clear();
    d_hist.resize(1, 0);
    d_view = view;

    for(unsigned y = 0; y < d_size.y; y++)
        for(unsigned x = 0; x < d_size.x; x++)
        {
            size_t index = x + d_size.x * y;
            d_valid[index] = index;

            Point& point = d_points[index];
            point.x = 0;
            point.y = 0;
            point.value = 0;
            point.remove = false;
        }
    d_iterations = 0;
}

void Mandelbrot::iterate(int count)
{
    size_t maxSize = 50000;
    size_t threadCount = std::min<size_t>(10, d_valid.size() / maxSize);
    size_t size = d_valid.size() / (threadCount + 1);
    std::vector<std::thread> threads;
    std::vector<std::vector<size_t> > thread_dones;

    auto func = [&] (int i) {
        iterate(count, i*size, size, thread_dones[i]);
    };

    thread_dones.resize(threadCount);
    d_hist.resize(d_hist.size() + count, 0);

    for(size_t i = 0; i < threadCount; i++)
        threads.push_back(std::thread(func, i));

    for(size_t pos: d_done)
        d_points[pos].value+=count;

    iterate(count, threadCount * size, d_valid.size() - threadCount * size, d_done);

    size_t i = 0;
    for(auto& thread: threads)
    {
        thread.join();
        std::vector<size_t>& done = thread_dones[i++];
        d_done.insert(d_done.end(), done.begin(), done.end());
    }

    for(i = 0; i < d_valid.size();)
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

    //if (d_iterations % 100 == 0)
    //    std::cout << d_iterations << std::endl;
}

void Mandelbrot::iterate(int count, size_t left, size_t size, std::vector<size_t>& done)
{
    for(size_t j = 0; j < size; j++)
    {
        size_t index = d_valid[left + j];
        Point& point = d_points[index];

        long double cx = d_view.width;
        long double cy = d_view.height;

        cx = cx * (index % d_size.x) / d_size.x + d_view.left;
        cy = cy * (index / d_size.x) / d_size.y + d_view.top;
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

void Mandelbrot::getColors(const ColorScheme& cs, std::vector<sf::Color>& colors)
{
    bool histogram = false;
    if (histogram)
    {
        size_t total = d_points.size();
        double hue = 0;
        for(int i = 0; i <= d_iterations; i++)
        {
            hue = (i == d_iterations) ? 1 : (hue + 1.0 * d_hist[i] / total);
            colors.push_back(cs.getColor(hue));
        }
    }
    else
    {
        for(int i = 0; i <= d_iterations; i++)
            colors.push_back(cs.getColor(1.0 * i / d_iterations));
    }

}

void Mandelbrot::draw(sf::RenderTarget& target, const ColorScheme& cs)
{
    std::vector<sf::Color> colors;
    getColors(cs, colors);

    sf::VertexArray vertices(sf::Points, d_size.x * (d_size.y + 1));

    if (d_iterations > 0)
    {
        for(unsigned y = 0; y < d_size.y; y++)
            for(unsigned x = 0; x < d_size.x; x++)
            {
                size_t index = x + d_size.x * y;
                vertices[index].position = sf::Vector2f(x+0.5, y+0.5);

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

                        if (it < 0)
                            vertices[index].color = colors[0];
                        else if (it+1 > colors.size()-1)
                            vertices[index].color = colors.back();
                        else
                        {
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
            }
    }

    target.draw(vertices);
}
