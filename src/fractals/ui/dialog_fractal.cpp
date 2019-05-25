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


    auto& juliaEntryEx = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-julia-entry-exponent");
    auto& juliaEntryA = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-julia-entry-a");
    auto& juliaEntryB = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-julia-entry-b");

    auto& mandelbrotEntryEx = BU::getWidget<Gtk::SpinButton>(
        builder, idPrefix + "-mandelbrot-entry-exponent");

    auto rebuildFractal = [preview, view]() {
        preview->fractal().setView(view);
        preview->fractal().reset();
        preview->fractal().iterate(100);
    };

    auto updateMandelbrot = [&, preview, rebuildFractal, previewSize]() {
        bool fast = preview->fractal().getDrawMode();
        auto fractal = std::make_unique<Mandelbrot>(previewSize);
        fractal->setPower(mandelbrotEntryEx.get_value());
        fractal->setDrawMode(fast);
        preview->setFractal(std::move(fractal));
        rebuildFractal();
    };

    auto mandelbrotSignalEx = mandelbrotEntryEx.signal_changed().connect(updateMandelbrot);

    auto updateJulia = [&, preview, rebuildFractal, previewSize]() {
        bool fast = preview->fractal().getDrawMode();
        auto fractal = std::make_unique<Julia>(previewSize);
        fractal->setValue(
            juliaEntryA.get_value(),
            juliaEntryB.get_value(),
            juliaEntryEx.get_value());
        fractal->setDrawMode(fast);
        preview->setFractal(std::move(fractal));
        rebuildFractal();
    };

    auto juliaSignalEx = juliaEntryEx.signal_changed().connect(updateJulia);
    auto juliaSignalA = juliaEntryA.signal_changed().connect(updateJulia);
    auto juliaSignalB = juliaEntryB.signal_changed().connect(updateJulia);

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
        [&, preview, rebuildFractal, previewSize,
         mandelbrotSignalEx,
         juliaSignalEx, juliaSignalA, juliaSignalB]() mutable {
        PauseFractal pause(app.fractalWidget());

        preview->setFractal(app.fractal().clone(previewSize));
        preview->colorScheme() = app.colorScheme();
        rebuildFractal();

        Mandelbrot* mandelbrot = dynamic_cast<Mandelbrot*>(&app.fractal());
        if (mandelbrot)
        {
            SignalBlock bEx(mandelbrotSignalEx);
            mandelbrotEntryEx.set_value(mandelbrot->getPower());
        }

        Julia* julia = dynamic_cast<Julia*>(&app.fractal());
        if (julia)
        {
            SignalBlock bEx(juliaSignalEx), bA(juliaSignalA), bB(juliaSignalB);
            juliaEntryEx.set_value(julia->getPower());
            juliaEntryA.set_value(julia->getX());
            juliaEntryB.set_value(julia->getY());
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
