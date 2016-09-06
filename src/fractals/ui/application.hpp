#ifndef INCLUDED_FRAC_APPLICATION_HPP
#define INCLUDED_FRAC_APPLICATION_HPP

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

    void setPaused(bool paused = true);
    bool isPaused() const;

    void setIterateAmount(int iterations);
    int getIterateAmount() const;

    Gtk::Window& getWindow();
    const Gtk::Window& getWindow() const;

    void setFractal(std::unique_ptr<Fractal> fractal);
    Fractal& getFractal();
    const Fractal& getFractal() const;

    void setColorScheme(ColorScheme* colorScheme);
    ColorScheme& getColorScheme();
    const ColorScheme& getColorScheme() const;

    ViewManager& getViewManager();
    const ViewManager& getViewManager() const;

    void redrawFractal();

    void run(Glib::RefPtr<Gtk::Application> app);
private:
    std::unique_ptr<Fractal> d_fractal;
    std::unique_ptr<ColorScheme> d_colorScheme;
    sf::Vector2u d_windowSize;
    ViewManager d_viewManager;
    Gtk::Window d_window;

    int d_lastIteration;
    bool d_redraw;

    bool d_paused;
    int d_iterateAmount;

    sf::RenderTexture d_fractalTexture;
    sf::Sprite d_fractalSprite;
    std::shared_ptr<ViewChanger> d_viewChanger;
};

} // close namespace frac

#endif //INCLUDED_FRAC_APPLICATION_HPP
