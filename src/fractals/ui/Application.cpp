#include <fractals/color/ColorScheme.hpp>
#include <fractals/fractal/Fractal.hpp>
#include <fractals/ui/Application.hpp>
#include <fractals/ui/SFMLWidget.hpp>
#include <fractals/ui/Menu.hpp>
#include <fractals/ui/ViewChanger.hpp>

#include <SFML/Graphics.hpp>

#include <cassert>
#include <iostream>

Application::Application(const sf::Vector2u& windowSize,
                         const View& view,
                         Fractal* fractal,
                         ColorScheme* colorScheme):
    d_fractal(fractal),
    d_colorScheme(colorScheme),
    d_windowSize(windowSize),
    d_viewManager(*this, view),
    d_lastIteration(-1),
    d_redraw(false),
    d_paused(false),
    d_iterateAmount(1)
{
    assert(fractal != nullptr);
    assert(colorScheme != nullptr);

    d_fractalTexture.create(windowSize.x, windowSize.y);
    d_fractalSprite.setTexture(d_fractalTexture.getTexture());

    SFMLWidget* sfmlWindow = Gtk::manage(new SFMLWidget(sf::VideoMode(windowSize.x, windowSize.y)));
    sfmlWindow->show();

    Gtk::VBox* box = Gtk::manage(new Gtk::VBox());

    Gtk::Widget* menu = createMenu(*this);

    box->pack_start(*menu);
    box->pack_start(*sfmlWindow, Gtk::PACK_SHRINK);
    box->show();
    d_window.add(*box);

    d_viewChanger = std::make_shared<ViewChanger>(*this, nullptr);

    sfmlWindow->onEvent([this] (sf::Event event) {
        d_viewChanger->handle(event);
    });

    sfmlWindow->setFramerate(60);
    sfmlWindow->onDraw([this] (SFMLWidget& widget) {
        if (!d_paused)
            d_fractal->iterate(d_iterateAmount);

        if (d_redraw || d_fractal->iterations() != d_lastIteration)
        {
            d_lastIteration = d_fractal->iterations();
            d_fractalTexture.clear();
            d_fractal->draw(d_fractalTexture, *d_colorScheme);
            d_fractalTexture.display();
            d_redraw = false;
        }

        sf::RenderTarget& target = widget.window();

        target.clear();
        target.draw(d_fractalSprite);

        d_viewChanger->drawBounds(target);
    });

    d_window.show();
}

const sf::Vector2u& Application::getWindowSize() const
{
    return d_windowSize;
}

Gtk::Window& Application::getWindow()
{
    return d_window;
}
const Gtk::Window& Application::getWindow() const
{
    return d_window;
}

void Application::setPaused(bool paused)
{
    d_paused = paused;
}
bool Application::isPaused() const
{
    return d_paused;
}

void Application::setIterateAmount(int iterations)
{
    if (iterations >= 0)
        d_iterateAmount = iterations;
}
int Application::getIterateAmount() const
{
    return d_iterateAmount;
}

void Application::setFractal(Fractal* fractal)
{
    assert(fractal != nullptr);
    d_fractal.reset(fractal);
    d_lastIteration = -1;
}
Fractal& Application::getFractal()
{
    return *d_fractal;
}
const Fractal& Application::getFractal() const
{
    return *d_fractal;
}

void Application::setColorScheme(ColorScheme* colorScheme)
{
    assert(colorScheme != nullptr);
    d_colorScheme.reset(colorScheme);
    redrawFractal();
}
ColorScheme& Application::getColorScheme()
{
    return *d_colorScheme;
}
const ColorScheme& Application::getColorScheme() const
{
    return *d_colorScheme;
}


void Application::run(Glib::RefPtr<Gtk::Application> app)
{
    app->run(d_window);
}

ViewManager& Application::getViewManager()
{
    return d_viewManager;
}
const ViewManager& Application::getViewManager() const
{
    return d_viewManager;
}

void Application::redrawFractal()
{
    d_redraw = true;
}
