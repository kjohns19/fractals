#include <fractals/ui/dialog_save_image.hpp>

#include <fractals/util/builder_utils.hpp>
#include <fractals/util/raii_helpers.hpp>

#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/spinbutton.h>

#include <iostream>

namespace frac {
namespace {

void save(
        Gtk::ProgressBar& progressGlobal,
        Gtk::ProgressBar& progressPartial,
        const std::string& filename,
        Fractal& fractal,
        const ColorScheme& cs,
        const sf::Vector2u& size,
        const View& view, int iterations);

} // close anonymous namespace

void DialogSaveImage::configure(
        Application& app,
        Glib::RefPtr<Gtk::Builder> builder,
        const std::string& idPrefix,
        const std::string& menuId)
{
    using BU = BuilderUtils;

    auto& saveButton = BU::getWidget<Gtk::Button>(
        builder, idPrefix + "-button");

    auto& filechooser = BU::getWidget<Gtk::FileChooserDialog>(
        builder, idPrefix + "-filechooser");
    auto& filenameEntry = BU::getWidget<Gtk::Entry>(
        builder, idPrefix + "-entry-filename");
    BU::getWidget<Gtk::Button>(builder, idPrefix + "-button-choose-file")
    .signal_clicked().connect([&saveButton, &filechooser, &filenameEntry]() {
        int result = filechooser.run();
        if (result == Gtk::RESPONSE_OK)
        {
            auto filename = filechooser.get_filename();
            filenameEntry.set_text(filename);
            saveButton.set_sensitive(!filename.empty());
        }
        filechooser.hide();
    });

    auto& entryIterations = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-entry-iterations");
    auto& entryWidth = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-entry-width");
    auto& entryHeight = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-entry-height");

    auto& dialog = BU::getWidget<Gtk::Dialog>(builder, idPrefix);
    auto& progressGlobal = BU::getWidget<Gtk::ProgressBar>(
        builder, idPrefix + "-progress-global");
    auto& progressPartial = BU::getWidget<Gtk::ProgressBar>(
        builder, idPrefix + "-progress-partial");

    BU::getWidget<Gtk::ImageMenuItem>(builder, menuId)
    .signal_activate().connect([&]() {
        PauseFractal pause(app.fractalWidget());

        auto& fractal = app.fractal();

        bool startEmpty = filenameEntry.get_text().empty();
        if (startEmpty)
        {
            filechooser.set_filename("");
            saveButton.set_sensitive(false);
            entryWidth.set_value(fractal.getSize().x);
            entryHeight.set_value(fractal.getSize().y);
        }

        entryIterations.set_value(fractal.iterations());

        int result = dialog.run();
        if (result == Gtk::RESPONSE_OK)
        {
            sf::Vector2u size(
                entryWidth.get_value_as_int(),
                entryHeight.get_value_as_int());
            save(
                progressGlobal, progressPartial,
                filechooser.get_filename(),
                fractal, app.colorScheme(), size,
                app.viewManager().getView(),
                entryIterations.get_value_as_int());
        }

        dialog.hide();

        if (startEmpty && result != Gtk::RESPONSE_OK)
            filenameEntry.set_text("");
    });
}

namespace {

void save(
        Gtk::ProgressBar& progressGlobal,
        Gtk::ProgressBar& progressPartial,
        const std::string& filename,
        Fractal& fractal,
        const ColorScheme& cs,
        const sf::Vector2u& size,
        const View& view, int iterations)
{
    // TODO use progress bars
    std::cout << "Creating fractal" << std::endl;
    auto newfractal = fractal.clone(size, view);
    std::cout << "Iterating" << std::endl;
    int stepCount = 100;
    for(int total = 0; total < iterations;)
    {
        int iterationCount = std::min(stepCount, iterations - total);
        newfractal->iterate(iterationCount);
        total+=iterationCount;
        std::cout << total << "/" << iterations  << std::endl;
    }
    newfractal->setDrawMode(false);

    std::cout << "Creating image" << std::endl;

    sf::RenderTexture texture;
    if (texture.create(size.x, size.y))
    {
        progressPartial.set_fraction(1.0/3);
        newfractal->draw(texture, cs);
        texture.display();
        progressPartial.set_fraction(2.0/3);

        sf::Image image = texture.getTexture().copyToImage();
        progressPartial.set_fraction(0);

        std::cout << "Saving image" << std::endl;
        image.saveToFile(filename);
        std::cout << "Done" << std::endl;
    }
    else
        std::cout << "Failed to create image" << std::endl;
}

} // close anonymous namespace
} // close namespace frac
