#include <fractals/ui/ViewChanger.hpp>
#include <fractals/ui/Application.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Event.hpp>


ViewChanger::ViewChanger(Application& app, std::shared_ptr<EventHandler> next):
    EventHandler(next),
    d_app(app),
    d_bounds(),
    d_mousePos(),
    d_drawBounds(false)
{
    d_bounds.setOutlineColor(sf::Color::Blue);
    d_bounds.setFillColor(sf::Color(0, 0, 0, 0));
    d_bounds.setOutlineThickness(1);
}

void ViewChanger::drawBounds(sf::RenderTarget& target) const
{
    if (d_drawBounds)
        target.draw(d_bounds);
}

bool ViewChanger::doHandle(const sf::Event& event)
{
    const sf::Event::MouseButtonEvent& button = event.mouseButton;
    const sf::Event::MouseMoveEvent& move = event.mouseMove;
    const sf::Vector2u& wsize = d_app.getWindowSize();
    ViewManager& vm = d_app.getViewManager();

    switch(event.type)
    {
        case sf::Event::MouseButtonPressed:
            if (button.button == sf::Mouse::Left)
            {
                d_mousePos = sf::Vector2f(button.x, button.y);
                d_bounds.setPosition(d_mousePos);
                d_bounds.setSize(sf::Vector2f(0, 0));
                d_drawBounds = true;
            }
            else if (button.button == sf::Mouse::Right)
            {
                if (!d_drawBounds)
                    vm.previousView();
                else
                    d_drawBounds = false;
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (button.button == sf::Mouse::Left && d_drawBounds)
            {
                if (d_bounds.getSize() != sf::Vector2f(0, 0))
                {
                    const Application::View& oldView = vm.getView();
                    Application::View  newView(d_bounds.getGlobalBounds());
                    newView.left   = oldView.left + oldView.width * newView.left / wsize.x;
                    newView.top    = oldView.top + oldView.height * newView.top / wsize.y;
                    newView.width *= oldView.width / wsize.x;
                    newView.height*= oldView.height / wsize.y;

                    vm.setView(newView);
                }
                d_drawBounds = false;
            }
            break;
        case sf::Event::MouseMoved:
            if (d_drawBounds)
            {
                sf::Vector2f mouse(move.x, move.y);
                sf::Vector2f difference = mouse - d_mousePos;
                if (difference.x < 0)
                    difference.x = -difference.x;
                if (difference.y < 0)
                    difference.y = -difference.y;

                float ratio = 1.0 * wsize.x / wsize.y;

                sf::Vector2f size(std::max(difference.x, difference.y * ratio),
                                    std::max(difference.y, difference.x / ratio));

                d_bounds.setPosition(d_mousePos - size);
                d_bounds.setSize(size*2.0f);
            }
            break;
        default:
            return false;
    }
    return true;
}
