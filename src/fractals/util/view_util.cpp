#include <fractals/util/view_util.hpp>

#include <fractals/util/parse_util.hpp>

#include <nlohmann/json.hpp>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

namespace frac {

View ViewUtil::loadFromJson(const nlohmann::json& json)
{
    return View(
        ParseUtil::read<long double>(json["x"]),
        ParseUtil::read<long double>(json["y"]),
        ParseUtil::read<long double>(json["width"]),
        ParseUtil::read<long double>(json["height"]));
}

nlohmann::json ViewUtil::saveToJson(const View& view)
{
    return {
        {"x", ParseUtil::write(view.x)},
        {"y", ParseUtil::write(view.y)},
        {"width", ParseUtil::write(view.width)},
        {"height", ParseUtil::write(view.height)}
    };
}

} // close namespace frac
