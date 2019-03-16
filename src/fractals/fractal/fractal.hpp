#ifndef INCLUDED_FRAC_FRACTAL_HPP
#define INCLUDED_FRAC_FRACTAL_HPP

#include <fractals/util/view.hpp>
#include <fractals/util/thread_pool.hpp>

#include <SFML/System/Vector2.hpp>

#include <nlohmann/json.hpp>

#include <complex>
#include <memory>
#include <vector>

namespace sf {

class Color;

template<typename T>
class Rect;

class RenderTarget;

} // close namespace sf

namespace frac {

class ColorScheme;

class Fractal
{
public:
    Fractal(const sf::Vector2u& size);

    std::unique_ptr<Fractal> clone() const;
    std::unique_ptr<Fractal> clone(const sf::Vector2u& size) const;
    std::unique_ptr<Fractal> clone(const View& view) const;
    virtual std::unique_ptr<Fractal> clone(const sf::Vector2u& size, const View& view) const = 0;

    void setView(const View& view);
    const View& getView() const { return d_view; }
    const sf::Vector2u& getSize() const { return d_size; }

    virtual nlohmann::json getValues() const;
    virtual void setValues(const nlohmann::json& json);

    void reset();

    virtual long double getPower() const = 0;

    virtual std::string type() const = 0;

    void iterate(int count = 1);

    void setDrawMode(bool fast) { d_fastDraw = fast; }
    bool getDrawMode() const { return d_fastDraw; }

    void draw(sf::RenderTarget& target, const ColorScheme& cs);

    int iterations() const { return d_iterations; }
protected:
    struct Point
    {
        std::complex<long double> pos;
        unsigned short value;
        bool remove;
    };

private:
    virtual void resetPoint(long double x, long double y, Point& point) = 0;
    virtual void doIterate(
            int count,
            const std::vector<size_t>& valid,
            std::vector<Point>& points,
            size_t startPoint,
            size_t numPoints,
            std::vector<size_t>& done) = 0;

    void getColors(const ColorScheme& cs, std::vector<sf::Color>& colors);

    std::vector<Point> d_points;
    std::vector<size_t> d_valid;
    std::vector<size_t> d_done;
    std::vector<int> d_hist;
    sf::Vector2u d_size;
    View d_view;

    ThreadPool d_threadPool;

    int d_iterations;
    bool d_fastDraw;
};

} // close namespace frac

#endif //INCLUDED_FRAC_FRACTAL_HPP
