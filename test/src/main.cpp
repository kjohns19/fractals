#include <sfml_widget.hpp>

#include <gtkmm.h>
#include <SFML/Graphics.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create(argc, argv, "Example");
    auto builder = Gtk::Builder::create();
    builder->add_from_file("example.glade");

    SFMLWidget* sfmlWindow =
        Gtk::manage(new SFMLWidget(sf::VideoMode(500, 500)));

    Gtk::Box* box = nullptr;
    builder->get_widget("box", box);
    if (!box)
    {
        std::cout << "box null" << std::endl;
        return 1;
    }

    box->pack_start(*sfmlWindow, Gtk::PACK_SHRINK);
    sfmlWindow->show();

    sfmlWindow->onDraw([](SFMLWidget& widget) {
        sf::CircleShape circle;
        circle.setRadius(50);
        circle.setPosition(100, 100);
        circle.setFillColor(sf::Color::Green);

        auto& target = widget.window();
        target.clear();
        target.draw(circle);
    });

    Gtk::Window* window = nullptr;
    builder->get_widget("window", window);
    if (!window)
    {
        std::cout << "Oops window" << std::endl;
        return 1;
    }

    app->run(*window);

    return 0;
}
