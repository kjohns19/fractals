#ifndef INCLUDED_FRAC_DIALOG_FRACTAL_HPP
#define INCLUDED_FRAC_DIALOG_FRACTAL_HPP

#include <fractals/ui/application.hpp>
#include <gtkmm/builder.h>
#include <string>

namespace frac {

struct DialogFractal
{
    DialogFractal() = delete;

    static void configure(
        Application& app,
        Glib::RefPtr<Gtk::Builder> builder,
        const std::string& idPrefix,
        const std::string& toolId);
};

} // close namespace frac

#endif // INCLUDED_FRAC_DIALOG_FRACTAL_HPP
