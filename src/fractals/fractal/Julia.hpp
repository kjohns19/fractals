#ifndef INCLUDED_JULIA_HPP
#define INCLUDED_JULIA_HPP

#include <fractals/fractal/Fractal.hpp>

#include <SFML/System/Vector2.hpp>

#include <complex>
#include <vector>
#include <functional>

namespace sf
{
    class Color;
}

class View;

class Julia : public Fractal
{
public:
    using Func = std::function<bool(double zx, double zy, double& nx, double& ny)>;
    Julia(const sf::Vector2u& size, const View& view, const Func& func);

    virtual void setView(const View& view);

    virtual void iterate(int count = 1);

    virtual void draw(sf::RenderTarget& target, const ColorScheme& cs);

    virtual int iterations() const { return d_iterations; }
private:
    struct Point
    {
        double x, y;
        int value;
    };

    void getColors(const ColorScheme& cs, std::vector<sf::Color>& colors);

    std::vector<Point> d_points;
    std::vector<size_t> d_valid;
    sf::Vector2u d_size;

    Func d_func;
    int d_iterations;
};

#endif //INCLUDED_JULIA_HPP
