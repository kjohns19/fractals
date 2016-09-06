#ifndef INCLUDED_FRAC_MENU_UTILS_HPP
#define INCLUDED_FRAC_MENU_UTILS_HPP

#include <gtkmm/button.h>
#include <gtkmm/hvbox.h>
#include <gtkmm/toolbutton.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/box.h>

#include <functional>

namespace frac {

template<typename ButtonType = Gtk::Button>
ButtonType* createButton(const std::string& label, const std::string& stock);
template<typename ButtonType = Gtk::Button>
ButtonType* createButton(Gtk::Box* menu, const std::string& label, const std::string& stock, const std::function<void()>& func);

template<typename ButtonType = Gtk::ToolButton>
ButtonType* createToolButton(const std::string& label, const std::string& icon);
template<typename ButtonType = Gtk::ToolButton>
ButtonType* createToolButton(Gtk::Toolbar* toolbar, const std::string& label, const std::string& icon, const std::function<void()>& func);



template<typename ButtonType>
ButtonType* createButton(const std::string& label, const std::string& stock)
{
    ButtonType* button;

    button = Gtk::manage(new ButtonType());

    if (stock.empty())
        button->set_label(label);
    else
    {
        button->set_image_from_icon_name(stock, Gtk::ICON_SIZE_BUTTON);
        button->set_tooltip_text(label);
    }

    return button;
}

template<typename ButtonType>
ButtonType* createButton(Gtk::Box* menu, const std::string& label, const std::string& stock, const std::function<void()>& func)
{
    ButtonType* button = createButton<ButtonType>(label, stock);
    button->signal_clicked().connect(func);
    menu->pack_start(*button);
    return button;
}

template<typename ButtonType>
ButtonType* createToolButton(const std::string& label, const std::string& icon)
{
    ButtonType* button;

    button = Gtk::manage(new ButtonType());
    button->set_icon_name(icon);
    button->set_tooltip_text(label);

    return button;
}
template<typename ButtonType>
ButtonType* createToolButton(Gtk::Toolbar* toolbar, const std::string& label, const std::string& icon, const std::function<void()>& func)
{
    ButtonType* button = createToolButton(label, icon);
    button->signal_clicked().connect(func);
    toolbar->append(*button);
    return button;
}

} // close namespace frac

#endif //INCLUDED_FRAC_MENU_UTILS_HPP
