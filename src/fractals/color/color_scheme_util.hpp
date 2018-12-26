#ifndef INCLUDED_FRAC_COLOR_SCHEME_UTIL_HPP
#define INCLUDED_FRAC_COLOR_SCHEME_UTIL_HPP

#include <fractals/color/color_scheme.hpp>

#include <memory>
#include <string>

namespace frac {

struct ColorSchemeUtil
{
    ColorSchemeUtil() = delete;

    static ColorScheme loadFromFile(
            const std::string& filename);
    static ColorScheme loadFromJson(
            const std::string& jsonStr);

    static void saveToFile(
            const std::string& filename, const ColorScheme& colorScheme);
    static std::string saveToJson(const ColorScheme& colorScheme);
};

} // close namespace frac

#endif // INCLUDED_FRAC_COLOR_SCHEME_UTIL_HPP
