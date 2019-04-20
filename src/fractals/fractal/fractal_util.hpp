#ifndef INCLUDED_FRAC_FRACTAL_UTIL_HPP
#define INCLUDED_FRAC_FRACTAL_UTIL_HPP

#include <fractals/fractal/fractal.hpp>
#include <fractals/util/parse_util.hpp>

#include <nlohmann/json.hpp>

#include <memory>

namespace frac {

struct FractalUtil : ParseUtil::Helper<FractalUtil, std::unique_ptr<Fractal>,
                                       Fractal, const sf::Vector2u&>
{
    static std::unique_ptr<Fractal> loadFromJson(
        const nlohmann::json& json, const sf::Vector2u& size);
    static nlohmann::json saveToJson(const Fractal& fractal);
};

} // close namespace frac

#endif // INCLUDED_FRAC_FRACTAL_UTIL_HPP
