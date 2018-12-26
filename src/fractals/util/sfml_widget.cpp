#include <fractals/util/sfml_widget.hpp>

#include <gdk/gdkx.h>
#include <glibmm/main.h>

namespace frac {

SFMLWidget::SFMLWidget(sf::VideoMode mode, const char* name)
: Glib::ObjectBase(name)
, d_drawFunc([] (SFMLWidget&) {})
{
    set_size_request(mode.width, mode.height);

    // Makes this behave like an interal object rather then a parent window
    set_has_window(false);

    signal_draw().connect(sigc::bind_return(
                          sigc::hide(
                              sigc::mem_fun(this, &SFMLWidget::doDraw)),
                          true));

    add_events(
        Gdk::BUTTON_PRESS_MASK   |
        Gdk::BUTTON_RELEASE_MASK |
        Gdk::POINTER_MOTION_MASK |
        Gdk::ENTER_NOTIFY_MASK   |
        Gdk::LEAVE_NOTIFY_MASK);

    signal_button_press_event().connect(
        sigc::mem_fun(this, &SFMLWidget::eventButton));
    signal_button_release_event().connect(
        sigc::mem_fun(this, &SFMLWidget::eventButton));
    signal_enter_notify_event().connect(
        sigc::mem_fun(this, &SFMLWidget::eventCrossing));
    signal_leave_notify_event().connect(
        sigc::mem_fun(this, &SFMLWidget::eventCrossing));
    signal_motion_notify_event().connect(
        sigc::mem_fun(this, &SFMLWidget::eventMotion));
}

bool SFMLWidget::eventButton(GdkEventButton* ev)
{
    const sf::Mouse::Button buttons[] = {
        sf::Mouse::Left,
        sf::Mouse::Middle,
        sf::Mouse::Right,
        sf::Mouse::XButton1,
        sf::Mouse::XButton2
    };

    sf::Event event;
    if (ev->type == GDK_BUTTON_RELEASE)
        event.type = sf::Event::MouseButtonReleased;
    else
        event.type = sf::Event::MouseButtonPressed;

    event.mouseButton.button = buttons[ev->button-1];
    event.mouseButton.x = static_cast<int>(ev->x);
    event.mouseButton.y = static_cast<int>(ev->y);
    handleEvent(event);
    return false;
}

bool SFMLWidget::eventMotion(GdkEventMotion* ev)
{
    sf::Event event;
    event.type = sf::Event::MouseMoved;
    event.mouseMove.x = static_cast<int>(ev->x);
    event.mouseMove.y = static_cast<int>(ev->y);
    handleEvent(event);
    return false;
}

bool SFMLWidget::eventCrossing(GdkEventCrossing* ev)
{
    sf::Event event;
    if (ev->type == GDK_ENTER_NOTIFY)
        event.type = sf::Event::MouseEntered;
    else
        event.type = sf::Event::MouseLeft;
    handleEvent(event);
    return false;
}

void SFMLWidget::on_size_allocate(Gtk::Allocation& allocation)
{
    set_allocation(allocation);

    if(d_refGdkWindow)
    {
        d_refGdkWindow->move_resize(
            allocation.get_x(), allocation.get_y(),
            allocation.get_width(), allocation.get_height());
        d_renderWindow.setSize(
            sf::Vector2u(allocation.get_width(), allocation.get_height()));
    }
}

void SFMLWidget::on_realize()
{
    Gtk::Widget::on_realize();

    if (d_refGdkWindow)
        return;

    GdkWindowAttr attributes{};

    Gtk::Allocation allocation = get_allocation();

    attributes.x = allocation.get_x();
    attributes.y = allocation.get_y();
    attributes.width = allocation.get_width();
    attributes.height = allocation.get_height();

    attributes.event_mask = get_events() | Gdk::EXPOSURE_MASK;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.wclass = GDK_INPUT_OUTPUT;

    d_refGdkWindow = Gdk::Window::create(
        get_window(), &attributes, GDK_WA_X | GDK_WA_Y);
    set_has_window(true);
    set_window(d_refGdkWindow);

    unset_background_color();

    set_double_buffered(false);

    //make the widget receive expose events
    d_refGdkWindow->set_user_data(gobj());

    d_renderWindow.create(GDK_WINDOW_XID(d_refGdkWindow->gobj()));
}

void SFMLWidget::on_unrealize()
{
    d_refGdkWindow.clear();
    Gtk::Widget::on_unrealize();
}

void SFMLWidget::display()
{
    if(d_refGdkWindow)
        d_renderWindow.display();
}

void SFMLWidget::invalidate()
{
    if(d_refGdkWindow)
        d_refGdkWindow->invalidate(true);
}

void SFMLWidget::onDraw(const DrawFunc& func)
{
    d_drawFunc = func;
}

void SFMLWidget::doDraw()
{
    d_drawFunc(*this);
    display();
}

void SFMLWidget::doEvent()
{
    sf::Event event;
    while (d_renderWindow.pollEvent(event))
        handleEvent(event);
}

void SFMLWidget::handleEvent(const sf::Event& event)
{
    if (d_eventHandler)
        d_eventHandler->handle(event);
}

void SFMLWidget::setFramerate(int framerate)
{
    if (framerate > 0)
        Glib::signal_timeout().connect(
            sigc::bind_return(
                sigc::mem_fun(this, &SFMLWidget::invalidate),
                true),
            1000/framerate);
}

} // close namespace frac
