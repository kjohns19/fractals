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
    Julia(const sf::Vector2u& size, long double x, long double y);

    using Fractal::clone;
    virtual std::unique_ptr<Fractal> clone(const sf::Vector2u& size, const View& view) const;

    long double getX() const { return d_x; }
    long double getY() const { return d_y; }

    void setValue(long double x, long double y);
private:
    virtual void resetPoint(long double x, long double y, Point& point) override;
    virtual void doIterate(
            int count,
            const std::vector<size_t>& valid,
            std::vector<Fractal::Point>& points,
            size_t startPoint,
            size_t numPoints,
            std::vector<size_t>& done) override;
    long double d_x;
    long double d_y;
};

#endif //INCLUDED_JULIA_HPP
