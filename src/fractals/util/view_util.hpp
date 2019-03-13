#ifndef INCLUDED_FRAK_VIEW_UTIL_HPP
#define INCLUDED_FRAK_VIEW_UTIL_HPP

#include <fractals/util/parse_util.hpp>
#include <fractals/util/view.hpp>

#include <nlohmann/json.hpp>

#include <string>

namespace frac {

struct ViewUtil : ParseUtil::Helper<ViewUtil, View>
{
    static View loadFromJson(const nlohmann::json& json);
    static nlohmann::json saveToJson(const View& view);
};

} // close namespace frac

#endif // INCLUDED_FRAK_VIEW_UTIL_HPP
