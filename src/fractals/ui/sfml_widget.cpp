#include <fractals/ui/sfml_widget.hpp>
#include <iostream>

// Tested on Linux Mint 12.4 and Windows 7
#if defined(SFML_SYSTEM_WINDOWS)

#include <gdk/gdkwin32.h>
#define GET_WINDOW_HANDLE_FROM_GDK GDK_WINDOW_HANDLE

#elif defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD)

#include <gdk/gdkx.h>
#define GET_WINDOW_HANDLE_FROM_GDK GDK_WINDOW_XID

#elif defined(SFML_SYSTEM_MACOS)

#error Note: You have to figure out an analogue way to access the handle of the widget on a Mac-System

#else

#error Unsupported Operating System

#endif
#include <glibmm/main.h>


SFMLWidget::SFMLWidget(sf::VideoMode mode, int size_request):
    d_drawFunc([] (SFMLWidget&) {}),
    d_eventFunc([] (sf::Event) {})
{
    //if(size_request<=0)
    //    size_request = std::max<int>(1, std::min<int>(mode.width, mode.height) / 2);

    //set_size_request(size_request, size_request);
    set_size_request(mode.width, mode.height);

    set_has_window(false); // Makes this behave like an interal object rather then a parent window.

    signal_draw().connect(sigc::bind_return(
                          sigc::hide(
                              sigc::mem_fun(this, &SFMLWidget::doDraw)),
                          true));
    
    add_events(Gdk::BUTTON_PRESS_MASK   |
               Gdk::BUTTON_RELEASE_MASK |
               Gdk::POINTER_MOTION_MASK |
               Gdk::ENTER_NOTIFY_MASK   |
               Gdk::LEAVE_NOTIFY_MASK);

    signal_button_press_event().connect(sigc::mem_fun(this, &SFMLWidget::eventButton));
    signal_button_release_event().connect(sigc::mem_fun(this, &SFMLWidget::eventButton));
    signal_enter_notify_event().connect(sigc::mem_fun(this, &SFMLWidget::eventCrossing));
    signal_leave_notify_event().connect(sigc::mem_fun(this, &SFMLWidget::eventCrossing));
    signal_motion_notify_event().connect(sigc::mem_fun(this, &SFMLWidget::eventMotion));
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
    event.mouseButton.x = (int)ev->x;
    event.mouseButton.y = (int)ev->y;
    d_eventFunc(event);
    return false;
}

/*
bool SFMLWidget::eventKey(GdkEventKey* ev)
{
    sf::Event event;
    if (ev->type == GDK_KEY_PRESS)
        event.type = sf::Event::KeyPressed;
    else
        event.type = sf::Event::KeyReleased;
    d_eventFunc(event);
    return false;
}
*/

bool SFMLWidget::eventMotion(GdkEventMotion* ev)
{
    sf::Event event;
    event.type = sf::Event::MouseMoved;
    event.mouseMove.x = (int)ev->x;
    event.mouseMove.y = (int)ev->y;
    d_eventFunc(event);
    return false;
}

bool SFMLWidget::eventCrossing(GdkEventCrossing* ev)
{
    sf::Event event;
    if (ev->type == GDK_ENTER_NOTIFY)
        event.type = sf::Event::MouseEntered;
    else
        event.type = sf::Event::MouseLeft;
    d_eventFunc(event);
    return false;
}

void SFMLWidget::on_size_allocate(Gtk::Allocation& allocation)
{
    //Do something with the space that we have actually been given:
    //(We will not be given heights or widths less than we have requested, though
    //we might get more)

    this->set_allocation(allocation);

    if(d_refGdkWindow)
    {
        d_refGdkWindow->move_resize(allocation.get_x(),
                                    allocation.get_y(),
                                    allocation.get_width(),
                                    allocation.get_height() );
        d_renderWindow.setSize(sf::Vector2u(allocation.get_width(),
                                          allocation.get_height()));
    }
}

void SFMLWidget::on_realize()
{
    Gtk::Widget::on_realize();

    if(!d_refGdkWindow)
    {
        //Create the GdkWindow:
        GdkWindowAttr attributes;
        memset(&attributes, 0, sizeof(attributes));

        Gtk::Allocation allocation = get_allocation();

        //Set initial position and size of the Gdk::Window:
        attributes.x = allocation.get_x();
        attributes.y = allocation.get_y();
        attributes.width = allocation.get_width();
        attributes.height = allocation.get_height();

        attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;


        d_refGdkWindow = Gdk::Window::create(get_window(), &attributes,
                GDK_WA_X | GDK_WA_Y);
        set_has_window(true);
        set_window(d_refGdkWindow);

        // transparent background
#if GTK_VERSION_GE(3, 0)
        this->unset_background_color();
#else
        this->get_window()->set_back_pixmap(Glib::RefPtr<Gdk::Pixmap>());
#endif

        this->set_double_buffered(false);

        //make the widget receive expose events
        d_refGdkWindow->set_user_data(gobj());

        d_renderWindow.create(GET_WINDOW_HANDLE_FROM_GDK(d_refGdkWindow->gobj()));
    }
}

void SFMLWidget::on_unrealize()
{
  d_refGdkWindow.clear();

  //Call base class:
  Gtk::Widget::on_unrealize();
}

void SFMLWidget::display()
{
    if(d_refGdkWindow)
    {
        d_renderWindow.display();
    }
}

void SFMLWidget::invalidate()
{
    if(d_refGdkWindow)
    {
        d_refGdkWindow->invalidate(true);
    }
}

void SFMLWidget::onDraw(const DrawFunc& func)
{
    d_drawFunc = func;
}

void SFMLWidget::onEvent(const EventFunc& func)
{
    d_eventFunc = func;
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
        d_eventFunc(event);
}

void SFMLWidget::setFramerate(int framerate)
{
    d_updateSignal.disconnect();
    if (framerate > 0)
        Glib::signal_timeout().connect(sigc::bind_return(
                                          sigc::mem_fun(this, &SFMLWidget::invalidate),
                                          true),
                                       1000/framerate);
}
