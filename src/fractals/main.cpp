#include <fractals/color/color_scheme.hpp>
#include <fractals/color/color_scheme_util.hpp>
#include <fractals/fractal/fractal.hpp>
#include <fractals/fractal/julia.hpp>
#include <fractals/fractal/mandelbrot.hpp>
#include <fractals/ui/application.hpp>
#include <fractals/util/view.hpp>

#include <SFML/Graphics.hpp>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

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

    frac::View view(-0.5, 0.0, 3, 2.5);
    view.fit({wsize.x, wsize.y});

    auto fractal = std::make_unique<frac::Mandelbrot>(wsize);
    fractal->setView(view);
    auto colorScheme = frac::ColorSchemeUtil::loadFromFile("data/colors.json");

    int   gtk_argc = 1;
    char* gtk_args[] = { argv[0], nullptr };
    char** gtk_argv = gtk_args;

    auto app = Gtk::Application::create(
        gtk_argc, gtk_argv, "com.github.kjohns19.fractals");

    frac::Application application(wsize, view, std::move(fractal), colorScheme);
    application.run(app);
    return 0;
}
