#ifndef INCLUDED_GRADIENT_COLOR_SCHEME_HPP
#define INCLUDED_GRADIENT_COLOR_SCHEME_HPP

#include <fractals/color/ColorScheme.hpp>

#include <initializer_list>
#include <map>

class GradientColorScheme : public ColorScheme
{
private:
    typedef std::pair<double, sf::Color> ColorValue;

public:
    GradientColorScheme(std::initializer_list<ColorValue> l);

    void add(std::initializer_list<ColorValue> l);

    virtual sf::Color color(double value) const;

private:
    std::map<double, sf::Color> d_colors;
};

#endif //INCLUDED_GRADIENT_COLOR_SCHEME_HPP
