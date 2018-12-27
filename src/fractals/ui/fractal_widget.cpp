#include <fractals/ui/fractal_widget.hpp>
#include <cassert>

namespace frac {

FractalWidget::FractalWidget(
        sf::VideoMode mode, std::unique_ptr<Fractal> fractal)
: SFMLWidget(mode, "FractalWidget")
, d_drawFunc([] (FractalWidget&) {})
, d_fractal(std::move(fractal))
, d_lastIteration(-1)
, d_running(false)
, d_redraw(true)
{
    assert(d_fractal);

    d_fractalTexture.create(mode.width, mode.height);
    d_fractalSprite.setTexture(d_fractalTexture.getTexture());

    SFMLWidget::onDraw([this] (SFMLWidget& widget) {
        if (d_running)
            d_fractal->iterate();

        if (d_redraw || d_fractal->iterations() != d_lastIteration)
        {
            d_lastIteration = d_fractal->iterations();
            d_fractalTexture.clear();
            d_fractal->draw(d_fractalTexture, d_colorScheme);
            d_fractalTexture.display();
            d_redraw = false;
        }

        sf::RenderTarget& target = window();

        target.clear();
        target.draw(d_fractalSprite);

        d_drawFunc(*this);
    });

    setFramerate(60);
}

void FractalWidget::setFractal(std::unique_ptr<Fractal> fractal)
{
    d_fractal = std::move(fractal);
    d_lastIteration = -1;
}

} // close namespace frac

