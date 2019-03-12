#ifndef INCLUDED_FRAK_VIEW_UTIL_HPP
#define INCLUDED_FRAK_VIEW_UTIL_HPP

#include <fractals/util/view.hpp>

#include <string>

namespace frac {

struct ViewUtil
{
    ViewUtil() = delete;

    static View loadFromFile(const std::string& filename);
    static View loadFromJson(const std::string& jsonStr);

    static void saveToFile(const std::string& filename, const View& view);
    static std::string saveToJson(const View& view);
};

} // close namespace frac

#endif // INCLUDED_FRAK_VIEW_UTIL_HPP
