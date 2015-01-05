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
    template<typename Container>
    GradientColorScheme(const Container& container):
        d_colors(std::begin(container), std::end(container)),
        d_loop_count(1) {}

    GradientColorScheme(std::initializer_list<ColorValue> l);

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
    void set(std::initializer_list<ColorValue> l);
    void set(double value, const sf::Color& color);

    void remove(double value);

    const std::map<double, sf::Color> getColors() const { return d_colors; }

    virtual sf::Color getColor(double value) const;

    int getLoopCount() const;
    void setLoopCount(int count);

private:
    std::map<double, sf::Color> d_colors;
    int d_loop_count;
};

#endif //INCLUDED_GRADIENT_COLOR_SCHEME_HPP
