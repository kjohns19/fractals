#ifndef INCLUDED_MENU_HPP
#define INCLUDED_MENU_HPP

namespace Gtk
{
    class Widget;
}

class Application;

Gtk::Widget* createMenu(Application& app);

#endif //INCLUDED_MENU_HPP
