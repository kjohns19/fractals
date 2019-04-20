#include <fractals/fractal/fractal_util.hpp>

#include <fractals/fractal/julia.hpp>
#include <fractals/fractal/mandelbrot.hpp>
#include <fractals/util/view_util.hpp>

#include <stdexcept>

namespace frac {

std::unique_ptr<Fractal> FractalUtil::loadFromJson(
        const nlohmann::json& json, const sf::Vector2u& size)
{
    std::unique_ptr<Fractal> fractal;

    std::string type = json["type"];
    if (type == "Mandelbrot")
        fractal = std::make_unique<Mandelbrot>(size);
    else if (type == "Julia")
        fractal = std::make_unique<Julia>(size);
    else
        throw std::runtime_error("Unknown fractal type " + type);

    fractal->setValues(json["values"]);
    fractal->setView(ViewUtil::loadFromJson(json["view"]));
    fractal->setDrawMode(json["fastdraw"]);
    fractal->iterate(json["iterations"]);

    return fractal;
}

nlohmann::json FractalUtil::saveToJson(const Fractal& fractal)
{
    return {
        {"fastdraw", fractal.getDrawMode()},
        {"iterations", fractal.iterations()},
        {"type", fractal.type()},
        {"values", fractal.getValues()},
        {"view", ViewUtil::saveToJson(fractal.getView())}
    };
}

} // close namespace frac

