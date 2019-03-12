#include <fractals/util/view_util.hpp>

#include <nlohmann/json.hpp>

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

namespace frac {
namespace {

uint64_t ntoh64(uint64_t value);

double parseValue(const nlohmann::json &json);
nlohmann::json writeValue(double value);

}

View ViewUtil::loadFromFile(const std::string& filename)
{
    // TODO error handling
    std::ifstream in(filename);
    std::string jsonStr(
        std::istreambuf_iterator<char>{in},
        std::istreambuf_iterator<char>{});
    return loadFromJson(jsonStr);
}

View ViewUtil::loadFromJson(const std::string& jsonStr)
{
    auto j = nlohmann::json::parse(jsonStr);

    double x = parseValue(j["x"]);
    double y = parseValue(j["y"]);
    double width = parseValue(j["width"]);
    double height = parseValue(j["height"]);

    return View(x, y, width, height);
}

void ViewUtil::saveToFile(const std::string& filename, const View& view)
{
    // TODO error handling
    std::ofstream out(filename);
    std::string j = saveToJson(view);
    out << j;
}

std::string ViewUtil::saveToJson(const View& view)
{
    nlohmann::json j;

    j["x"] = writeValue(view.x);
    j["y"] = writeValue(view.y);
    j["width"] = writeValue(view.width);
    j["height"] = writeValue(view.height);

    std::ostringstream ss;
    ss << std::setw(4) << j;
    return ss.str();
}

namespace {

static_assert(sizeof(uint64_t) == sizeof(double));

uint64_t ntoh64(uint64_t value)
{
    return (
        (((value >>  0) & 0xFF) << 56) |
        (((value >>  8) & 0xFF) << 48) |
        (((value >> 16) & 0xFF) << 40) |
        (((value >> 24) & 0xFF) << 32) |
        (((value >> 32) & 0xFF) << 24) |
        (((value >> 40) & 0xFF) << 16) |
        (((value >> 48) & 0xFF) <<  8) |
        (((value >> 56) & 0xFF) <<  0)
    );
}

double parseValue(const nlohmann::json &j)
{
    std::string hexStr = j["hex"];
    std::istringstream ss(hexStr);
    unsigned long long hexValue;
    ss >> std::hex >> hexValue;
    hexValue = ntoh64(hexValue);
    double value;
    std::memcpy(&value, &hexValue, sizeof(value));
    return value;
}

nlohmann::json writeValue(double value)
{
    unsigned long long hexValue;
    std::memcpy(&hexValue, &value, sizeof(hexValue));
    hexValue = ntoh64(hexValue);
    std::ostringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hexValue;

    nlohmann::json j;
    j["value"] = value;
    j["hex"] = ss.str();

    return j;
}

}
} // close namespace frac
