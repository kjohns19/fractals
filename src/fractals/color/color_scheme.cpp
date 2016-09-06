#include <fractals/color/color_scheme.hpp>

#include <algorithm>
#include <iostream>
#include <cmath>

namespace frac {

ColorScheme::ColorScheme(std::initializer_list<ColorValue> l)
: d_colors(l.begin(), l.end())
, d_loop_count(1) {}
    

void ColorScheme::add(std::initializer_list<ColorValue> l)
{
    d_colors.insert(l.begin(), l.end());
}

void ColorScheme::add(double value, const sf::Color& color)
{
    d_colors.insert(ColorValue(value, color));
}

void ColorScheme::set(std::initializer_list<ColorValue> l)
{
    d_colors.insert(l.begin(), l.end());
}

void ColorScheme::set(double value, const sf::Color& color)
{
    d_colors[value] = color;
}

void ColorScheme::remove(double value)
{
    d_colors.erase(value);
}

const std::map<double, sf::Color>& ColorScheme::getColors() const
{
    return d_colors;
}

int ColorScheme::getLoopCount() const
{
    return d_loop_count;
}
void ColorScheme::setLoopCount(int count)
{
    if (count < 1)
        count = 1;
    d_loop_count = count;
}

sf::Color ColorScheme::getColor(double val) const
{
    if (d_colors.empty())
        return sf::Color::Black;

    double value = val;
    if (d_loop_count != 1 && val != 1)
        value = std::fmod(value, 1.0 / d_loop_count) * d_loop_count;
    int sector = (int)(val * d_loop_count);

    std::map<double, sf::Color>::const_iterator it, it2;
    for(it = d_colors.begin(); it != d_colors.end(); ++it)
    {
        if (it->first > value)
            break;
    }

    if (it == d_colors.begin())
    {
        if (sector == 0)
            return it->second;
        else
            it2 = std::prev(d_colors.end());
    }
    else if (it == d_colors.end())
    {
        it2 = std::prev(it);
        if (sector >= d_loop_count-1)
            return it2->second;
        else
            it = d_colors.begin();
    }
    else
        it2 = std::prev(it);

    const sf::Color& col1 = it2->second;
    const sf::Color& col2 = it->second;

    double val1 = it->first;
    double val2 = it2->first;

    if (val2 > val1)
        val1+=1;
    if (val2 > value)
        value+=1;

    double percent = (value - val2) / (val1 - val2);

    sf::Color result(
        (sf::Uint8) ((int)col1.r + ((int)col2.r - (int)col1.r) * percent),
        (sf::Uint8) ((int)col1.g + ((int)col2.g - (int)col1.g) * percent),
        (sf::Uint8) ((int)col1.b + ((int)col2.b - (int)col1.b) * percent));
    return result;
}

} // close namespace frac
