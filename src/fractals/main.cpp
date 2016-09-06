#include <fractals/fractal/fractal.hpp>
#include <fractals/fractal/mandelbrot.hpp>
#include <fractals/fractal/julia.hpp>
#include <fractals/ui/application.hpp>
#include <fractals/color/color_scheme.hpp>
#include <fractals/util/view.hpp>

#include <SFML/Graphics.hpp>

#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <memory>
#include <thread>

namespace {

std::unique_ptr<frac::Fractal> createFractal(const sf::Vector2u& size, const frac::View& view);
std::unique_ptr<frac::ColorScheme> createColorScheme();

} // close anonymous namespace

int main(int argc, char* argv[])
{
    sf::Vector2u wsize(1400*3/5, 1100*3/5);
    if (argc >= 3)
    {
        int w = std::atoi(argv[1]);
        int h = std::atoi(argv[2]);
        if (w > 0 && h > 0)
        {
            wsize.x = w;
            wsize.y = h;
        }
    }

    frac::View view(0, 0, 3, 2.5);

    int   gtk_argc = 1;
    char* gtk_args[] = { argv[0], nullptr };

    char** gtk_argv = gtk_args;

    auto app = Gtk::Application::create(gtk_argc, gtk_argv, "mandelbrot.set");

    view.fit({wsize.x, wsize.y});

    auto fractal = createFractal(wsize, view);
    auto colorScheme = createColorScheme();
    frac::Application application(wsize, view, std::move(fractal), std::move(colorScheme));

    application.run(app);
    return 0;
}

namespace {

std::unique_ptr<frac::Fractal> createFractal(const sf::Vector2u& size, const frac::View& view)
{
    std::unique_ptr<frac::Fractal> fractal;
    if (1)
    {
        fractal = std::make_unique<frac::Mandelbrot>(size);
    }
    else
    {
        long double cx, cy;
        cx = 0.285;
        cy = 0.01;
        fractal = std::make_unique<frac::Julia>(size, cx, cy);
    }
    fractal->setView(view);
    return fractal;
}

std::unique_ptr<frac::ColorScheme> createColorScheme()
{
    return std::make_unique<frac::ColorScheme>(
        std::initializer_list<frac::ColorScheme::ColorValue>{
            { 0.0000, sf::Color(0,   7,   100) },
            { 0.1600, sf::Color(32,  107, 203) },
            { 0.4200, sf::Color(237, 255, 255) },
            { 0.6425, sf::Color(255, 170, 0  ) },
            { 0.8575, sf::Color(0,   2,   0  ) }
    });
}

} // close anonymous namespace
