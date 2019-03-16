#include <fractals/ui/view_changer.hpp>

#include <fractals/ui/application.hpp>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

#include <iostream>

namespace frac {

ViewChanger::ViewChanger(Application& app)
: ViewChanger(app, nullptr) {}

ViewChanger::ViewChanger(Application& app, std::shared_ptr<EventHandler> next)
: EventHandler(next)
, d_app(app)
, d_bounds()
, d_mouseClickPos()
, d_mousePos()
, d_drawBounds(false)
, d_drawMode(false)
{
    d_bounds.setOutlineColor(sf::Color::Blue);
    d_bounds.setFillColor(sf::Color(0, 0, 0, 0));
    d_bounds.setOutlineThickness(1);
}

void ViewChanger::drawBounds(sf::RenderTarget& target) const
{
    if (d_drawBounds)
    {
        target.draw(d_bounds);
        if (!d_drawMode)
        {
            sf::RectangleShape shape;
            shape.setOutlineColor(sf::Color::Red);
            shape.setFillColor(sf::Color(0, 0, 0, 0));
            shape.setOutlineThickness(1);
            shape.setPosition(d_mouseClickPos);
            shape.setSize(d_mousePos - d_mouseClickPos);
            target.draw(shape);
        }
    }
}

bool ViewChanger::doHandle(const sf::Event& event)
{
    const sf::Event::MouseButtonEvent& button = event.mouseButton;
    const sf::Event::MouseMoveEvent& move = event.mouseMove;
    const sf::Vector2u& wsize = d_app.windowSize();
    ViewManager& vm = d_app.viewManager();

    switch(event.type)
    {
        case sf::Event::MouseButtonPressed:
            if (button.button == sf::Mouse::Left || button.button == sf::Mouse::Right)
            {
                if (d_drawBounds)
                    d_drawBounds = false;
                else
                {
                    d_mouseClickPos = sf::Vector2f(button.x, button.y);
                    d_mousePos = d_mouseClickPos;
                    d_bounds.setPosition(d_mouseClickPos);
                    d_bounds.setSize(sf::Vector2f(0, 0));
                    d_drawBounds = true;
                    d_drawMode = (button.button == sf::Mouse::Left);
                }
            }
            else if (button.button == sf::Mouse::Middle && !d_drawBounds)
                vm.previousView();
            break;
        case sf::Event::MouseButtonReleased:
            if (d_drawBounds &&
                    (button.button == sf::Mouse::Left ||
                    button.button == sf::Mouse::Right))
            {
                if (d_drawMode == (button.button == sf::Mouse::Left))
                {
                    if (d_bounds.getSize() != sf::Vector2f(0, 0))
                    {
                        const View& oldView = vm.getView();
                        View  newView(d_bounds.getGlobalBounds());
                        newView.x = oldView.x + oldView.width*(newView.x-wsize.x/2.0)/wsize.x;
                        newView.y = oldView.y + oldView.height*(newView.y-wsize.y/2.0)/wsize.y;
                        newView.width *= oldView.width / wsize.x;
                        newView.height*= oldView.height / wsize.y;
                        newView.fit(wsize);

                        vm.setView(newView);
                    }
                    d_drawBounds = false;
                }
            }
            break;
        case sf::Event::MouseMoved:
            if (d_drawBounds)
            {
                d_mousePos = sf::Vector2f(move.x, move.y);
                sf::Vector2f difference = d_mousePos - d_mouseClickPos;
                if (difference.x < 0)
                    difference.x = -difference.x;
                if (difference.y < 0)
                    difference.y = -difference.y;

                float ratio = 1.0 * wsize.x / wsize.y;

                sf::Vector2f size(std::max(difference.x, difference.y * ratio),
                                  std::max(difference.y, difference.x / ratio));

                if (d_drawMode)
                {
                    d_bounds.setPosition(d_mouseClickPos - size);
                    d_bounds.setSize(size*2.0f);
                }
                else
                {
                    sf::Vector2f center = d_mouseClickPos + (d_mousePos - d_mouseClickPos) / 2.0f;
                    d_bounds.setPosition(center - size / 2.0f);
                    d_bounds.setSize(size);
                }
            }
            break;
        default:
            return false;
    }
    return true;
}

} // close namespace frac
