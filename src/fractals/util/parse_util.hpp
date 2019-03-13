#ifndef INCLUDED_FRAC_PARSE_UTIL_HPP
#define INCLUDED_FRAC_PARSE_UTIL_HPP

#include <nlohmann/json.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace frac {

struct ParseUtil
{
    ParseUtil() = delete;

    static double readDouble(const nlohmann::json& json);
    static nlohmann::json writeDouble(double value);

    template<typename ClassT, typename LoadT, typename SaveT=LoadT,
             typename ...LoadArgs>
    struct Helper
    {
        Helper() = delete;

        static LoadT loadFromFile(
            const std::string& filename, LoadArgs&&... args);
        static LoadT loadFromString(
            const std::string& str, LoadArgs&&... args);

        static void saveToFile(
            const std::string& filename, const SaveT& value);
        static std::string saveToString(const SaveT& value);
    };
};

template<typename ClassT, typename LoadT, typename SaveT, typename ...LoadArgs>
LoadT ParseUtil::Helper<ClassT, LoadT, SaveT, LoadArgs...>::loadFromFile(
        const std::string& filename, LoadArgs&&... args)
{
    std::ifstream in(filename);
    std::string jsonStr(
        std::istreambuf_iterator<char>{in},
        std::istreambuf_iterator<char>{});
    return loadFromString(jsonStr, std::forward<LoadArgs>(args)...);
}
template<typename ClassT, typename LoadT, typename SaveT, typename ...LoadArgs>
LoadT ParseUtil::Helper<ClassT, LoadT, SaveT, LoadArgs...>::loadFromString(
        const std::string& str, LoadArgs&&... args)
{
    return ClassT::loadFromJson(
        nlohmann::json::parse(str), std::forward<LoadArgs>(args)...);
}

template<typename ClassT, typename LoadT, typename SaveT, typename ...LoadArgs>
void ParseUtil::Helper<ClassT, LoadT, SaveT, LoadArgs...>::saveToFile(
        const std::string& filename, const SaveT& value)
{
    std::ofstream out(filename);
    out << saveToString(value);
}

template<typename ClassT, typename LoadT, typename SaveT, typename ...LoadArgs>
std::string ParseUtil::Helper<ClassT, LoadT, SaveT, LoadArgs...>::saveToString(
        const SaveT& value)
{
    std::ostringstream ss;
    ss << std::setw(4) << ClassT::saveToJson(value);
    return ss.str();
}

} // close namespace frac

#endif // INCLUDED_FRAC_PARSE_UTIL_HPP
