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
    double x = ParseUtil::readDouble(json["x"]);
    double y = ParseUtil::readDouble(json["y"]);
    double width = ParseUtil::readDouble(json["width"]);
    double height = ParseUtil::readDouble(json["height"]);
    return View(x, y, width, height);
}

nlohmann::json ViewUtil::saveToJson(const View& view)
{
    nlohmann::json json;

    json["x"] = ParseUtil::writeDouble(view.x);
    json["y"] = ParseUtil::writeDouble(view.y);
    json["width"] = ParseUtil::writeDouble(view.width);
    json["height"] = ParseUtil::writeDouble(view.height);

    return json;
}

} // close namespace frac
