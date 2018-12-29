#ifndef INCLUDED_FRAC_DIALOG_SAVE_IMAGE_HPP
#define INCLUDED_FRAC_DIALOG_SAVE_IMAGE_HPP

#include <fractals/ui/application.hpp>
#include <gtkmm/builder.h>
#include <string>

namespace frac {

struct DialogSaveImage
{
    DialogSaveImage() = delete;

    static void configure(
        Application& app,
        Glib::RefPtr<Gtk::Builder> builder,
        const std::string& idPrefix,
        const std::string& menuId);
};

} // close namespace frac

#endif // INCLUDED_FRAC_DIALOG_SAVE_IMAGE_HPP
