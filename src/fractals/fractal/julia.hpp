#ifndef INCLUDED_FRAC_JULIA_HPP
#define INCLUDED_FRAC_JULIA_HPP

#include <fractals/fractal/fractal.hpp>

#include <complex>
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
    Julia(const sf::Vector2u& size);

    using Fractal::clone;
    virtual std::unique_ptr<Fractal> clone(const sf::Vector2u& size, const View& view) const;

    long double getX() const { return d_value.real(); }
    long double getY() const { return d_value.imag(); }
    virtual long double getPower() const override { return d_power; }

    void setValue(long double x, long double y, long double power);

    virtual nlohmann::json getValues() const override;
    virtual void setValues(const nlohmann::json& json) override;

    virtual std::string type() const override { return "Julia"; }
private:
    virtual void resetPoint(long double x, long double y, Point& point) override;
    virtual void doIterate(
            int count,
            const std::vector<size_t>& valid,
            std::vector<Fractal::Point>& points,
            size_t startPoint,
            size_t numPoints,
            std::vector<size_t>& done) override;

    std::complex<long double> d_value;
    long double d_power;
};

} // close namespace frac

#endif //INCLUDED_FRAC_JULIA_HPP
