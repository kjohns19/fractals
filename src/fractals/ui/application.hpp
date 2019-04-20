#ifndef INCLUDED_FRAC_APPLICATION_HPP
#define INCLUDED_FRAC_APPLICATION_HPP

#include <fractals/ui/fractal_widget.hpp>
#include <fractals/ui/view_manager.hpp>
#include <fractals/util/observer.hpp>

#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/window.h>

#include <SFML/System/Vector2.hpp>

#include <memory>
#include <vector>

namespace frac {

class ColorScheme;
class Fractal;
class ViewChanger;

class Application
{
public:
    Application(
        const sf::Vector2u& windowSize,
        std::unique_ptr<Fractal> fractal,
        ColorScheme colorScheme);

    const sf::Vector2u& windowSize() const { return d_windowSize; }

    FractalWidget& fractalWidget() { return *d_fractalWidget; }
    const FractalWidget& fractalWidget() const { return *d_fractalWidget; }

    Fractal& fractal() { return fractalWidget().fractal(); }
    const Fractal& fractal() const { return fractalWidget().fractal(); }

    ColorScheme& colorScheme()
        { return fractalWidget().colorScheme(); }
    const ColorScheme& colorScheme() const
        { return fractalWidget().colorScheme(); }

    ViewManager& viewManager() { return d_viewManager; }
    const ViewManager& viewManager() const { return d_viewManager; }

    void run(Glib::RefPtr<Gtk::Application> app);

private:
    void initMenu(Glib::RefPtr<Gtk::Builder> builder);
    void initToolbar(Glib::RefPtr<Gtk::Builder> builder);

    sf::Vector2u d_windowSize;
    FractalWidget* d_fractalWidget;
    ViewManager d_viewManager;
    Gtk::Window* d_window;
    std::unique_ptr<Observer<ViewManager&>> d_viewObserver;
    Glib::RefPtr<Gtk::Application> d_app;
};

} // close namespace frac

#endif //INCLUDED_FRAC_APPLICATION_HPP
