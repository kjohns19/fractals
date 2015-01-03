#include <fractals/color/GradientColorScheme.hpp>

#include <algorithm>
#include <iostream>

GradientColorScheme::GradientColorScheme(std::initializer_list<ColorValue> l):
    d_colors(l.begin(), l.end()) {}
    

void GradientColorScheme::add(std::initializer_list<ColorValue> l)
{
    d_colors.insert(l.begin(), l.end());
}

sf::Color GradientColorScheme::color(double value) const
{
    auto it = std::lower_bound(d_colors.begin(), d_colors.end(), value,
        [](const ColorValue& col, double value) {
            return col.first < value;
        });


    if (it == d_colors.end())
        return sf::Color::Black;

    if (it == d_colors.begin())
        return it->second;
        
    auto prev = it;
    --prev;

    const sf::Color& col1 = prev->second;
    const sf::Color& col2 = it->second;

    double percent = (value - prev->first) / (it->first - prev->first);

    sf::Color result((sf::Uint8) ((int)col1.r + ((int)col2.r - (int)col1.r) * percent),
                     (sf::Uint8) ((int)col1.g + ((int)col2.g - (int)col1.g) * percent),
                     (sf::Uint8) ((int)col1.b + ((int)col2.b - (int)col1.b) * percent));
    return result;
}
