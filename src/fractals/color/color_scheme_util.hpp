#ifndef INCLUDED_FRAC_COLOR_SCHEME_UTIL_HPP
#define INCLUDED_FRAC_COLOR_SCHEME_UTIL_HPP

#include <fractals/color/color_scheme.hpp>
#include <fractals/util/parse_util.hpp>

#include <nlohmann/json.hpp>

namespace frac {

struct ColorSchemeUtil : ParseUtil::Helper<ColorSchemeUtil, ColorScheme>
{
    static ColorScheme loadFromJson(const nlohmann::json& json);
    static nlohmann::json saveToJson(const ColorScheme& colorScheme);
};

} // close namespace frac

#endif // INCLUDED_FRAC_COLOR_SCHEME_UTIL_HPP
