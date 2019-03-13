#include <fractals/util/parse_util.hpp>

#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <sstream>

namespace frac {
namespace {

uint64_t ntoh64(uint64_t value);

} // close anonymous namespace

double ParseUtil::readDouble(const nlohmann::json& json)
{
    std::string hexStr = json["hex"];
    std::istringstream ss(hexStr);
    uint64_t hexValue;
    ss >> std::hex >> hexValue;
    hexValue = ntoh64(hexValue);
    double value;
    std::memcpy(&value, &hexValue, sizeof(value));
    return value;
}

nlohmann::json ParseUtil::writeDouble(double value)
{
    uint64_t hexValue;
    std::memcpy(&hexValue, &value, sizeof(hexValue));
    hexValue = ntoh64(hexValue);
    std::ostringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hexValue;

    nlohmann::json j;
    j["value"] = value;
    j["hex"] = ss.str();

    return j;
}

namespace {

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

} // close anonymous namespace
} // close namespace frac
