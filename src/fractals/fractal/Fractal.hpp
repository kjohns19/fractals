#ifndef INCLUDED_FRACTAL_HPP
#define INCLUDED_FRACTAL_HPP

namespace sf
{
    template<typename T>
    class Rect;

    class RenderTarget;
}

class ColorScheme;

class Fractal
{
public:
    Fractal():
        d_fastDraw(true) {}
    virtual ~Fractal() {}

    virtual void setView(const sf::Rect<double>& view) = 0;

    virtual void iterate(int count = 1) = 0;

    virtual int iterations() const = 0;

    void setDrawMode(bool fast) { d_fastDraw = fast; }
    bool getDrawMode() const { return d_fastDraw; }

    virtual void draw(sf::RenderTarget& target, const ColorScheme& cs) = 0;
    //virtual void draw2(sf::RenderTarget& target, const ColorScheme& cs) { draw(target, cs); }
private:
    bool d_fastDraw;
};

#endif //INCLUDED_FRACTAL_HPP
