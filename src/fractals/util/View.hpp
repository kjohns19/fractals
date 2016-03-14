#ifndef INCLUDED_VIEW_HPP
#define INCLUDED_VIEW_HPP

#include <SFML/Graphics/Rect.hpp>
#include <vector>

namespace sf
{
    template<typename T>
    class Vector2;
}

struct View
{
    View();

    template<typename U>
    View(const sf::Rect<U>& rect)
    : View(rect.left, rect.top, rect.width, rect.height) {}

    View(long double left, long double top, long double width, long double height);
    View(const sf::Vector2<long double>& pos, const sf::Vector2<long double>& size);

    void fit(const sf::Vector2u& size);
    void fit(long double ratio);

    std::vector<View> path(const View& view, int count) const;

    long double left;
    long double top;
    long double width;
    long double height;
};

#endif //INCLUDED_VIEW_HPP
