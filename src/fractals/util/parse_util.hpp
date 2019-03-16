#ifndef INCLUDED_FRAC_PARSE_UTIL_HPP
#define INCLUDED_FRAC_PARSE_UTIL_HPP

#include <nlohmann/json.hpp>

#include <array>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <type_traits>

namespace frac {

struct ParseUtil
{
    ParseUtil() = delete;

    template<typename T,
             typename=std::enable_if_t<std::is_floating_point<T>::value>>
    static T read(const nlohmann::json& json);

    template<typename T,
             typename=std::enable_if_t<std::is_floating_point<T>::value>>
    static nlohmann::json write(T value);

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

template<typename T, typename>
T ParseUtil::read(const nlohmann::json& json)
{
    // TODO endianness
    std::string hexStr = json["hex"];
    std::array<unsigned char, sizeof(T)> bytes;
    int offset = 0;
    for (auto& byte: bytes)
    {
        unsigned val;
        std::sscanf(hexStr.c_str()+offset, "%2x", &val);
        byte = val;
        offset += 2;
    }
    T value;
    std::memcpy(&value, bytes.data(), sizeof(T));
    return value;
}

template<typename T, typename>
nlohmann::json ParseUtil::write(T value)
{
    // TODO endianness
    std::ostringstream ss;
    std::array<unsigned char, sizeof(T)> bytes;
    std::memcpy(bytes.data(), &value, sizeof(T));
    for (auto byte: bytes)
        ss << std::hex << std::setw(2)
           << std::setfill('0') << static_cast<int>(byte);
    return {
        {"value", value},
        {"hex", ss.str()}
    };
}

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
