#ifndef INCLUDED_FRAC_VIEW_CHANGER_HPP
#define INCLUDED_FRAC_VIEW_CHANGER_HPP

#include <kj/event_handler.hpp>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

namespace sf {

class RenderTarget;

} // close namespace sf

namespace frac {

class Application;

class ViewChanger : public kj::EventHandler
{
public:
    ViewChanger(Application& app);
    ViewChanger(Application& app, std::shared_ptr<kj::EventHandler> next);

    void drawBounds(sf::RenderTarget& target) const;
private:
    virtual bool doHandle(const sf::Event& event) override;

    Application& d_app;
    sf::RectangleShape d_bounds;
    sf::Vector2f d_mouseClickPos;
    sf::Vector2f d_mousePos;
    bool d_drawBounds;
    bool d_drawMode;
};

} // close namespace frac

#endif //INCLUDED_FRAC_VIEW_CHANGER_HPP
