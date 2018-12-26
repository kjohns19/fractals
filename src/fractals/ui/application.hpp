#ifndef INCLUDED_FRAC_APPLICATION_HPP
#define INCLUDED_FRAC_APPLICATION_HPP

#include <fractals/ui/fractal_widget.hpp>
#include <fractals/ui/view_manager.hpp>

#include <gtkmm/application.h>
#include <gtkmm/window.h>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
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
            const View& view,
            std::unique_ptr<Fractal> fractal,
            std::unique_ptr<ColorScheme> colorScheme);

    const sf::Vector2u& getWindowSize() const;

    Gtk::Window& getWindow();
    const Gtk::Window& getWindow() const;

    FractalWidget& fractalWidget() { return *d_fractalWidget; }
    const FractalWidget& fractalWidget() const { return *d_fractalWidget; }

    Fractal& fractal() { return fractalWidget().fractal(); }
    const Fractal& fractal() const { return fractalWidget().fractal(); }

    ColorScheme& colorScheme()
        { return fractalWidget().colorScheme(); }
    const ColorScheme& colorScheme() const
        { return fractalWidget().colorScheme(); }

    ViewManager& getViewManager();
    const ViewManager& getViewManager() const;

    void run(Glib::RefPtr<Gtk::Application> app);
private:
    sf::Vector2u d_windowSize;
    FractalWidget* d_fractalWidget;
    ViewManager d_viewManager;

    Gtk::Window d_window;

    std::shared_ptr<ViewChanger> d_viewChanger;
};

} // close namespace frac

#endif //INCLUDED_FRAC_APPLICATION_HPP
