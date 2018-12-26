#include <fractals/color/color_scheme.hpp>
#include <fractals/fractal/fractal.hpp>
#include <fractals/ui/application.hpp>
#include <fractals/ui/menu.hpp>
#include <fractals/ui/view_changer.hpp>
#include <fractals/util/sfml_widget.hpp>

#include <SFML/Graphics.hpp>

#include <gtkmm/hvbox.h>

#include <cassert>
#include <iostream>

namespace frac {

Application::Application(
        const sf::Vector2u& windowSize,
        const View& view,
        std::unique_ptr<Fractal> fractal,
        std::unique_ptr<ColorScheme> colorScheme)
: d_windowSize(windowSize)
, d_fractalWidget(Gtk::manage(
    new FractalWidget({windowSize.x, windowSize.y}, std::move(fractal))))
, d_viewManager(*d_fractalWidget, view)
{
    d_fractalWidget->colorScheme() = *colorScheme;
    d_fractalWidget->show();

    Gtk::VBox* box = Gtk::manage(new Gtk::VBox());

    Gtk::Widget* menu = createMenu(*this);

    box->pack_start(*menu);
    box->pack_start(*d_fractalWidget, Gtk::PACK_SHRINK);
    box->show();
    d_window.add(*box);

    auto viewChanger = std::make_shared<ViewChanger>(*this, nullptr);
    d_fractalWidget->eventHandler(viewChanger);

    d_fractalWidget->onDraw([this, viewChanger] (FractalWidget& widget) {
        sf::RenderTarget& target = widget.window();
        viewChanger->drawBounds(target);
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

} // close namespace frac
