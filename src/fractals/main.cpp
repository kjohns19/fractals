#include <fractals/fractal/Fractal.hpp>
#include <fractals/fractal/Mandelbrot.hpp>
#include <fractals/fractal/Julia.hpp>
#include <fractals/ui/Menu.hpp>
#include <fractals/ui/SFMLWidget.hpp>
#include <fractals/ui/Application.hpp>
#include <fractals/color/ColorScheme.hpp>
#include <fractals/util/View.hpp>

#include <SFML/Graphics.hpp>

#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <memory>

//static sf::Color colorHSV(float h, float s, float v);

static Fractal* createFractal(const sf::Vector2u& size, const View& view);
static ColorScheme* createColorScheme();

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

    View view(-0.5, 0, 3, 2.5);

    int   gtk_argc = 1;
    char* gtk_args[] = { argv[0], nullptr };

    char** gtk_argv = gtk_args;

    auto app = Gtk::Application::create(gtk_argc, gtk_argv, "mandelbrot.set");

    double ratio = 1.0 * wsize.x / wsize.y;
    if (ratio < 1.0 * view.width / view.height)
        view.height = view.width / ratio;
    else
        view.width = view.height * ratio;

    view.left = -0.5 - view.width/2;
    view.top = -view.height/2;

    Application application(wsize, view, createFractal(wsize, view), createColorScheme());

    application.run(app);
    return 0;
}

sf::Color colorRGB(float r, float g, float b)
{
    return sf::Color((int)(255*r), (int)(255*g), (int)(255*b));
}

Fractal* createFractal(const sf::Vector2u& size, const View& view)
{
    Fractal* fractal = new Mandelbrot(size, view);
    //fractal->iterate(1000);
    /*
    double cx = -0.8;
    double cy = 0.156;

    Fractal* fractal = new Julia(size, view,
        [cx, cy](double zx, double zy, double& nx, double& ny) {
            if (zx*zx + zy*zy < 4)
            {
                nx = zx*zx - zy*zy + cx;
                ny = 2*zx*zy + cy;
                return true;
            }
            return false;
        });
    */
    return fractal;
}

ColorScheme* createColorScheme()
{
    ColorScheme* colorScheme = new ColorScheme({
        { 0.0000, sf::Color(0,   7,   100) },
        { 0.1600, sf::Color(32,  107, 203) },
        { 0.4200, sf::Color(237, 255, 255) },
        { 0.6425, sf::Color(255, 170, 0  ) },
        { 0.8575, sf::Color(0,   2,   0  ) }
    });
    /*
    ColorScheme* colorScheme = new ColorScheme({
        {0.9, sf::Color::Black},
        {1.0, sf::Color::Black},
    });
    int count = 100;
    for(int i = 0; i < count; i++)
        colorScheme->add({
            {0.45 * i*2 / count, sf::Color::Black},
            {0.45* (i*2+1) / count, colorHSV(1-1.0*i/count, 1, 1)}
        });
    */
    return colorScheme;
}
