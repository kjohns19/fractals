#include <fractals/color/color_scheme_util.hpp>

#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>
#include <string>

namespace frac {

ColorScheme ColorSchemeUtil::loadFromFile(const std::string& filename)
{
    // TODO error handling
    std::ifstream in(filename);
    std::string jsonStr(
        std::istreambuf_iterator<char>{in},
        std::istreambuf_iterator<char>{});
    return loadFromJson(jsonStr);
}

ColorScheme ColorSchemeUtil::loadFromJson(const std::string& jsonStr)
{
    ColorScheme colorScheme;

    auto j = nlohmann::json::parse(jsonStr);

    colorScheme.setLoopCount(j["repeat"]);

    auto& colors = j["colors"];
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

void ColorSchemeUtil::saveToFile(
        const std::string& filename, const ColorScheme& colorScheme)
{
    // TODO error handling
    std::ofstream out(filename);
    std::string j = saveToJson(colorScheme);
    out << j;
}

std::string ColorSchemeUtil::saveToJson(const ColorScheme& colorScheme)
{
    nlohmann::json j;
    j["repeat"] = colorScheme.getLoopCount();

    for(auto& colorData: colorScheme)
    {
        auto position = colorData.first;
        auto& color = colorData.second;
        j["colors"].push_back({
            {"position", position},
            {"values", { color.r, color.g, color.b }}
        });
    }
    std::ostringstream ss;
    ss << std::setw(4) << j;
    return ss.str();
}

} // close namespace frac

