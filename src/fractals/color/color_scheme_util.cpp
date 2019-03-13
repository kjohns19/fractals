#include <fractals/color/color_scheme_util.hpp>

#include <nlohmann/json.hpp>

namespace frac {

ColorScheme ColorSchemeUtil::loadFromJson(const nlohmann::json& json)
{
    ColorScheme colorScheme;

    colorScheme.setLoopCount(json["repeat"]);
    colorScheme.setOffset(json.value("offset", 0));

    auto& colors = json["colors"];
    for(auto& colorData: colors)
    {
        double position = colorData["position"];
        auto& values = colorData["values"];
        sf::Color color(
            values[0],
            values[1],
            values[2]);
        colorScheme.add(position, color);
    }

    return colorScheme;
}

nlohmann::json ColorSchemeUtil::saveToJson(const ColorScheme& colorScheme)
{
    nlohmann::json json;
    json["repeat"] = colorScheme.getLoopCount();
    json["offset"] = colorScheme.getOffset();

    for(auto& colorData: colorScheme)
    {
        auto position = colorData.first;
        auto& color = colorData.second;
        json["colors"].push_back({
            {"position", position},
            {"values", { color.r, color.g, color.b }}
        });
    }
    return json;
}

} // close namespace frac

