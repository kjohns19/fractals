#ifndef INCLUDED_JULIA_HPP
#define INCLUDED_JULIA_HPP

#include <fractals/fractal/Fractal.hpp>

#include <SFML/System/Vector2.hpp>

#include <complex>
#include <vector>
#include <functional>

class Julia : public Fractal
{
public:
    using Func = std::function<bool(const std::complex<double>&, std::complex<double>&)>;
    Julia(const sf::Vector2u& size, const sf::Rect<double>& view, const Func& func);

    virtual void setView(const sf::Rect<double>& view);

    virtual void iterate(int count = 1);

    virtual void draw(sf::RenderTarget& target, const ColorScheme& cs);

    virtual int iterations() const { return d_iterations; }
private:
    struct Point
    {
        std::complex<double> z;
        int value;
    };

    std::vector<Point> d_points;
    std::vector<size_t> d_valid;
    sf::Vector2u d_size;

    Func d_func;
    int d_iterations;
};

#endif //INCLUDED_JULIA_HPP
