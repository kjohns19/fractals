#include <fractals/ui/dialog_fractal.hpp>

#include <fractals/fractal/julia.hpp>
#include <fractals/fractal/mandelbrot.hpp>
#include <fractals/ui/fractal_widget.hpp>
#include <fractals/util/builder_utils.hpp>
#include <fractals/util/raii_helpers.hpp>

#include <gtkmm/box.h>
#include <gtkmm/combobox.h>
#include <gtkmm/dialog.h>
#include <gtkmm/notebook.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/toolbutton.h>

#include <iostream>

namespace frac {

void DialogFractal::configure(
        Application& app,
        Glib::RefPtr<Gtk::Builder> builder,
        const std::string& idPrefix,
        const std::string& toolId)
{
    using BU = BuilderUtils;

    unsigned int previewW = 300;
    unsigned int previewH = 200;
    sf::Vector2u previewSize(previewW, previewH);

    auto preview = Gtk::manage(
        new FractalWidget(
            {previewW, previewH},
            std::make_unique<Mandelbrot>(previewSize)));
    preview->show();

    View view{0, 0, 4, 4};
    view.fit(previewSize);

    auto& previewArea = BU::getWidget<Gtk::Box>(
        builder, idPrefix + "-preview-area");
    previewArea.pack_start(*preview, Gtk::PACK_SHRINK);


    auto& entryEx = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-entry-exponent");
    auto& entryA = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-entry-a");
    auto& entryB = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-entry-b");

    auto rebuildFractal = [preview, view]() {
        preview->fractal().setView(view);
        preview->fractal().reset();
        preview->fractal().iterate(100);
    };

    auto updateMandelbrot = [&, preview, rebuildFractal, previewSize]() {
        bool fast = preview->fractal().getDrawMode();
        preview->setFractal(std::make_unique<Mandelbrot>(previewSize));
        preview->fractal().setDrawMode(fast);
        rebuildFractal();
    };

    auto updateJulia = [&, preview, rebuildFractal, previewSize]() {
        bool fast = preview->fractal().getDrawMode();
        auto fractal = std::make_unique<Julia>(previewSize);
        fractal->setValue(
            entryA.get_value(),
            entryB.get_value(),
            entryEx.get_value());
        fractal->setDrawMode(fast);
        preview->setFractal(std::move(fractal));
        rebuildFractal();
    };

    auto signalEx = entryEx.signal_changed().connect(updateJulia);
    auto signalA = entryA.signal_changed().connect(updateJulia);
    auto signalB = entryB.signal_changed().connect(updateJulia);

    std::function<void()> updaters[] = {
        updateMandelbrot, updateJulia
    };

    auto& notebook = BU::getWidget<Gtk::Notebook>(
        builder, idPrefix + "-notebook");
    auto& selector = BU::getWidget<Gtk::ComboBox>(
        builder, idPrefix + "-combobox");
    selector.signal_changed().connect(
        [&notebook, &selector, updaters]() {
        int row = selector.get_active_row_number();
        notebook.set_current_page(row);
        updaters[row]();
    });

    auto& dialog = BU::getWidget<Gtk::Dialog>(builder, idPrefix);

    BU::getWidget<Gtk::ToolButton>(builder, toolId)
    .signal_clicked().connect(
        [&app, &dialog, preview, rebuildFractal, previewSize,
         &entryEx, &entryA, &entryB,
         signalEx, signalA, signalB]() mutable {
        PauseFractal pause(app.fractalWidget());

        preview->setFractal(app.fractal().clone(previewSize));
        preview->colorScheme() = app.colorScheme();
        rebuildFractal();

        Julia* julia = dynamic_cast<Julia*>(&app.fractal());
        if (julia)
        {
            SignalBlock bEx(signalEx), bA(signalA), bB(signalB);
            entryEx.set_value(julia->getPower());
            entryA.set_value(julia->getX());
            entryB.set_value(julia->getY());
        }

        int result;
        do
        {
            result = dialog.run();
            if (result == Gtk::RESPONSE_APPLY || result == Gtk::RESPONSE_OK)
            {
                auto& current = app.fractal();
                auto& newFrac = preview->fractal();
                View view = newFrac.getView();
                view.fit(current.getSize());

                int iterations = current.iterations();
                app.fractalWidget().setFractal(
                    newFrac.clone(current.getSize(), view));
                app.viewManager().setView(view, iterations);
                app.viewManager().clearViews();
            }
        }
        while (result == Gtk::RESPONSE_APPLY);

        dialog.hide();
    });
}

} // close namespace frac
