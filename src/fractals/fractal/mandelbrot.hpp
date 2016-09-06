#ifndef INCLUDED_FRAC_MANDELBROT_HPP
#define INCLUDED_FRAC_MANDELBROT_HPP

#include <fractals/fractal/fractal.hpp>
#include <fractals/util/view.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <memory>

namespace frac {

class View;

class Mandelbrot : public Fractal
{
public:
    Mandelbrot(const sf::Vector2u& size);

    using Fractal::clone;
    virtual std::unique_ptr<Fractal> clone(const sf::Vector2u& size, const View& view) const;
private:
    virtual void resetPoint(long double x, long double y, Point& point) override;
    virtual void doIterate(
            int count,
            const std::vector<size_t>& valid,
            std::vector<Fractal::Point>& points,
            size_t startPoint,
            size_t numPoints,
            std::vector<size_t>& done) override;
};

} // close namespace frac

#endif //INCLUDED_FRAC_MANDELBROT_HPP
