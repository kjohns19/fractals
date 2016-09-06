#ifndef INCLUDED_FRAC_VIEW_HPP
#define INCLUDED_FRAC_VIEW_HPP

#include <SFML/Graphics/Rect.hpp>
#include <ostream>
#include <vector>

namespace sf {

template<typename T>
class Vector2;

} // close namespace sf

namespace frac {

struct View
{
    View();

    template<typename U>
    View(const sf::Rect<U>& rect)
    : View(rect.left+rect.width/2.0, rect.top+rect.height/2.0,
           rect.width, rect.height) {}

    View(long double x, long double y, long double width, long double height);
    View(const sf::Vector2<long double>& pos, const sf::Vector2<long double>& size);

    void fit(const sf::Vector2u& size);
    void fit(long double ratio);

    std::vector<View> path(const View& view, int count) const;

    long double x;
    long double y;
    long double width;
    long double height;
};

std::ostream& operator<<(std::ostream& os, const View& view);

} // close namespace frac

#endif //INCLUDED_FRAC_VIEW_HPP
