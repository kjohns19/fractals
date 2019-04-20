#ifndef INCLUDED_FRAC_DIALOG_LOAD_SAVE_HPP
#define INCLUDED_FRAC_DIALOG_LOAD_SAVE_HPP

#include <fractals/ui/application.hpp>

#include <gtkmm/builder.h>

#include <string>

namespace frac {

struct DialogLoadSave
{
    DialogLoadSave() = delete;

    static void configure(
        Application& app,
        Glib::RefPtr<Gtk::Builder> builder,
        const std::string& loadIdPrefix,
        const std::string& saveIdPrefix,
        const std::string& loadMenuId,
        const std::string& saveMenuId);
};

} // close namespace frac

#endif // INCLUDED_FRAC_DIALOG_LOAD_SAVE_HPP
