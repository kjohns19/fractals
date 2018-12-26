#ifndef INCLUDED_FRAC_FRACTAL_WIDGET_HPP
#define INCLUDED_FRAC_FRACTAL_WIDGET_HPP

#include <fractals/color/color_scheme.hpp>
#include <fractals/fractal/fractal.hpp>
#include <fractals/util/sfml_widget.hpp>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/VideoMode.hpp>

#include <memory>

namespace frac {

class FractalWidget : public SFMLWidget
{
public:
    using DrawFunc = std::function<void(FractalWidget&)>;

    FractalWidget(sf::VideoMode mode, std::unique_ptr<Fractal> fractal);

    void setFractal(std::unique_ptr<Fractal> fractal);
    Fractal& fractal() { return *d_fractal; }
    const Fractal& fractal() const { return *d_fractal; }

    ColorScheme& colorScheme() { return d_colorScheme; }
    const ColorScheme& colorScheme() const { return d_colorScheme; }

    // Hides SFMLWidget::onDraw
    void onDraw(const DrawFunc& func) { d_drawFunc = func; }

    void setRunning(bool running) { d_running = running; }
    bool isRunning() const { return d_running; }

    void redraw() { d_redraw = true; }
private:
    DrawFunc d_drawFunc;

    std::unique_ptr<Fractal> d_fractal;
    ColorScheme d_colorScheme;

    sf::RenderTexture d_fractalTexture;
    sf::Sprite d_fractalSprite;

    int d_lastIteration;
    bool d_running;
    bool d_redraw;
};

} // close namespace frac

#endif // INCLUDED_FRAC_FRACTAL_WIDGET_HPP
