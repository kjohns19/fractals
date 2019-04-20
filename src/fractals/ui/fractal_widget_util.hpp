#ifndef INCLUDED_FRAC_FRACTAL_WIDGET_UTIL_HPP
#define INCLUDED_FRAC_FRACTAL_WIDGET_UTIL_HPP

#include <fractals/ui/fractal_widget.hpp>
#include <fractals/util/parse_util.hpp>

#include <nlohmann/json.hpp>

namespace frac {

struct FractalWidgetUtil : ParseUtil::Helper<FractalWidgetUtil, void,
                                             FractalWidget, FractalWidget*>
{
    static void loadFromJson(
        const nlohmann::json& json, FractalWidget* widget);
    static nlohmann::json saveToJson(const FractalWidget& widget);
};

} // close namespace frac

#endif // INCLUDED_FRAC_FRACTAL_WIDGET_UTIL_HPP
