#ifndef INCLUDED_SFML_WIDGET_HPP
#define INCLUDED_SFML_WIDGET_HPP

#include <kj/event_handler.hpp>

#include <gtkmm/builder.h>
#include <gtkmm/label.h>
#include <gdkmm/window.h>

#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <sigc++/connection.h>

#include <functional>
#include <memory>

class SFMLWidget : public Gtk::Label
{
public:
    typedef std::function<void(SFMLWidget&)> DrawFunc;

protected:
    virtual void on_size_allocate(Gtk::Allocation& allocation) override;
    virtual void on_realize() override;
    virtual void on_unrealize() override;

public:
    SFMLWidget(sf::VideoMode mode);
    virtual ~SFMLWidget() {}

    void invalidate();
    void display();

    void onDraw(const DrawFunc& func);

    void eventHandler(const std::shared_ptr<kj::EventHandler>& handler) { d_eventHandler = handler; }
    const std::shared_ptr<kj::EventHandler>& eventHandler() const { return d_eventHandler; }

    sf::RenderWindow& window() { return d_renderWindow; }
    const sf::RenderWindow& window() const { return d_renderWindow; }

    void setFramerate(int framerate);

private:
    void doDraw();
    void doEvent();
    void handleEvent(const sf::Event& event);

    bool eventButton(GdkEventButton* ev);
    bool eventMotion(GdkEventMotion* ev);
    bool eventCrossing(GdkEventCrossing* ev);


    sf::RenderWindow d_renderWindow;
    DrawFunc d_drawFunc;
    std::shared_ptr<kj::EventHandler> d_eventHandler;
    Glib::RefPtr<Gdk::Window> d_refGdkWindow;
};

#endif // INCLUDED_SFML_WIDGET_HPP
