#ifndef INCLUDED_FRAC_COLOR_SCHEME_HPP
#define INCLUDED_FRAC_COLOR_SCHEME_HPP

#include <SFML/Graphics/Color.hpp>

#include <map>

namespace frac {

class ColorScheme
{
public:
    using ColorValue = std::pair<double, sf::Color>;

    ColorScheme();

    template<typename Container>
    ColorScheme(const Container& container)
    : d_colors(std::begin(container), std::end(container))
    , d_loop_count(1) {}

    ColorScheme(std::initializer_list<ColorValue> l);

    template<typename Container>
    void add(const Container& container)
    {
        d_colors.insert(std::begin(container), std::end(container));
    }
    void add(std::initializer_list<ColorValue> l);
    void add(double value, const sf::Color& color);

    template<typename Container>
    void set(const Container& container)
    {
        d_colors.clear();
        add(container);
    }

    const std::map<double, sf::Color>& getColors() const;

    sf::Color getColor(double value) const;

    int getLoopCount() const;
    void setLoopCount(int count);

private:
    std::map<double, sf::Color> d_colors;
    int d_loop_count;
};

} // close namespace frac

#endif //INCLUDED_FRAC_COLOR_SCHEME_HPP
