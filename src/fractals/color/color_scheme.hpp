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
    using iterator = ColorMap::const_iterator;

    ColorScheme();

    void add(double value, const sf::Color& color);

    iterator begin() const { return d_colors.begin(); }
    iterator end() const { return d_colors.end(); }
    size_t size() const { return d_colors.size(); }

    sf::Color getColor(double value) const;

    int getLoopCount() const;
    void setLoopCount(int count);

    int getOffset() const;
    void setOffset(int offset);

private:
    ColorMap d_colors;
    int d_loop_count;
    int d_offset;
};

} // close namespace frac

#endif //INCLUDED_FRAC_COLOR_SCHEME_HPP
