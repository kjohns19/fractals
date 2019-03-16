#ifndef INCLUDED_FRAC_MANDELBROT_HPP
#define INCLUDED_FRAC_MANDELBROT_HPP

#include <fractals/fractal/fractal.hpp>
#include <fractals/util/view.hpp>

#include <memory>
#include <vector>

namespace sf {

class Color;

template<typename T>
class Vector2;

} // close namespace sf

namespace frac {

class View;

class Mandelbrot : public Fractal
{
public:
    Mandelbrot(const sf::Vector2u& size);

    using Fractal::clone;
    virtual std::unique_ptr<Fractal> clone(const sf::Vector2u& size, const View& view) const;

    virtual long double getPower() const override { return 2; }

    virtual std::string type() const override { return "Mandelbrot"; }
private:
    virtual void resetPoint(long double x, long double y, Point& point) override;
    virtual void doIterate(
            int count,
            const std::vector<size_t>& valid,
            std::vector<Fractal::Point>& points,
            size_t startPoint,
            size_t numPoints,
            std::vector<size_t>& done) override;

    double d_power;
};

} // close namespace frac

#endif //INCLUDED_FRAC_MANDELBROT_HPP
