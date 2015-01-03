#ifndef INCLUDED_COLOR_SCHEME_HPP
#define INCLUDED_COLOR_SCHEME_HPP

#include <SFML/Graphics/Color.hpp>

class ColorScheme
{
public:
    virtual ~ColorScheme() {}

    virtual sf::Color color(double value) const = 0;
};

#endif //INCLUDED_COLOR_SCHEME_HPP
