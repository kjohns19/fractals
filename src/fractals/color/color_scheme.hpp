#ifndef INCLUDED_FRAC_COLOR_SCHEME_HPP
#define INCLUDED_FRAC_COLOR_SCHEME_HPP

#include <SFML/Graphics/Color.hpp>

#include <map>

namespace frac {

class ColorScheme
{
private:
    using ColorMap = std::map<double, sf::Color>;
public:
    using ColorValue = ColorMap::value_type;
    using iterator = ColorMap::const_iterator;

    ColorScheme();

    template<typename Container>
    ColorScheme(const Container& container)
    : d_colors(std::begin(container), std::end(container))
    , d_loop_count(1) {}

    template<typename Container>
    void add(const Container& container)
    {
        d_colors.insert(std::begin(container), std::end(container));
    }
    void add(double value, const sf::Color& color);

    template<typename Container>
    void set(const Container& container)
    {
        d_colors.clear();
        add(container);
    }

    iterator begin() const { return d_colors.begin(); }
    iterator end() const { return d_colors.end(); }
    size_t size() const { return d_colors.size(); }

    sf::Color getColor(double value) const;

    int getLoopCount() const;
    void setLoopCount(int count);

private:
    ColorMap d_colors;
    int d_loop_count;
};

} // close namespace frac

#endif //INCLUDED_FRAC_COLOR_SCHEME_HPP
