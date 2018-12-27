#ifndef INCLUDED_FRAC_BUILDER_UTILS_HPP
#define INCLUDED_FRAC_BUILDER_UTILS_HPP

#include <gtkmm/builder.h>
#include <exception>
#include <string>

namespace frac {

struct BuilderUtils
{
    BuilderUtils() = delete;

    template<typename T>
    static T& getWidget(
            Glib::RefPtr<Gtk::Builder> builder, const std::string& name)
    {
        T* widget = nullptr;
        builder->get_widget(name, widget);
        if (!widget)
            throw std::runtime_error("Failed to get widget named " + name);
        return *widget;
    }

    template<typename T>
    static Glib::RefPtr<T> getObject(
            Glib::RefPtr<Gtk::Builder> builder, const std::string& name)
    {
        auto obj = Glib::RefPtr<T>::cast_dynamic(builder->get_object(name));
        if (!obj)
            throw std::runtime_error("Failed to get object named " + name);
        return obj;
    }
};

} // close namespace frac

#endif // INCLUDED_FRAC_BUILDER_UTILS_HPP
