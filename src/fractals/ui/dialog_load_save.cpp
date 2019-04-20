#include <fractals/ui/dialog_load_save.hpp>

#include <fractals/ui/fractal_widget_util.hpp>
#include <fractals/util/builder_utils.hpp>

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/imagemenuitem.h>

#include <iostream>

namespace frac {

void DialogLoadSave::configure(
        Application& app,
        Glib::RefPtr<Gtk::Builder> builder,
        const std::string& loadIdPrefix,
        const std::string& saveIdPrefix,
        const std::string& loadMenuId,
        const std::string& saveMenuId)
{
    using BU = BuilderUtils;

    auto& loadFilechooser = BU::getWidget<Gtk::FileChooserDialog>(
        builder, loadIdPrefix + "-filechooser");
    auto& saveFilechooser = BU::getWidget<Gtk::FileChooserDialog>(
        builder, saveIdPrefix + "-filechooser");

    BU::getWidget<Gtk::ImageMenuItem>(builder, loadMenuId)
    .signal_activate().connect([&app, &loadFilechooser]() {
        int result = loadFilechooser.run();
        if (result == Gtk::RESPONSE_OK)
        {
            auto filename = loadFilechooser.get_filename();
            FractalWidgetUtil::loadFromFile(filename, &app.fractalWidget());
        }
        loadFilechooser.hide();
    });
    BU::getWidget<Gtk::ImageMenuItem>(builder, saveMenuId)
    .signal_activate().connect([&app, &saveFilechooser]() {
        int result = saveFilechooser.run();
        if (result == Gtk::RESPONSE_OK)
        {
            auto filename = saveFilechooser.get_filename();
            FractalWidgetUtil::saveToFile(filename, app.fractalWidget());
        }
        saveFilechooser.hide();
    });
}

} // close namespace frac

