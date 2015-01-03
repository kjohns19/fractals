#include <fractals/color/GradientColorScheme.hpp>
#include <fractals/fractal/Fractal.hpp>
#include <fractals/fractal/Mandelbrot.hpp>
#include <fractals/ui/Menu.hpp>
#include <fractals/ui/SFMLWidget.hpp>
#include <fractals/ui/Application.hpp>

#include <SFML/Graphics.hpp>

#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <memory>

void iterate(sf::Image& image, const sf::Rect<double>& bounds, double cx, double cy, int count);

sf::Color colorHSV(float h, float s, float v);

std::ostream& operator<<(std::ostream& out, const sf::Rect<double>& r)
{
    out << "[" << r.left << ", " << r.top << ", "
        << r.width-r.left << ", " << r.height-r.top << "]";
    return out;
}

Fractal* createFractal(const sf::Vector2u& size,
                       const sf::Rect<double>& view);
ColorScheme* createColorScheme();

void save(const sf::Rect<double>& view, int current_it, const ColorScheme& colorScheme);

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create(argc, argv, "mandelbrot.set");

    sf::Vector2u wsize(1280, 720);
    sf::Rect<double> view(-2, -1, 1.92, 1.08);

    Application application(wsize, view, createFractal(wsize, view), createColorScheme());

    application.run(app);
    return 0;
}

sf::Color colorRGB(float r, float g, float b)
{
    return sf::Color((int)(255*r), (int)(255*g), (int)(255*b));
}

sf::Color colorHSV(float h, float s, float v)
{
    if (s == 0)
        return sf::Color(v, v, v);

    h/=0.2;
    int i = std::floor(h);
    float f = h - i;
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));

    if (i == 0)
        return colorRGB(v, t, p);
    else if (i == 1)
        return colorRGB(q, v, p);
    else if (i == 2)
        return colorRGB(p, v, t);
    else if (i == 3)
        return colorRGB(p, q, v);
    else if (i == 4)
        return colorRGB(t, p, v);
    else
        return colorRGB(v, p, q);
}

void save(const sf::Rect<double>& view, int current_it, const ColorScheme& colorScheme)
{
    bool okay = false;
    unsigned width, height;
    int iteration;
    int frames;
    std::string filename;
    while(!okay)
    {
        std::cout << "Enter dimensions: ";
        std::cin >> width >> height;
        std::cout << "Enter start iteration (" << current_it << " current): " << std::endl;
        std::cin >> iteration;
        std::cout << "Enter frame count: " << std::endl;
        std::cin >> frames;
        std::cout << "Enter filename base: ";
        std::cin >> filename;

        std::cout << "Will create " << frames << " mandelbrot set image(s) at "
                  << view << " of size " << width << "x" << height
                  << " starting at iteration " << iteration
                  << " and save to " << filename << std::endl;
        std::cout << "Is this correct? ";
        std::cin >> okay;
    }

    std::cout << "Iterating..." << std::endl;

    sf::Image image;
    Mandelbrot fractal(sf::Vector2u(width, height), view);
    fractal.iterate(iteration-1);

    for(int i = 0; i < frames; i++)
    {
        std::stringstream ss;
        ss << filename;
        if (frames > 1)
            ss << std::setfill('0') << std::setw(3) << i;
        ss << ".png";

        fractal.iterate();

        std::cout << "Creating image " << i+1 << std::endl;
        //fractal.toImage2(image, colorScheme);

        std::cout << "Saving image " << i+1 << " to " << ss.str() << "..." << std::endl;
        if (!image.saveToFile(ss.str()))
            std::cout << "Failed to save" << std::endl;
    }
}

Fractal* createFractal(const sf::Vector2u& size,
                                       const sf::Rect<double>& view)
{
    Fractal* fractal = new Mandelbrot(size, view);
    //fractal->iterate(1000);
    return fractal;
}
ColorScheme* createColorScheme()
{
    ColorScheme* colorScheme = new GradientColorScheme({
        { 0.0000, sf::Color(0,   7,   100) },
        { 0.1600, sf::Color(32,  107, 203) },
        { 0.4200, sf::Color(237, 255, 255) },
        { 0.6425, sf::Color(255, 170, 0  ) },
        { 0.8575, sf::Color(0,   2,   0  ) }
    });
    /*
    GradientColorScheme* colorScheme = new GradientColorScheme({
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
