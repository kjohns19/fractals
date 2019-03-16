#ifndef INCLUDED_FRAC_COLOR_SCHEME_EDITOR_HPP
#define INCLUDED_FRAC_COLOR_SCHEME_EDITOR_HPP

#include <fractals/ui/application.hpp>

#include <gtkmm/builder.h>

#include <string>

namespace frac {

struct ColorSchemeEditor
{
    ColorSchemeEditor() = delete;

    static void configure(
        Application& app,
        Glib::RefPtr<Gtk::Builder> builder,
        const std::string& idPrefix,
        const std::string& toolId);
};

} // close namespace frac

#endif // INCLUDED_FRAC_COLOR_SCHEME_EDITOR_HPP
