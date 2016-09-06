#ifndef INCLUDED_FRAC_MENU_HPP
#define INCLUDED_FRAC_MENU_HPP

namespace Gtk {

class Widget;

} // close namespace Gtk

namespace frac {

class Application;

Gtk::Widget* createMenu(Application& app);

} // close namespace frac

#endif //INCLUDED_FRAC_MENU_HPP
