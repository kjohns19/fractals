#ifndef INCLUDED_MANDELBROT_HPP
#define INCLUDED_MANDELBROT_HPP

#include <fractals/fractal/Fractal.hpp>
#include <fractals/util/View.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>

class View;

class Mandelbrot : public Fractal
{
public:
    Mandelbrot(const sf::Vector2u& size, const View& view);

    virtual void setView(const View& view);

    virtual void iterate(int count = 1);

    virtual void draw(sf::RenderTarget& target, const ColorScheme& cs);

    virtual int iterations() const { return d_iterations; }
private:
    struct Point
    {
        long double x, y;
        unsigned short value;
        bool remove;
    };

    void iterate(int count, size_t left, size_t size, std::vector<size_t>& done);

    void getColors(const ColorScheme& cs, std::vector<sf::Color>& colors);

    std::vector<Point> d_points;
    std::vector<size_t> d_valid;
    std::vector<size_t> d_done;
    std::vector<int> d_hist;
    sf::Vector2u d_size;
    View d_view;

    int d_iterations;
};

#endif //INCLUDED_MANDELBROT_HPP
