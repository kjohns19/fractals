#include <fractals/fractal/julia.hpp>

#include <fractals/util/parse_util.hpp>
#include <fractals/util/view.hpp>

#include <SFML/Graphics.hpp>

#include <cmath>

namespace frac {

Julia::Julia(const sf::Vector2u& size)
: Fractal(size)
, d_value(0, 0)
, d_power(2) {}

void Julia::resetPoint(long double x, long double y, Point& point)
{
    point.pos = {x, y};
}

void Julia::setValue(long double x, long double y, long double power)
{
    std::complex<long double> value(x, y);
    if (d_value != value || d_power != power)
    {
        d_value = {x, y};
        d_power = power;
        int it = iterations();
        setView(getView());
        iterate(it);
    }
}

nlohmann::json Julia::getValues() const
{
    return {
        {"power", ParseUtil::write(d_power)},
        {"x", ParseUtil::write(d_value.real())},
        {"y", ParseUtil::write(d_value.imag())}
    };
}

void Julia::setValues(const nlohmann::json& json)
{
    setValue(
        ParseUtil::read<long double>(json["x"]),
        ParseUtil::read<long double>(json["y"]),
        ParseUtil::read<long double>(json["power"]));
}

std::unique_ptr<Fractal> Julia::clone(const sf::Vector2u& size, const View& view) const
{
    auto fractal = std::make_unique<Julia>(size);
    fractal->setValue(d_value.real(), d_value.imag(), d_power);
    fractal->setView(view);
    fractal->setDrawMode(getDrawMode());
    return fractal;
}

void Julia::doIterate(
        int count,
        const std::vector<size_t>& valid,
        std::vector<Fractal::Point>& points,
        size_t startPoint,
        size_t numPoints,
        std::vector<size_t>& done)
{
    for(size_t j = 0; j < numPoints; j++)
    {
        size_t index = valid[startPoint + j];
        auto& point = points[index];
        std::complex<long double> next;

        for(int i = 0; i < count; i++)
        {
            if (std::norm(point.pos) < (1 << 16))
            {
                next = std::pow(point.pos, d_power) + d_value;
                if (next == point.pos)
                {
                    point.value+=count-i;
                    point.remove = true;
                    done.push_back(index);
                    break;
                }
                point.pos = next;
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

} // close namespace frac
