#include <fractals/ui/fractal_widget_util.hpp>

#include <fractals/color/color_scheme_util.hpp>
#include <fractals/fractal/fractal_util.hpp>

namespace frac {

void FractalWidgetUtil::loadFromJson(
        const nlohmann::json& json, FractalWidget* widget)
{
    auto size = widget->window().getSize();
    widget->setFractal(FractalUtil::loadFromJson(json["fractal"], size));
    widget->colorScheme() = ColorSchemeUtil::loadFromJson(json["colorscheme"]);
}

nlohmann::json FractalWidgetUtil::saveToJson(const FractalWidget& widget)
{
    return {
        {"colorscheme", ColorSchemeUtil::saveToJson(widget.colorScheme())},
        {"fractal", FractalUtil::saveToJson(widget.fractal())}
    };
}

} // close namespace frac

