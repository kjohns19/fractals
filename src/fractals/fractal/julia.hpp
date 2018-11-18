#ifndef INCLUDED_FRAC_JULIA_HPP
#define INCLUDED_FRAC_JULIA_HPP

#include <fractals/fractal/fractal.hpp>

#include <vector>

namespace sf {

class Color;

template<typename T>
class Vector2;

} // close namespace sf

namespace frac {

class View;

class Julia : public Fractal
{
public:
    Julia(const sf::Vector2u& size, long double x, long double y, long double power);

    using Fractal::clone;
    virtual std::unique_ptr<Fractal> clone(const sf::Vector2u& size, const View& view) const;

    long double getX() const { return d_x; }
    long double getY() const { return d_y; }
    long double getPower() const { return d_power; }

    void setValue(long double x, long double y, long double power);
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
    long double d_power;
};

} // close namespace frac

#endif //INCLUDED_FRAC_JULIA_HPP
